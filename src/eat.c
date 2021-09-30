/*	SCCS Id: @(#)eat.c	3.4	2003/02/13	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
/* #define DEBUG */	/* uncomment to enable new eat code debugging */

#ifdef DEBUG
# ifdef WIZARD
#define debugpline	if (wizard) pline
# else
#define debugpline	pline
# endif
#endif

STATIC_PTR int eatmdone(void);
STATIC_PTR int eatfood(void);
STATIC_PTR void costly_tin(const char*);
STATIC_PTR int opentin(void);
STATIC_PTR int unfaint(void);

#ifdef OVLB
STATIC_DCL const char *food_xname(struct obj *,BOOLEAN_P);
STATIC_DCL const char *Food_xname(struct obj *,BOOLEAN_P);
STATIC_DCL void choke(struct obj *);
STATIC_DCL void recalc_wt(void);
STATIC_DCL struct obj *touchfood(struct obj *);
STATIC_DCL void do_reset_eat(void);
STATIC_DCL void done_eating(BOOLEAN_P);
STATIC_DCL void cprefx(int);
STATIC_DCL int intrinsic_possible(int,struct permonst *);
STATIC_DCL void givit(int,struct permonst *);
STATIC_DCL void cpostfx(int);
STATIC_DCL void start_tin(struct obj *);
STATIC_DCL int eatcorpse(struct obj *);
STATIC_DCL void start_eating(struct obj *);
STATIC_DCL void fprefx(struct obj *);
STATIC_DCL void accessory_has_effect(struct obj *);
STATIC_DCL void fpostfx(struct obj *);
STATIC_DCL int bite(void);
STATIC_DCL int edibility_prompts(struct obj *);
STATIC_DCL int rottenfood(struct obj *);
STATIC_DCL void eatspecial(void);
STATIC_DCL void eataccessory(struct obj *);
STATIC_DCL const char *foodword(struct obj *);
STATIC_DCL boolean maybe_cannibal(int,BOOLEAN_P);
STATIC_DCL struct obj *floorfood(const char *);
STATIC_DCL boolean cannibal_allowed(void);

char msgbuf[BUFSZ];

#endif /* OVLB */

/* hunger texts used on bottom line (each 8 chars long) */
#define SATIATED	0
#define NOT_HUNGRY	1
#define HUNGRY		2
#define WEAK		3
#define FAINTING	4
#define FAINTED		5
#define STARVED		6

#ifndef OVLB

STATIC_DCL NEARDATA const char comestibles[];
STATIC_DCL NEARDATA const char allobj[];
STATIC_DCL boolean force_save_hs;

#else

STATIC_OVL NEARDATA const char comestibles[] = { FOOD_CLASS, 0 };

/* Gold must come first for getobj(). */
STATIC_OVL NEARDATA const char allobj[] = { COIN_CLASS, ALLOW_FLOOROBJ,
	WEAPON_CLASS, ARMOR_CLASS, POTION_CLASS, SCROLL_CLASS,
	WAND_CLASS, RING_CLASS, AMULET_CLASS, IMPLANT_CLASS, FOOD_CLASS, TOOL_CLASS,
	GEM_CLASS, ROCK_CLASS, BALL_CLASS, CHAIN_CLASS, VENOM_CLASS, SPBOOK_CLASS, 0 };

STATIC_OVL boolean force_save_hs = FALSE;

const char *hu_stat[] = {
	"Satiated",
	"        ",
	"Hungry  ",
	"Weak    ",
	"Fainting",
	"Fainted ",
	"Starved "
};

#endif /* OVLB */
#ifdef OVL1

/* also used to see if you're allowed to eat cats and dogs */

STATIC_OVL boolean
cannibal_allowed()
{
	if (RngeCannibalism) return TRUE;

	switch (Role_switch) {
		case PM_CAVEMAN:
		case PM_LUNATIC:
			return TRUE;
	}

	switch (Race_switch) {
		case PM_ORC:
		case PM_YEEK:
		case PM_CURSER:
		case PM_ALIEN:
		case PM_TROLLOR:
		case PM_SHOE:
		case PM_PLAYER_SALAMANDER:
		case PM_VORTEX:
		case PM_GAVIL:
		case PM_CORTEX:
		case PM_GOAULD:
		case PM_KLACKON:
		case PM_KUTAR:
		case PM_TONBERRY:
		case PM_PLAYER_GLORKUM:
		case PM_RACE_THAT_DOESN_T_EXIST:
		case PM_HYBRIDRAGON:
		case PM_PLAYER_ANDROID:
		case PM_RETICULAN:
		case PM_OUTSIDER:
		case PM_HUMANOID_DEVIL:
		case PM_MUMMY:
		case PM_LICH_WARRIOR:
		case PM_KOBOLT:
		case PM_PHANTOM_GHOST:
		case PM_GIGANT:
		case PM_RODNEYAN:
		case PM_OGRO:
		case PM_WEAPON_TRAPPER:
		case PM_INSECTOID:
		case PM_MOULD:
		case PM_MISSINGNO:
		case PM_HUMANLIKE_DRAGON:
		case PM_HUMANLIKE_NAGA:
		case PM_DEATHMOLD:
		case PM_PLAYER_JABBERWOCK:
		case PM_AQUATIC_MONSTER:
		case PM_WORM_THAT_WALKS:
		case PM_UNGENOMOLD:
		case PM_UNALIGNMENT_THING:
		case PM_HUMAN_WEREWOLF:
		case PM_AK_THIEF_IS_DEAD_:
		case PM_SNAKEMAN:
		case PM_SPIDERMAN:
		case PM_PLAYER_ZRUTY:
		case PM_PLAYER_MUSHROOM:
		case PM_PLAYER_ASURA:
		case PM_METAL:
		case PM_SHELL:
		case PM_PLAYER_GOLEM:
		case PM_PIERCER:
		case PM_PLAYER_HULK:
		case PM_RACE_X:
		case PM_VAMPIRE:
		case PM_VAMGOYLE:
		case PM_SUCKING_FIEND:
		case PM_LEVITATOR:
		case PM_CLOCKWORK_AUTOMATON:
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
		case PM_AUREAL:
		case PM_MAZKE:
		case PM_PLAYER_GREMLIN:
		case PM_BORG:
		case PM_ELONA_SNAIL:
		case PM_PLAYER_UNICORN:
		case PM_WEAPONIZED_DINOSAUR:
		case PM_ANCIPITAL:
		case PM_FAWN:
		case PM_CHIROPTERAN:
		case PM_YUKI_PLAYA:
		case PM_OCTOPODE:
		case PM_INKA:
		case PM_SATRE:
		case PM_WISP:
		case PM_PLAYER_SKELETON:
		case PM_WEAPON_XORN:
		case PM_PLAYER_DOLGSMAN:
			return TRUE;
	}

	return FALSE;

}

/*
 * Decide whether a particular object can be eaten by the possibly
 * polymorphed character.  Not used for monster checks.
 */
boolean
is_edible(obj)
register struct obj *obj;
{

	/* protect invocation tools but not Rider corpses (handled elsewhere)*/
     /* if (obj->oclass != FOOD_CLASS && obj_resists(obj, 0, 0)) */
	if (evades_destruction(obj))
		return FALSE;
	if (objects[obj->otyp].oc_unique)
		return FALSE;
#ifdef MAIL
	if (obj->otyp == SCR_MAIL) return FALSE; /* cheator! :-P */
#endif
	/* above also prevents the Amulet from being eaten, so we must never
	   allow fake amulets to be eaten either [which is already the case] */

	/* Clockwork automatons can't eat anything at all, they need to use booze or oil --Amy */
	if (Race_if(PM_CLOCKWORK_AUTOMATON) && !Upolyd) return 0;

	if (uamul && uamul->oartifact == ART_AMULET_OF_SPLENDOR) return 0;

	/* same for golems, except they also don't get hungry over time */
	if (Race_if(PM_PLAYER_GOLEM) && !Upolyd) return 0;

	/* and also Demo's etherealoid */
	if (Race_if(PM_ETHEREALOID) && !Upolyd) return 0;
	if (Race_if(PM_INCORPOREALOID) && !Upolyd) return 0;

	/* Spirits can't eat corpses --Amy */
	if (Race_if(PM_SPIRIT) && obj->otyp == CORPSE && !Upolyd) return 0;

	if (Role_if(PM_ANACHRONIST) && (uarmh && itemhasappearance(uarmh, APP_ANACHRO_HELMET))) return 0;

	/* android can't either (anachrononononononist from dnethack who can only eat processed food) */
	if (Race_if(PM_PLAYER_ANDROID) && obj->otyp == CORPSE && !Upolyd) return 0;

	/* chewable potion randomized appearance is edible --Amy */
	if (itemhasappearance(obj, APP_POTION_CHEWABLE)) return 1;

	if (Race_if(PM_OCTOPODE) && obj->oclass == RING_CLASS) return 1;
	if (uarmh && uarmh->oartifact == ART_VORE_OF_THE_RINGS && obj->oclass == RING_CLASS) return 1;

	/* Incantifier only eats stone and metal --Amy */
	if (Race_if(PM_INCANTIFIER) && !Upolyd) return (boolean) (is_metallic(obj) || is_lithic(obj));

	/* Xorn only eats metal --Amy */
	if (Race_if(PM_WEAPON_XORN) && !Upolyd) return (boolean) (is_metallic(obj));

	/* Unicorn only eats stone --Amy */
	if (Race_if(PM_PLAYER_UNICORN) && !Upolyd) return (boolean) (is_lithic(obj));

	/* metallivores can eat metal; rustproofing makes it impossible now --Amy */
	if (metallivorous(youmonst.data) && is_metallic(obj) && !obj->oerodeproof)
		return TRUE;
	if (uwep && uwep->oartifact == ART_KRONSCHER_BAR && is_metallic(obj) && !obj->oerodeproof)
		return TRUE;

	/* lithivores can eat stone; erosionproofing makes it impossible now --Amy */
	if (lithivorous(youmonst.data) && is_lithic(obj) && !obj->oerodeproof )
		return TRUE;

	/* KMH -- Taz likes organics, too! */
	if (( (!Upolyd && Race_if(PM_JELLY) ) || (!Upolyd && Race_if(PM_WEAPON_CUBE) ) || organivorous(youmonst.data)) && is_organic(obj) &&
		/* [g.cubes can eat containers and retain all contents
		    as engulfed items, but poly'd player can't do that] */
	    !Has_contents(obj) && !obj->oerodeproof )
		return TRUE;

	/* Koalas only eat Eucalyptus leaves */
	if (u.umonnum == PM_KOALA)
		return (boolean)(obj->otyp == EUCALYPTUS_LEAF);
	if (u.umonnum == PM_GIANT_KOALA)
		return (boolean)(obj->otyp == EUCALYPTUS_LEAF);

	/* Ghouls, ghasts only eat corpses */
	if (saprovorousnumber(u.umonnum) || (Race_if(PM_GASTLY) && !Upolyd) || (Race_if(PM_PLAYER_SKELETON) && !Upolyd) || (Race_if(PM_PHANTOM_GHOST) && !Upolyd) )
	   	return (boolean)(obj->otyp == CORPSE);
	/* Vampires drink the blood of meaty corpses */
	/* [ALI] (fully) drained food is not presented as an option,
	 * but partly eaten food is (even though you can't drain it).
	 */

	if ((is_vampire(youmonst.data) || (Role_if(PM_GOFF) && !Upolyd) ) && mons[obj->corpsenm].mlet == S_TROVE && (!Race_if(PM_VAMGOYLE) || Upolyd) )
		return (boolean)(obj->otyp == CORPSE && (!obj->odrained || obj->oeaten > drainlevel(obj)));
	if ((is_vampire(youmonst.data) || (Role_if(PM_GOFF) && !Upolyd) ) && mons[obj->corpsenm].mlet == S_EEL && (!Race_if(PM_VAMGOYLE) || Upolyd) )
		return (boolean)(obj->otyp == CORPSE && (!obj->odrained || obj->oeaten > drainlevel(obj)));
	if ((is_vampire(youmonst.data) || (Role_if(PM_GOFF) && !Upolyd) ) && mons[obj->corpsenm].mlet == S_FLYFISH && (!Race_if(PM_VAMGOYLE) || Upolyd) )
		return (boolean)(obj->otyp == CORPSE && (!obj->odrained || obj->oeaten > drainlevel(obj)));

	if ((is_vampire(youmonst.data) || (Role_if(PM_GOFF) && !Upolyd) ) && (!Race_if(PM_VAMGOYLE) || Upolyd) )
		return (boolean)(obj->otyp == CORPSE &&
		  has_blood(&mons[obj->corpsenm]) && (!obj->odrained ||
		  obj->oeaten > drainlevel(obj)));

	if (Race_if(PM_VAMGOYLE) && !Upolyd)
		return (boolean)(obj->oclass == FOOD_CLASS && (!obj->odrained || obj->oeaten > drainlevel(obj)));

     /* return((boolean)(!!index(comestibles, obj->oclass))); */
	return (boolean)((obj->oclass == FOOD_CLASS) || (objects[(obj)->otyp].oc_material == MT_VEGGY) || (objects[(obj)->otyp].oc_material == MT_SECREE) || (objects[(obj)->otyp].oc_material == MT_FLESH) ) ;
}

#endif /* OVL1 */
#ifdef OVLB

void
init_uhunger()
{
	u.uhunger = 1500;
	u.uhs = NOT_HUNGRY;
}

static const struct { const char *txt; int nut; } tintxts[] = {
	{"deep fried",	 60},
	{"pickled",	 40},
	{"soup made from", 20},
	{"pureed",	500},
#define ROTTEN_TIN 4
	{"rotten",	-50},
#define HOMEMADE_TIN 5
	{"homemade",	 50},
	{"stir fried",   80},
	{"candied",      100},
	{"boiled",       50},
	{"dried",        55},
	{"szechuan",     70},
#define FRENCH_FRIED_TIN 11
	{"french fried", 40},
	{"sauteed",      95},
	{"broiled",      80},
	{"smoked",       50},
	/* [Tom] added a few new styles */        
	{"stir fried",   80},
	{"candied",      100},
	{"boiled",       50},
	{"dried",        55},
	{"szechuan",     70},
#define FRENCH_FRIED_TINX 20
	{"french fried", 40},
	{"sauteed",      95},
	{"mashed",      90},
	{"salted",      120},
	{"broiled",      80},
	{"smoked",       50},
	{"roasted",      120},
	{"cooked",      110},
	{"beefed",      150},
	{"tainted",      -500},
	{"moldy",      0},
	{"terrible",      5},
	/* added even more types, including some that actually give good nutrition --Amy */
	{"peppered",      100},
	{"spicy",      105},
	{"cheesy", 20},
	{"gourmand grade",      200},
	{"army grade", 30},
	{"tasty", 300},
	{"toasted", 320},
	{"ice-cold", 350},
	{"icy", 340},
	{"delicious", 400},
	{"gourmet", 350},
	{"stale", 100},
	{"dungeonmade", 60},
	{"lairmade", 80},
	{"planemade", 200},
	{"maggot-infested", -200},
	{"", 0}
};
#define TTSZ	SIZE(tintxts)

static NEARDATA struct {
	struct	obj *tin;
	int	usedtime, reqtime;
} tin;

static NEARDATA struct {
	struct	obj *piece;	/* the thing being eaten, or last thing that
				 * was partially eaten, unless that thing was
				 * a tin, which uses the tin structure above,
				 * in which case this should be 0 */
	/* doeat() initializes these when piece is valid */
	int	usedtime,	/* turns spent eating */
		reqtime;	/* turns required to eat */
	int	nmod;		/* coded nutrition per turn */
	Bitfield(canchoke,1);	/* was satiated at beginning */

	/* start_eating() initializes these */
	Bitfield(fullwarn,1);	/* have warned about being full */
	Bitfield(eating,1);	/* victual currently being eaten */
	Bitfield(doreset,1);	/* stop eating at end of turn */
} victual;

static char *eatmbuf = 0;	/* set by cpostfx() */

STATIC_PTR
int
eatmdone()		/* called after mimicing is over */
{
	/* release `eatmbuf' */
	if (eatmbuf) {
	    if (nomovemsg == eatmbuf) nomovemsg = 0;
	    free((void *)eatmbuf),  eatmbuf = 0;
	}
	/* update display */
	if (youmonst.m_ap_type) {
	    youmonst.m_ap_type = M_AP_NOTHING;
	    newsym(u.ux,u.uy);
	}
	return 0;
}

/* ``[the(] singular(food, xname) [)]'' with awareness of unique monsters */
STATIC_OVL const char *
food_xname(food, the_pfx)
struct obj *food;
boolean the_pfx;
{
	const char *result;
	int mnum = food->corpsenm;

	if (food->otyp == CORPSE && (mons[mnum].geno & G_UNIQ) && !Hallucination) {
	    /* grab xname()'s modifiable return buffer for our own use */
	    char *bufp = xname(food);

	    sprintf(bufp, "%s%s corpse",
		    (the_pfx && !type_is_pname(&mons[mnum])) ? "the " : "",
		    s_suffix(mons[mnum].mname));
	    result = bufp;
	} else {
	    /* the ordinary case */
	    result = singular(food, xname);
	    if (the_pfx) result = the(result);
	}
	return result;
}

STATIC_OVL const char *
Food_xname(food, the_pfx)
struct obj *food;
boolean the_pfx;
{
	/* food_xname() uses a modifiable buffer, so we can use it too */ 
	char *buf = (char *)food_xname(food, the_pfx);

	*buf = highc(*buf);
	return buf;
}

/* Created by GAN 01/28/87
 * Amended by AKP 09/22/87: if not hard, don't choke, just vomit.
 * Amended by 3.  06/12/89: if not hard, sometimes choke anyway, to keep risk.
 *		  11/10/89: if hard, rarely vomit anyway, for slim chance.
 */
STATIC_OVL void
choke(food)	/* To a full belly all food is bad. (It.) */
register struct obj *food;
{
	/* only happens if you were satiated */
	if (u.uhs != SATIATED) {
		if (!food || food->otyp != AMULET_OF_STRANGULATION)
			return;
	} else if ((Role_if(PM_KNIGHT) && u.ualign.type == A_LAWFUL) || (uwep && uwep->otyp == HONOR_KATANA) || (u.twoweap && uswapwep && uswapwep->otyp == HONOR_KATANA) || Role_if(PM_CHEVALIER) || Race_if(PM_VEELA) || Role_if(PM_PALADIN) || have_anorexia() || Role_if(PM_FAILED_EXISTENCE)) {
			adjalign(-3);		/* gluttony is unchivalrous */
		You(FunnyHallu ? "feel that your belly's gonna burst!" : "feel like a glutton!");
	}

	if (Race_if(PM_VEELA) || Role_if(PM_FAILED_EXISTENCE) ) badeffect();

	if (have_anorexia()) { /* They aren't used to eat much. --Amy */

	if(!rn2(4)) {
		if (FunnyHallu) You_feel("rather trippy.");
		else You_feel("rather %s.", body_part(LIGHT_HEADED));
		make_confused(HConfusion + d(2,4),FALSE);
	} else if(!rn2(5)) {
		if (FunnyHallu) You_feel("uncontrollable."); /* this and some other effects added by Amy */
		else You_feel("stunned.");
		make_stunned(HStun + d(2,4),FALSE);
	} else if(!rn2(5)) {
		if (FunnyHallu) You_feel("even weirder!");
		else You_feel("weirded out!");
		make_hallucinated(HHallucination + d(2,10),FALSE,0L);
	} else if(!rn2(4)) {
		if (!Blind) pline("Everything suddenly goes dark.");
		make_blinded(Blinded+d(2,10),FALSE);
		if (!Blind) Your("%s", vision_clears);
	} else if(!rn2(8)) {
		if (FunnyHallu) You_feel("like your body is trying to fall asleep!");
		else You_feel("numb.");
		make_numbed(HNumbed + d(2,15),FALSE);
	} else if(!rn2(10)) {
		if (FunnyHallu) pline("Crap! A %s suddenly appeared! RUN!",rndmonnam() );
		else You_feel("fear spreading through your body.");
		make_feared(HFeared + d(2,15),FALSE);
	} else if(!rn2(10)) {
		if (FunnyHallu) You_feel("like your on fire despite there being no heat!");
		else You_feel("like you're on fire!");
		make_burned(HBurned + d(2,15),FALSE);
	} else if(!rn2(20)) {
		if (FunnyHallu) You_feel("a giant ice cream cone enclosing you!");
		else pline("You're getting the chills.");
		make_frozen(HFrozen + d(2,15),FALSE);
	} else if(!rn2(20)) {
		if (FunnyHallu) You_feel("that your wife is gonna break up with you!");
		else pline("You're dimmed.");
		make_dimmed(HDimmed + d(2,15),FALSE);
	} else if(!rn2(10)) {
		if (FunnyHallu) You_feel("like you have dementia tremor!"); /* not a real name --Amy */
		else pline("Your hands start trembling violently!");
		incr_itimeout(&Glib, d(2,4) );
	} else if(!rn2(10)) {
		if (FunnyHallu) You_feel("totally down! Seems you tried some illegal shit!");
		else You_feel("like you're going to throw up.");
	      make_vomiting(Vomiting+20, TRUE);
		if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */
	} else if(!rn2(3)) {
		const char *what, *where;
		if (!Blind)
		    what = "goes",  where = "dark";
		else if (Levitation || Is_airlevel(&u.uz) ||
			 Is_waterlevel(&u.uz))
		    what = "you lose control of",  where = "yourself";
		else
		    what = "you slap against the", where = (u.usteed) ? "saddle" : surface(u.ux,u.uy);
		pline_The("world spins and %s %s.", what, where);
		flags.soundok = 0;
		nomul(-rnd(10), "unconscious from breaking anorexia conduct", TRUE);
		nomovemsg = "You are conscious again.";
		afternmv = Hear_again;
		return;
	} else if(!rn2(50) && !Sick) { /* The chance of this outcome !MUST! be low. Everything else would be unfair. --Amy */
	    make_sick(rn1(25,25), "rotten food", TRUE, SICK_VOMITABLE);

	/* "No longer possible to get slimed from eating food. Wayyyyyy too evil" In Soviet Russia, people don't know that scrolls of fire, wands of fireball, fire traps and prayer exist! Out of general paranoia, they have to remove the incredibly low chance of getting slimed, even though it basically never happened anyway. Chances are they just looked at the code and decided that it must be evil because it's in there. Come on! It has to pass the !rn2(200) chance AND all the "else if"s above, so it's bloody unlikely for it to happen, and even on the off chance it does, that's why you have scrolls of fire/cure! --Amy */

	} else if(!rn2(200) && !Slimed && !issoviet && !flaming(youmonst.data) && !Unchanging && !slime_on_touch(youmonst.data) ) { /* This chance should be even lower. --Amy */
			You("don't feel very well.");
			make_slimed(100);
		    killer_format = KILLED_BY_AN;
		    delayed_killer = "slimed by a slimy meal";
	}

	}

	exercise(A_CON, FALSE);

	if (Breathless || (!Strangled && !rn2(isfriday ? 4 : 2))) { /* much higher chance to survive --Amy */
		/* choking by eating AoS doesn't involve stuffing yourself */
		/* ALI - nor does other non-food nutrition (eg., life-blood) */
		if (!food || food->otyp == AMULET_OF_STRANGULATION) {
			nomovemsg = "You recover your composure.";
			You(FunnyHallu ? "spew bits of puke everywhere." : "choke over it.");
			morehungry(100);	/* remove a bit of nutrition so you don't choke again instantly --Amy */
			nomul(-2, "vomiting", TRUE);
			u_wipe_engr(100);

			u.cnd_vomitingcount++;

			if (isevilvariant) {
				register struct monst *shkp = shop_keeper(*u.ushops);
				if (shkp) {
					verbalize("You dare vomiting all over my shop, motherfucker???");
					make_angry_shk(shkp, 0, 0);
				}
			}

			if (u.inasuppression) {

				FemaleTrapIna += u.inasuppression;
				FemaleTrapIna += rnz(20 * (monster_difficulty() + 1));
				if (rn2(3)) FemaleTrapIna += 100;
				if (!rn2(3)) FemaleTrapIna += rnz(500);
				u.inasuppression = 0;
				pline("Oh my god, you are now afflicted with the life-threatening disease known as anorexia!");
			}

			return;
		}
		You(FunnyHallu ? "vomit all over the place. Shit, now your clothes are a huge mess!" : "stuff yourself and then vomit voluminously.");
		if (have_anorexia() || Role_if(PM_FAILED_EXISTENCE)) {
			adjalign(-20); /* overeating doesn't befit a topmodel */
			pline(FunnyHallu ? "Uaargh - maybe you should order some smaller meals next time?" : "Bleeargh! You feel very bad for trying to overeat.");
		}
		morehungry(2000);	/* you just got *very* sick! */
		u_wipe_engr(100);
		nomovemsg = 0;
		vomit();
	} else {
		u.youaredead = 1;
		killer_format = KILLED_BY_AN;
		/*
		 * Note all "killer"s below read "Choked on %s" on the
		 * high score list & tombstone.  So plan accordingly.
		 */
		if(food) {
			You("choke over your %s.", foodword(food));
			if (food->oclass == COIN_CLASS) {
				killer = "a very rich meal";
			} else {
				killer = food_xname(food, FALSE);
				if (food->otyp == CORPSE &&
				    (mons[food->corpsenm].geno & G_UNIQ)) {
				    if (!type_is_pname(&mons[food->corpsenm]))
					killer = the(killer);
				    killer_format = KILLED_BY;
				}
			}
		} else {
			You("choke over it.");
			killer = "quick snack";
		}
		You(isangbander ? "have died." : "die...");
		done(CHOKING);
		u.youaredead = 0;
	}
}

/* modify object wt. depending on time spent consuming it */
STATIC_OVL void
recalc_wt()
{
	struct obj *piece = victual.piece;

#ifdef DEBUG
	debugpline("Old weight = %d", piece->owt);
	debugpline("Used time = %d, Req'd time = %d",
		victual.usedtime, victual.reqtime);
#endif
	piece->owt = weight(piece);
#ifdef DEBUG
	debugpline("New weight = %d", piece->owt);
#endif
}

void
reset_eat()		/* called when eating interrupted by an event */
{
    /* we only set a flag here - the actual reset process is done after
     * the round is spent eating.
     */
	if(victual.eating && !victual.doreset) {
#ifdef DEBUG
	    debugpline("reset_eat...");
#endif
	    victual.doreset = TRUE;
	}
	return;
}

STATIC_OVL struct obj *
touchfood(otmp)
register struct obj *otmp;
{
	if (otmp->quan > 1L) {
	    if(!carried(otmp))
		(void) splitobj(otmp, otmp->quan - 1L);
	    else
		otmp = splitobj(otmp, 1L);

#ifdef DEBUG
	    debugpline("split object,");
#endif
	}

	if (!otmp->oeaten) {
	    if(((!carried(otmp) && costly_spot(otmp->ox, otmp->oy) &&
		 !otmp->no_charge)
		 || otmp->unpaid)) {
		/* create a dummy duplicate to put on bill */
		verbalize("You bit it, you bought it!");
		bill_dummy_object(otmp);
	    }
	    otmp->oeaten = (otmp->otyp == CORPSE ?
				mons[otmp->corpsenm].cnutrit :
				objects[otmp->otyp].oc_nutrition);
	}

	if (carried(otmp)) {
	    freeinv(otmp);
	    if (inv_cnt() >= 52) {
		sellobj_state(SELL_DONTSELL);
		dropy(otmp);
		sellobj_state(SELL_NORMAL);
	    } else {
		otmp->oxlth++;		/* hack to prevent merge */
		otmp = addinv(otmp);
		otmp->oxlth--;
	    }
	}
	return(otmp);
}

/* When food decays, in the middle of your meal, we don't want to dereference
 * any dangling pointers, so set it to null (which should still trigger
 * do_reset_eat() at the beginning of eatfood()) and check for null pointers
 * in do_reset_eat().
 */
void
food_disappears(obj)
register struct obj *obj;
{
	if (obj == victual.piece) victual.piece = (struct obj *)0;
	if (obj->timed) obj_stop_timers(obj);
}

/* renaming an object usually results in it having a different address;
   so the sequence start eating/opening, get interrupted, name the food,
   resume eating/opening would restart from scratch */
void
food_substitution(old_obj, new_obj)
struct obj *old_obj, *new_obj;
{
	if (old_obj == victual.piece) victual.piece = new_obj;
	if (old_obj == tin.tin) tin.tin = new_obj;
}

STATIC_OVL void
do_reset_eat()
{
#ifdef DEBUG
	debugpline("do_reset_eat...");
#endif
	if (victual.piece) {
		victual.piece = touchfood(victual.piece);
		recalc_wt();
	}
	victual.fullwarn = victual.eating = victual.doreset = FALSE;
	/* Do not set canchoke to FALSE; if we continue eating the same object
	 * we need to know if canchoke was set when they started eating it the
	 * previous time.  And if we don't continue eating the same object
	 * canchoke always gets recalculated anyway.
	 */
	stop_occupation();
	newuhs(FALSE);
}

STATIC_PTR
int
eatfood()		/* called each move during eating process */
{
	if(!victual.piece ||
	 (!carried(victual.piece) && !obj_here(victual.piece, u.ux, u.uy))) {
		/* maybe it was stolen? */
		do_reset_eat();
		return(0);
	}
	if ( (is_vampire(youmonst.data) || Role_if(PM_GOFF) ) != victual.piece->odrained) {
	    /* Polymorphed while eating/draining */
	    do_reset_eat();
	    return(0);
	}
	if(!victual.eating) return(0);

	if(++victual.usedtime <= victual.reqtime) {
	    if(bite()) return(0);
	    return(1);	/* still busy */
	} else {	/* done */
	    int crumbs = victual.piece->oeaten;		/* The last crumbs */
	    if (victual.piece->odrained) crumbs -= drainlevel(victual.piece);
	    if (crumbs > 0) {
		lesshungry(crumbs);
		victual.piece->oeaten -= crumbs;
	    }
	    done_eating(TRUE);
	    return(0);
	}
}

STATIC_OVL void
done_eating(message)
boolean message;
{
	victual.piece->in_use = TRUE;
	occupation = 0; /* do this early, so newuhs() knows we're done */
	newuhs(FALSE);
	if (nomovemsg) {
		if (message) pline("%s", nomovemsg);
		nomovemsg = 0;
	} else if (message)
		You("finish %s %s.", victual.piece->odrained ? "draining" :
		  "eating", food_xname(victual.piece, TRUE));

	/* Bleeders who can drain corpses will benefit from doing so */
	if(victual.piece->otyp == CORPSE && victual.piece->odrained && Role_if(PM_BLEEDER) ) {

		pline(FunnyHallu ? "It tastes like sweet, delicious nectar!" : "That blood really hit the spot!");
		healup(d(8,8) + rnz(u.ulevel), 0, TRUE, TRUE);
		exercise(A_CON, TRUE);

	}

	if(victual.piece->otyp == CORPSE && victual.piece->odrained && Race_if(PM_HEMOPHAGE) ) {

		pline(FunnyHallu ? "It tastes like sweet, delicious nectar!" : "That blood really hit the spot!");
		healup(d(8,8) + rnz(u.ulevel), 0, TRUE, TRUE);
		exercise(A_CON, TRUE);

	}

	if(Race_if(PM_WORM_THAT_WALKS)) { /* code by Amy */

		/* If eating a corpse, or other item that has "corpsenm" data:
		 *	- 80% chance to polymorph into the eaten monster
		 *	- 20% chance to polymorph into something random
		 * If eating something other than a corpse:
		 *	- 80% chance that nothing happens
		 *	- 20% chance to polymorph into something random */

		if(victual.piece->otyp == CORPSE && polyok(&mons[victual.piece->corpsenm]) ) {

			if (rn2(5)) {

				u.wormpolymorph = victual.piece->corpsenm;
				polyself(FALSE);

			} else polyself(FALSE);

		} else if(victual.piece->otyp == EGG && victual.piece->corpsenm != NON_PM && polyok(&mons[victual.piece->corpsenm]) ) {

			if (rn2(5)) {

				u.wormpolymorph = victual.piece->corpsenm;
				polyself(FALSE);

			} else polyself(FALSE);

		} else if (!rn2(5)) polyself(FALSE);

	}

	if(Race_if(PM_WARPER)) { /* 20% chance to polymorph into the depicted monster --Amy */

		if(victual.piece->otyp == CORPSE && !rn2(5) ) {

			u.wormpolymorph = victual.piece->corpsenm;
			polyself(FALSE);

		} else if(victual.piece->otyp == EGG && victual.piece->corpsenm != NON_PM && !rn2(5) ) {

			u.wormpolymorph = victual.piece->corpsenm;
			polyself(FALSE);

		}

	}

	if(Race_if(PM_MISSINGNO) && !rn2(5)) polyself(FALSE);

	if(Race_if(PM_DEATHMOLD) && !rn2(5)) polyself(FALSE);

	if(victual.piece->otyp == CORPSE) {
		if (!victual.piece->odrained || /*Race_if(PM_VAMPIRE) &&*/ !rn2(5))
		cpostfx(victual.piece->corpsenm);
	} else
		fpostfx(victual.piece);

	if (victual.piece->odrained)
		victual.piece->in_use = FALSE;
	else
	if (carried(victual.piece)) useup(victual.piece);
	else useupf(victual.piece, 1L);
	victual.piece = (struct obj *) 0;
	victual.fullwarn = victual.eating = victual.doreset = FALSE;
}

STATIC_OVL boolean
maybe_cannibal(pm, allowmsg)
int pm;
boolean allowmsg;
{
	if (your_race(&mons[pm])) {
	    if (!cannibal_allowed()) {
		if (allowmsg) {
		    if (Upolyd)
			You("have a bad feeling deep inside.");
		    You("cannibal!  You will regret this!");
		}
		u.cnd_cannibalcount++;

		/* Amy edit: it's teh uber-sux if one mishap gives you aggravate monster permanently.
		 * So we're just making it last longer the more often you cannibalize */
		if (u.cnd_cannibalcount < 1) u.cnd_cannibalcount = 1; /* should never happen */
		if (u.cnd_cannibalcount < 5) {
			HAggravate_monster += (1000 * u.cnd_cannibalcount);
		} else {
			HAggravate_monster += ((1000 + rnd(9000)) * u.cnd_cannibalcount);

		}
		increasesanity(rnd((level_difficulty() * 2) + 5));

		/* "Cannibalism angers your god instead of removing telepathy." In the Evil Variant, new bad effects are added,
		 * they don't replace old bad effects but happen in addition to them. --Amy */
		if (evilfriday) {
			u.ugangr++;
		      You("get the feeling that %s is angry...", u_gname());
		}

		if (u.ualign.type == A_LAWFUL && !rn2(3) ) change_luck(-rnd(5));		/* -5..-1 */
		else if (u.ualign.type == A_LAWFUL) change_luck(-rnd(4));		/* -4..-1 */
		else if (u.ualign.type == A_NEUTRAL && !rn2(3) ) change_luck(-rnd(4));		/* -4..-1 */
		else if (u.ualign.type == A_NEUTRAL) change_luck(-rnd(3));		/* -4..-1 */
		else if (u.ualign.type == A_CHAOTIC && !rn2(3) ) change_luck(-rnd(3));		/* -3..-1 */
		else if (u.ualign.type == A_CHAOTIC && rn2(2) ) change_luck(-rnd(2));		/* -2..-1 */
	    } else if (Role_if(PM_CAVEMAN)) {
		adjalign(sgn(u.ualign.type));
		You("honour the dead.");
	    } else {
		adjalign(-sgn(u.ualign.type));
		You_feel("evil and fiendish!");
	    }
	    return TRUE;
	}
	return FALSE;
}

STATIC_OVL void
cprefx(pm)
register int pm;
{
	(void) maybe_cannibal(pm,TRUE);
	if ((touch_petrifies(&mons[pm]) || pm == PM_MEDUSA) && pm != PM_PLAYERMON) {
	    if ((!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) &&
		!(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) {
		/* sprintf(killer_buf, "tasting petrifying meat (%s)", mons[pm].mname);
		killer_format = KILLED_BY;
		killer = killer_buf;
		You("turn to stone.");
		done(STONING);
		if (victual.piece)
		    victual.eating = FALSE;
		return; */
		if (!Stoned) {
			if (Hallucination && rn2(10)) pline("Good thing you are already stoned.");
			else {
				You("start turning to stone.");
				Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
				u.cnd_stoningcount++;
				sprintf(killer_buf, "tasting petrifying meat (%s)", mons[pm].mname);
				delayed_killer = killer_buf;
			}
		}
		
	    }
	}

	if (dmgtype(&mons[pm], AD_AGGR) ) {
	    You_feel("that %s the %s%s was a bad idea.",
	      victual.eating ? "eating" : "biting",
	      occupation == opentin ? "tinned " : "", mons[pm].mname);
	    HAggravate_monster += rnd(100);

	}

	if (HardcoreAlienMode && mons[pm].mlet == S_QUADRUPED) {
		u.ugangr++;
		pline("Eeeek - you ate forbidden meat! The gods will most probably not like that...");
	}

	if (dmgtype(&mons[pm], AD_RAGN) ) {
		ragnarok(FALSE);
		if (evilfriday) evilragnarok(FALSE,level_difficulty());
	}

	switch(pm) {
	    case PM_KAMIKAZE_COCKATRICE: /* will normally not leave a corpse anyway */
		ragnarok(FALSE);
		if (evilfriday) evilragnarok(FALSE,level_difficulty());
		break;
	    case PM_LITTLE_DOG:
	    case PM_DOG:
	    case PM_LARGE_DOG:
	    case PM_KITTEN:
	    case PM_HOUSECAT:
	    case PM_LARGE_CAT:
	    case PM_DOMESTIC_COCKATRICE:
	    case PM_CHAIN_SMOKER_ASSHOLE:
		if (!cannibal_allowed()) {
		    You_feel("that %s the %s%s was a bad idea.",
		      victual.eating ? "eating" : "biting",
		      occupation == opentin ? "tinned " : "", mons[pm].mname);

			/* make aggravation last only for a couple turns, but summon stuff (see cpostfx) --Amy */

		}
		break;
	    case PM_ROCK_HARD_COCKATRICE:
		if (u.ublessed < 20) {
			if (!(HProtection & INTRINSIC)) HProtection |= FROMOUTSIDE;
			u.ublessed++;
			find_ac();
			flags.botl = TRUE;
			pline("A holy aura of protection surrounds you!");
		}
		break;
	    case PM_LIZARD:
	    case PM_LIZZY:
	    case PM_LIZARD_PRINCE:
	    case PM_ROCK_LIZARD:
	    case PM_WILL_STONE_LIZARD:
	    case PM_WILL_RATCH_LIZARD:
	    case PM_LICHZARD:
	    case PM_SKELLIZARD:
	    case PM_BLACK_LIZARD:
	    case PM_BABY_CAVE_LIZARD:
	    case PM_NIGHT_LIZARD:
	    case PM_LIZARD_MAN:
	    case PM_LIZARD_KING:
	    case PM_LIZARD_OF_YENDOR:
	    case PM_CAVE_LIZARD:
	    case PM_PREHISTORIC_CAVE_LIZARD:
	    case PM_CHAOS_LIZARD:
	    case PM_CHAOTIC_LIZARD:
	    case PM_HUGE_LIZARD:
	    case PM_SAND_TIDE:
	    case PM_FBI_AGENT:
	    case PM_OWN_SMOKE:
	    case PM_GRANDPA:
	    case PM_PRESSLIZARD:
	    case PM_STATIOLIZARD:
	    case PM_GIBLIZARD:
	    case PM_CASINO_LIZARD:
	    case PM_TOUCAN:
	    case PM_GIANT_TOUCAN:
	    case PM_LIZARD_MAGE:
	    case PM_BLACK_LIZARDMAN:
	    case PM_ASSASSIN_LIZARD:
	    case PM_BLIZZARD_LIZARD:
	    case PM_HELTH_LIZARD:
	    case PM_NORMAL_LIZARD:
	    case PM_CLOCKBACK_LIZARD:
	    case PM_ADULT_LIZARD:
	    case PM_KARMIC_LIZARD:
	    case PM_GREEN_LIZARD:
	    case PM_SCORZARD:
	    case PM_MONSTER_LIZARD:
	    case PM_FIRE_LIZARD:
	    case PM_LIGHTNING_LIZARD:
	    case PM_ICE_LIZARD:
	    case PM_GIANT_LIZARD:
	    case PM_LIZARD_EEL:
	    case PM_INNOCLIZARD:
	    case PM_FAT_LIZARD:
	    case PM_EEL_LIZARD:
	    case PM_GRASS_LIZARD:
	    case PM_RUNE_LIZARD:
	    case PM_SPECTRAL_LIZARD:
	    case PM_BLUE_LIZARD:
	    case PM_SWAMP_LIZARD:
	    case PM_SPITTING_LIZARD:
	    case PM_ANTI_STONE_LIZARD:
	    case PM_MIMIC_LIZARD:
	    case PM_HIDDEN_LIZARD:
	    case PM_CLINGING_LIZARD:
	    case PM_DEFORMED_LIZARD:
	    case PM_MEDUSA:
	    case PM_HENATRICE:
	    case PM_HANH_S_BLOCK_HEELED_SANDAL:
		if (Stoned) fix_petrification();
		break;
	    case PM_KATNISS: /* bad idea --Amy */
		pline("A voice booms out:");
		verbalize("How darest thou eat Cato's girlfriend!");
		change_luck(-5);
		u.ualign.sins += 10; 
		u.alignlim -= 10;
		adjalign(-50);
		u.ugangr++; u.ugangr++; u.ugangr++;
		prayer_done();

	    break;

	    case PM_SQUIRREL:
	    case PM_KWIRREL:
	    case PM_IGUANA:
	    case PM_LEGWA:
	    case PM_BIG_IGUANA:
	    case PM_HELPFUL_SQUIRREL:
	    case PM_STAR_SQUIRREL:
		make_hallucinated(0L,TRUE,0L);
		make_confused(0L,TRUE);
		make_stunned(0L,TRUE);
		make_blinded(0L,TRUE);
		break;
	    case PM_RHAUMBUSUN:
	    case PM_FEMBUSUN:
	    case PM_BLOODBUSUN:
	    case PM_BIG_RHAUMBUSUN:
		if (Slimed) {pline("The slime disappears.");
		    Slimed = 0;
		    flags.botl = 1;
		    delayed_killer = 0;
			}
		break;
	    case PM_SALAMANDER:
	    case PM_SALAMANDER_PRISONER:
	    case PM_SALAMANDER_SLAVE:
	    case PM_SALAMANDER_MAGE:
	    case PM_SALAMANDER_SHAMAN:
	    case PM_POISON_WHIP_SALAMANDER:
	    case PM_PARALYSIS_WHIP_SALAMANDER:
	    case PM_FROST_SALAMANDER:
	    case PM_KOMODO_DRAGON:
	    case PM_KOMODO_BEAST:
	    case PM_PETTY_KOMODO_DRAGON:
		make_numbed(0L,TRUE);
		make_frozen(0L,TRUE);
		make_burned(0L,TRUE);
		make_feared(0L,TRUE);
		break;
	    case PM_GECKO:
	    case PM_GEGGO:
	    case PM_FLYING_GECKO:
	    case PM_GIANT_GECKO:
		if (Sick)
		    make_sick(0L, (char *)0, TRUE, SICK_ALL);
		if (Vomiting && !(uarmf && uarmf->oartifact == ART_CRUEL_GODDESS_ANA))
		    make_vomiting(0L, TRUE);
		break;
	    case PM_DEATH:
	    case PM_PESTILENCE:
	    case PM_WAR:
	    case PM_FRUSTRATION:
	    case PM_BLACK_HOLE_BLOB:
	    case PM_FAMINE:
	    case PM_GLUTTONY:
	    case PM_LUST:
	    case PM_ENVY:
	    case PM_SLOTH:
	    case PM_PRIDE:
	    case PM_WRATH:
	    case PM_GREED:
	    case PM_DEPRESSION:
	    case PM_BASTARD_OPERATOR_FROM_HELL:
	    case PM_DNETHACK_ELDER_PRIEST_TM_:
		{ char buf[BUFSZ];
		    u.youaredead = 1;
		    pline("Eating that is instantly fatal.");
		    sprintf(buf, "unwisely ate the body of a monster (%s)",
			    mons[pm].mname);
		    killer = buf;
		    killer_format = NO_KILLER_PREFIX;
		    done(DIED);
		    u.youaredead = 0;
		    /* It so happens that since we know these monsters */
		    /* cannot appear in tins, victual.piece will always */
		    /* be what we want, which is not generally true. */
			/* Amy edit: no longer the case. Black hole blobs exist now. Segfaults ahoy. */

			if (victual.piece) {
			    if (revive_corpse(victual.piece, FALSE))
				victual.piece = (struct obj *)0;
			    return;
			}
		}
		/* Fall through */
	    default:

		if (!Slimed && !Unchanging && !flaming(youmonst.data) && slime_on_touch(&mons[pm]) &&
			!slime_on_touch(youmonst.data) ) {
		    You("don't feel very well.");
			make_slimed(100);
		    killer_format = KILLED_BY_AN;
		    delayed_killer = "slimed by doing something stupid";
		}
		if (acidic(&mons[pm]) && Stoned)
		    fix_petrification();
		break;
	}
}

/*
 * Called when a vampire bites a monster.
 * Returns TRUE if hero died and was lifesaved.
 */

boolean
bite_monster(mon)
struct monst *mon;
{
    switch(monsndx(mon->data)) {
	case PM_LIZARD:
	case PM_LIZZY:
	case PM_LIZARD_PRINCE:
	case PM_ROCK_LIZARD:
      case PM_WILL_STONE_LIZARD:
      case PM_WILL_RATCH_LIZARD:
      case PM_LICHZARD:
      case PM_SKELLIZARD:
      case PM_INNOCLIZARD:
      case PM_FAT_LIZARD:
	case PM_BABY_CAVE_LIZARD:
	case PM_NIGHT_LIZARD:
	case PM_LIZARD_MAN:
	case PM_LIZARD_KING:
	case PM_LIZARD_OF_YENDOR:
	case PM_CAVE_LIZARD:
	case PM_PREHISTORIC_CAVE_LIZARD:
	case PM_CHAOS_LIZARD:
	case PM_CHAOTIC_LIZARD:
	case PM_LIZARD_EEL:
	case PM_EEL_LIZARD:
	case PM_GRASS_LIZARD:
	case PM_RUNE_LIZARD:
	case PM_SPECTRAL_LIZARD:
      case PM_BLUE_LIZARD:
	case PM_SWAMP_LIZARD:
	case PM_SPITTING_LIZARD:
	case PM_ANTI_STONE_LIZARD:
    case PM_MIMIC_LIZARD:
    case PM_HIDDEN_LIZARD:
    case PM_CLINGING_LIZARD:
    case PM_DEFORMED_LIZARD:
	case PM_HUGE_LIZARD:
	case PM_SAND_TIDE:
	case PM_FBI_AGENT:
	    case PM_PRESSLIZARD:
	    case PM_STATIOLIZARD:
	    case PM_GIBLIZARD:
	    case PM_CASINO_LIZARD:
	    case PM_OWN_SMOKE:
	    case PM_ADULT_LIZARD:
	    case PM_GRANDPA:
	    case PM_TOUCAN:
	    case PM_GIANT_TOUCAN:
	    case PM_LIZARD_MAGE:
	    case PM_BLACK_LIZARDMAN:
	    case PM_ASSASSIN_LIZARD:
	    case PM_BLIZZARD_LIZARD:
	    case PM_HELTH_LIZARD:
	    case PM_NORMAL_LIZARD:
	    case PM_CLOCKBACK_LIZARD:
	case PM_KARMIC_LIZARD:
	case PM_GREEN_LIZARD:
	case PM_SCORZARD:
	case PM_BLACK_LIZARD:
	case PM_MONSTER_LIZARD:
	case PM_FIRE_LIZARD:
	case PM_LIGHTNING_LIZARD:
	case PM_ICE_LIZARD:
	case PM_GIANT_LIZARD:
	    if (Stoned) fix_petrification();
	    break;

	case PM_DEATH:
	case PM_PESTILENCE:
	case PM_WAR:
	case PM_FRUSTRATION:
	case PM_BLACK_HOLE_BLOB:
	case PM_FAMINE:
	case PM_GLUTTONY:
	case PM_LUST:
	case PM_ENVY:
	case PM_SLOTH:
	case PM_PRIDE:
	case PM_WRATH:
	case PM_GREED:
	case PM_DEPRESSION:
	case PM_BASTARD_OPERATOR_FROM_HELL:
	case PM_DNETHACK_ELDER_PRIEST_TM_:
	    pline("Unfortunately, eating any of it is fatal.");
	    done_in_by(mon);
	    return TRUE;		/* lifesaved */

	    /* Fall through */
	default:
	    if (!Unchanging && youmonst.data != &mons[PM_FIRE_VORTEX] &&
			    youmonst.data != &mons[PM_FIRE_ELEMENTAL] &&
			    !slime_on_touch(youmonst.data) && slime_on_touch(mon->data) ) {
		You("don't feel very well.");
		make_slimed(100);
		killer_format = KILLED_BY_AN;
		delayed_killer = "slimed by doing something stupid";
	    }
	    if (acidic(mon->data) && Stoned)
		fix_petrification();
	    break;
    }
    return FALSE;
}

void
fix_petrification()
{
	Stoned = 0;
	delayed_killer = 0;
	if (FunnyHallu)
	    pline("What a pity - you just ruined a future piece of %sart!",
		  ACURR(A_CHA) > 15 ? "fine " : "");
	else {
	    You_feel("limber!");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vy tol'ko otsrochila neizbezhnoye i v konechnom schete zakonchatsya pobivaniye kamnyami lecheniy." : "Schwueing!");
	}
}

/*
 * If you add an intrinsic that can be gotten by eating a monster, add it
 * to intrinsic_possible() and givit().  (It must already be in prop.h to
 * be an intrinsic property.)
 * It would be very easy to make the intrinsics not try to give you one
 * that you already had by checking to see if you have it in
 * intrinsic_possible() instead of givit().
 */

/* Acid and petrification resistance added by Amy. */

/* intrinsic_possible() returns TRUE if a monster can give an intrinsic. */
STATIC_OVL int
intrinsic_possible(type, ptr)
int type;
register struct permonst *ptr;
{

	if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) return(FALSE);

	/* if additional detrimental ones are added, make sure they don't become less likely on Friday the 13th --Amy */
	if (isfriday && type == TELEPORT_CONTROL && rn2(5)) return FALSE;
	if (isfriday && type != TELEPORT && type != TELEPORT_CONTROL && !rn2(3)) return FALSE;

	switch (type) {
	    case ACID_RES:
#ifdef DEBUG
		if (ptr->mconveys & MR_ACID) {
			debugpline("can get acid resistance");
			return(TRUE);
		} else  return(FALSE);
#else
		return(ptr->mconveys & MR_ACID);
#endif
	    case STONE_RES:
#ifdef DEBUG
		if (ptr->mconveys & MR_STONE) {
			debugpline("can get petrify resistance");
			return(TRUE);
		} else  return(FALSE);
#else
		return(ptr->mconveys & MR_STONE);
#endif
	    case FIRE_RES:
#ifdef DEBUG
		if (ptr->mconveys & MR_FIRE) {
			debugpline("can get fire resistance");
			return(TRUE);
		} else  return(FALSE);
#else
		return(ptr->mconveys & MR_FIRE);
#endif
	    case SLEEP_RES:
#ifdef DEBUG
		if (ptr->mconveys & MR_SLEEP) {
			debugpline("can get sleep resistance");
			return(TRUE);
		} else  return(FALSE);
#else
		return(ptr->mconveys & MR_SLEEP);
#endif
	    case COLD_RES:
#ifdef DEBUG
		if (ptr->mconveys & MR_COLD) {
			debugpline("can get cold resistance");
			return(TRUE);
		} else  return(FALSE);
#else
		return(ptr->mconveys & MR_COLD);
#endif
	    case DISINT_RES:
#ifdef DEBUG
		if (ptr->mconveys & MR_DISINT) {
			debugpline("can get disintegration resistance");
			return(TRUE);
		} else  return(FALSE);
#else
		return(ptr->mconveys & MR_DISINT);
#endif
	    case SHOCK_RES:	/* shock (electricity) resistance */
#ifdef DEBUG
		if (ptr->mconveys & MR_ELEC) {
			debugpline("can get shock resistance");
			return(TRUE);
		} else  return(FALSE);
#else
		return(ptr->mconveys & MR_ELEC);
#endif
	    case POISON_RES:
#ifdef DEBUG
		if (ptr->mconveys & MR_POISON) {
			debugpline("can get poison resistance");
			return(TRUE);
		} else  return(FALSE);
#else
		return(ptr->mconveys & MR_POISON);
#endif
	    case TELEPORT:
#ifdef DEBUG
		if (can_teleport(ptr)) {
			debugpline("can get teleport");
			return(TRUE);
		} else  return(FALSE);
#else
		return(can_teleport(ptr));
#endif
	    case TELEPORT_CONTROL:
#ifdef DEBUG
		if (control_teleport(ptr)) {
			debugpline("can get teleport control");
			return(TRUE);
		} else  return(FALSE);
#else
		return(control_teleport(ptr));
#endif
	    case TELEPAT:
#ifdef DEBUG
		if (telepathic(ptr)) {
			debugpline("can get telepathy");
			return(TRUE);
		} else  return(FALSE);
#else
		return(telepathic(ptr));
#endif

	    case HALLU_PARTY:
		return(dmgtype(ptr, AD_HALU) );
	    case DRUNKEN_BOXING:
		return(dmgtype(ptr, AD_CONF) );
	    case STUNNOPATHY:
		return(dmgtype(ptr, AD_STUN) );
	    case NUMBOPATHY:
		return(dmgtype(ptr, AD_NUMB) );
	    case DIMMOPATHY:
		return(dmgtype(ptr, AD_DIMN) );
	    case FREEZOPATHY:
		return(dmgtype(ptr, AD_FRZE) );
	    case STONED_CHILLER:
		return(dmgtype(ptr, AD_STON) );
	    case CORROSIVITY:
		return(dmgtype(ptr, AD_SLIM) );
	    case FEAR_FACTOR:
		return(dmgtype(ptr, AD_FEAR) );
	    case BURNOPATHY:
		return(dmgtype(ptr, AD_BURN) );
	    case SICKOPATHY:
		return(dmgtype(ptr, AD_DISE) );
	    case WONDERLEGS:
		return(dmgtype(ptr, AD_LEGS) );
	    case GLIB_COMBAT:
		return(dmgtype(ptr, AD_GLIB) );

	    default:
		return(FALSE);
	}
	/*NOTREACHED*/
}

/* givit() tries to give you an intrinsic based on the monster's level
 * and what type of intrinsic it is trying to give you.
 */
/* KMH, balance patch -- eliminated temporary intrinsics from
 * corpses, and restored probabilities to NetHack levels.
 *
 * There were several ways to deal with this issue:
 * 1.  Let corpses convey permanent intrisics (as implemented in
 *     vanilla NetHack).  This is the easiest method for players
 *     to understand and has the least player frustration.
 * 2.  Provide a temporary intrinsic if you don't already have it,
 *     a give the permanent intrinsic if you do have it (Slash's
 *     method).  This is probably the most realistic solution,
 *     but players were extremely annoyed by it.
 * 3.  Let certain intrinsics be conveyed one way and the rest
 *     conveyed the other.  However, there would certainly be
 *     arguments about which should be which, and it would
 *     certainly become yet another FAQ.
 * 4.  Increase the timeouts.  This is limited by the number of
 *     bits reserved for the timeout.
 * 5.  Convey a permanent intrinsic if you have _ever_ been
 *     given the temporary intrinsic.  This is a nice solution,
 *     but it would use another bit, and probably isn't worth
 *     the effort.
 * 6.  Give the player better notice when the timeout expires,
 *     and/or some method to check on intrinsics that is not as
 *     revealing as enlightenment.
 * 7.  Some combination of the above.
 *
 * In the end, I decided that the simplest solution would be the
 * best solution.
 */

/* Acid and petrification resistance are possible now. However, they're temporary instead of permanent. --Amy */

STATIC_OVL void
givit(type, ptr)
int type;
register struct permonst *ptr;
{
	register int chance;

#ifdef DEBUG
	debugpline("Attempting to give intrinsic %d", type);
#endif
	/* some intrinsics are easier to get than others */

	if (!issoviet) {
	switch (type) {
		/* case POISON_RES:
			if ((ptr == &mons[PM_KILLER_BEE] ||
					ptr == &mons[PM_SCORPION]) && !rn2(4))
				chance = 1;
			else
				chance = 15;
			break;
		case TELEPORT:
			chance = 10;
			break;
		case TELEPORT_CONTROL:
			chance = 12;
			break;
		case TELEPAT:
			chance = 1;
			break; */
		default:
			chance = (Race_if(PM_ILLITHID) ? 105 : Race_if(PM_BACTERIA) ? 12 : 35); /*much lower chance now --Amy */
			break;
	}
	} else {

	/* "Re-adjust probs for intrinsic gain from corpses." Because in Soviet Russia people want to be fully kitted out with all the possible intrinsics at dlvl10, otherwise the game would be too hard for them. They also disregard the fact that poison instakills are a lot less likely in Slash'EM Extended. --Amy */
	switch (type) {
		case POISON_RES:
			if ((ptr == &mons[PM_KILLER_BEE] ||
					ptr == &mons[PM_SCORPION]) && !rn2(4))
				chance = 1;
			else
				chance = 15;
			break;
		case TELEPORT:
			chance = 10;
			break;
		case TELEPORT_CONTROL:
			chance = 12;
			break;
		case TELEPAT:
			chance = 1;
			break;
		default:
			chance = (Race_if(PM_ILLITHID) ? 35 : Race_if(PM_BACTERIA) ? 12 : 15);
			break;
	}

	} /* issoviet check */

	/* dragonmaster is much better at gaining dragon powers --Amy */
	if (Role_if(PM_DRAGONMASTER) && ptr->mlet == S_DRAGON && chance > 1) {
		chance /= 3;
		if (chance < 1) chance = 1;
	}

	if ( (ptr->mlevel <= rn2(chance) ) || !rn2(4) )
		return;		/* failed die roll */

	switch (type) {
	    case ACID_RES:
#ifdef DEBUG
		debugpline("Trying to give acid resistance");
#endif
		if(!(Acid_resistance & INTRINSIC)) {
			u.cnd_eatrinsicamount++;
			You(FunnyHallu ? "wanna do more acid!" :
			    "feel less afraid of corrosives.");
			if (rn2(100)) incr_itimeout(&HAcid_resistance, rn1(1000, 500));
			else HAcid_resistance |= FROMOUTSIDE;
		}
		break;
	    case STONE_RES:
#ifdef DEBUG
		debugpline("Trying to give petrify resistance");
#endif
		if(!(HStone_resistance & INTRINSIC)) {
			u.cnd_eatrinsicamount++;
			You(FunnyHallu ? "feel stony and groovy, man." :
			    "feel rock solid.");
			if (rn2(100)) incr_itimeout(&HStone_resistance, rn1(1000, 500));
			else HStone_resistance |= FROMOUTSIDE;
		}
		break;
	    case FIRE_RES:
#ifdef DEBUG
		debugpline("Trying to give fire resistance");
#endif
		if(!(HFire_resistance & FROMOUTSIDE)) {
			u.cnd_eatrinsicamount++;
			You(FunnyHallu ? "be chillin'." :
			    "feel a momentary chill.");
			HFire_resistance |= FROMOUTSIDE;
		}
		break;
	    case SLEEP_RES:
#ifdef DEBUG
		debugpline("Trying to give sleep resistance");
#endif
		if(!(HSleep_resistance & FROMOUTSIDE)) {
			u.cnd_eatrinsicamount++;
			You_feel("wide awake.");
			HSleep_resistance |= FROMOUTSIDE;
		}
		break;
	    case COLD_RES:
#ifdef DEBUG
		debugpline("Trying to give cold resistance");
#endif
		if(!(HCold_resistance & FROMOUTSIDE)) {
			u.cnd_eatrinsicamount++;
			You_feel("full of hot air.");
			HCold_resistance |= FROMOUTSIDE;
		}
		break;
	    case DISINT_RES:
#ifdef DEBUG
		debugpline("Trying to give disintegration resistance");
#endif
		if(!(HDisint_resistance & FROMOUTSIDE)) {
			u.cnd_eatrinsicamount++;
			You_feel(FunnyHallu ?
			    "totally together, man." :
			    "very firm.");
			HDisint_resistance |= FROMOUTSIDE;
		}
		break;
	    case SHOCK_RES:	/* shock (electricity) resistance */
#ifdef DEBUG
		debugpline("Trying to give shock resistance");
#endif
		if(!(HShock_resistance & FROMOUTSIDE)) {
			u.cnd_eatrinsicamount++;
			if (FunnyHallu)
				You_feel("grounded in reality.");
			else
				Your("health currently feels amplified!");
			HShock_resistance |= FROMOUTSIDE;
		}
		break;
	    case POISON_RES:
#ifdef DEBUG
		debugpline("Trying to give poison resistance");
#endif
		if(!(HPoison_resistance & FROMOUTSIDE)) {
			u.cnd_eatrinsicamount++;
			You_feel(Poison_resistance ?
				 "especially healthy." : "healthy.");
			HPoison_resistance |= FROMOUTSIDE;
		}
		break;
	    case TELEPORT:
#ifdef DEBUG
		debugpline("Trying to give teleport");
#endif
		if(!(HTeleportation & FROMOUTSIDE)) {
			u.cnd_eatrinsicamount++;
			You_feel(FunnyHallu ? "diffuse." :
			    "very jumpy.");
			HTeleportation |= FROMOUTSIDE;
		}
		break;
	    case TELEPORT_CONTROL:
#ifdef DEBUG
		debugpline("Trying to give teleport control");
#endif
		if(!(HTeleport_control & FROMOUTSIDE)) {
			u.cnd_eatrinsicamount++;
			You_feel(FunnyHallu ?
			    "centered in your personal space." :
			    "in control of yourself.");
			HTeleport_control |= FROMOUTSIDE;
		}
		break;
	    case TELEPAT:
#ifdef DEBUG
		debugpline("Trying to give telepathy");
#endif
		if(!(HTelepat & FROMOUTSIDE)) {
			u.cnd_eatrinsicamount++;
			You_feel(FunnyHallu ?
			    "in touch with the cosmos." :
			    "a strange mental acuity.");
			HTelepat |= FROMOUTSIDE;
			/* If blind, make sure monsters show up. */
			if (Blind) see_monsters();
		}
		break;

	    case HALLU_PARTY:
		if(!(HHallu_party & FROMOUTSIDE)) {
			u.cnd_eatrinsicamount++;
			You_feel(FunnyHallu ?
			    "like throwing wild parties with lots of sexy girls! Yeah!" :
			    "a strange desire to celebrate.");
			HHallu_party |= FROMOUTSIDE;
		}
		break;
	    case DRUNKEN_BOXING:
		if(!(HDrunken_boxing & FROMOUTSIDE)) {
			u.cnd_eatrinsicamount++;
			You_feel(FunnyHallu ?
			    "like Mike Tyson!" :
			    "ready for a good brawl.");
			HDrunken_boxing |= FROMOUTSIDE;
		}
		break;
	    case STUNNOPATHY:
		if(!(HStunnopathy & FROMOUTSIDE)) {
			u.cnd_eatrinsicamount++;
			You_feel(FunnyHallu ?
			    "a controlled wobbling! Feels like being on a bouncy ship!" :
			    "steadily observant.");
			HStunnopathy |= FROMOUTSIDE;
		}
		break;
	    case NUMBOPATHY:
		if(!(HNumbopathy & FROMOUTSIDE)) {
			u.cnd_eatrinsicamount++;
			You_feel(FunnyHallu ?
			    "as if a sweet woman were clamping your toes with a block-heeled combat boot!" :
			    "a numb feeling spreading through your body. Somehow, it doesn't feel bad at all...");
			HNumbopathy |= FROMOUTSIDE;
		}
		break;
	    case DIMMOPATHY:
		if(!(HDimmopathy & FROMOUTSIDE)) {
			u.cnd_eatrinsicamount++;
			You_feel(FunnyHallu ?
			    "like your wife was contemplating a breakup, but then you realize that she's gonna stay with you to the end of all time." :
			    "a little down. But then, good feelings overcome you.");
			HDimmopathy |= FROMOUTSIDE;
		}
		break;
	    case FREEZOPATHY:
		if(!(HFreezopathy & FROMOUTSIDE)) {
			u.cnd_eatrinsicamount++;
			You_feel(FunnyHallu ?
			    "like eating a big cone of ice-cream - mmmmmmmm!" :
			    "icy.");
			HFreezopathy |= FROMOUTSIDE;
		}
		break;
	    case STONED_CHILLER:
		if(!(HStoned_chiller & FROMOUTSIDE)) {
			u.cnd_eatrinsicamount++;
			You_feel(FunnyHallu ?
			    "that you're simply the best - yeah, no shit, man!" :
			    "like relaxing on a couch.");
			HStoned_chiller |= FROMOUTSIDE;
		}
		break;
	    case CORROSIVITY:
		if(!(HCorrosivity & FROMOUTSIDE)) {
			u.cnd_eatrinsicamount++;
			You_feel(FunnyHallu ?
			    "like you just got splashed with gunks of acid!" :
			    "an acidic burning.");
			HCorrosivity |= FROMOUTSIDE;
		}
		break;
	    case FEAR_FACTOR:
		if(!(HFear_factor & FROMOUTSIDE)) {
			u.cnd_eatrinsicamount++;
			You_feel(FunnyHallu ?
			    "like you're always running - from something! And the 'something' is a prostitute." :
			    "ready to face your fears.");
			HFear_factor |= FROMOUTSIDE;
		}
		break;
	    case BURNOPATHY:
		if(!(HBurnopathy & FROMOUTSIDE)) {
			u.cnd_eatrinsicamount++;
			You_feel(FunnyHallu ?
			    "super burninated by enemy with very tired!" :
			    "a burning inside. Strangely, it feels quite soothing.");
			HBurnopathy |= FROMOUTSIDE;
		}
		break;
	    case SICKOPATHY:
		if(!(HSickopathy & FROMOUTSIDE)) {
			u.cnd_eatrinsicamount++;
			You_feel(FunnyHallu ?
			    "that you just ate some really wacky stuff! What the heck was in there?" :
			    "ill for a moment, but get the feeling that you know more about diseases now.");
			HSickopathy |= FROMOUTSIDE;
		}
		break;
	    case WONDERLEGS:
		if(!(HWonderlegs & FROMOUTSIDE)) {
			u.cnd_eatrinsicamount++;
			You_feel(FunnyHallu ?
			    "a wonderful sensation in your shins, like they were just kicked by female hugging boots! How lovely!" :
			    "like having your legs scratched up and down by sexy leather pumps.");
			HWonderlegs |= FROMOUTSIDE;
		}
		break;
	    case GLIB_COMBAT:
		if(!(HGlib_combat & FROMOUTSIDE)) {
			u.cnd_eatrinsicamount++;
			You_feel(FunnyHallu ?
			    "like an absolute marital arts champion, so you can start fighting off your spouse!" :
			    "the fliction in your hands disappearing.");
			HGlib_combat |= FROMOUTSIDE;
		}
		break;

	    default:
#ifdef DEBUG
		debugpline("Tried to give an impossible intrinsic");
#endif
		break;
	}
}

STATIC_OVL void
cpostfx(pm)		/* called after completely consuming a corpse */
register int pm;
{
	register int tmp = 0;
	boolean catch_lycanthropy = FALSE;

	/* in case `afternmv' didn't get called for previously mimicking
	   gold, clean up now to avoid `eatmbuf' memory leak */
	if (eatmbuf) (void)eatmdone();

	switch(pm) {

	    case PM_LITTLE_DOG:
	    case PM_DOG:
	    case PM_LARGE_DOG:
	    case PM_KITTEN:
	    case PM_HOUSECAT:
	    case PM_LARGE_CAT:
	    case PM_DOMESTIC_COCKATRICE:
	    case PM_CHAIN_SMOKER_ASSHOLE:
			{
			int aggroamount = rnd(6);
			if (isfriday) aggroamount *= 2;
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
			while (aggroamount) {
				u.cnd_aggravateamount++;
				makemon((struct permonst *)0, u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
				aggroamount--;
				if (aggroamount < 0) aggroamount = 0;
			}
			u.aggravation = 0;
			pline("Several monsters come out of a portal.");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			}
		break;

	    case PM_IGUANA:
	    case PM_LEGWA:
	    case PM_GECKO:
	    case PM_GEGGO:
	    case PM_FLYING_GECKO:
			lesshungry(20);
		break;
	    case PM_RHAUMBUSUN:
	    case PM_FEMBUSUN:
	    case PM_BLOODBUSUN:
			lesshungry(40);
	    case PM_SQUIRREL:
	    case PM_KWIRREL:
	    case PM_HELPFUL_SQUIRREL:
	    case PM_STAR_SQUIRREL:
			lesshungry(50);
		break;
	    case PM_BIG_IGUANA:
	    case PM_GIANT_GECKO:
			lesshungry(120);
	    case PM_BIG_RHAUMBUSUN:
			lesshungry(160);
		break;
	    case PM_SALAMANDER:
	    case PM_SALAMANDER_SHAMAN:
	    case PM_SALAMANDER_PRISONER:
	    case PM_SALAMANDER_SLAVE:
	    case PM_SALAMANDER_MAGE:
	    case PM_POISON_WHIP_SALAMANDER:
	    case PM_PARALYSIS_WHIP_SALAMANDER:
	    case PM_FROST_SALAMANDER:
	    case PM_KOMODO_DRAGON:
	    case PM_KOMODO_BEAST:
	    case PM_PETTY_KOMODO_DRAGON:
			lesshungry(400);
		break;
	    case PM_GORGON:
	    case PM_HACKBEAKTRICE:
	    case PM_ATTRACTIVE_TROVE:

		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;
		if (ABASE(A_CHA) < ATTRMAX(A_CHA)) {
			if (!rn2(10)) {
				You_feel("more %s!", flags.female ? "pretty" : "attractive");
				(void) adjattrib(A_CHA, 1, FALSE, TRUE);
				break;
			}
		}
		break;

	    case PM_DRACOLISK:
	    case PM_COCKENTRICE:
	    case PM_CHARISMA_TROVE:
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;
		if (ABASE(A_CHA) < ATTRMAX(A_CHA)) {
			You_feel("more %s!", flags.female ? "pretty" : "attractive");
			(void) adjattrib(A_CHA, 1, FALSE, TRUE);
			}
		break;

	    case PM_STONING_MONSTER:
	    case PM_TOUGH_TROVE:
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;
		if (ABASE(A_CON) < ATTRMAX(A_CON)) {
			You_feel("tougher!");
			(void) adjattrib(A_CON, 1, FALSE, TRUE);
			}

		break;

	    case PM_PORTBEAMTRICE:

		pushplayer(TRUE);
		break;

	    case PM_EVILISK:
	    case PM_ADULT_SHIMMERING_DRAGON:
	    case PM_OLD_SHIMMERING_DRAGON:
	    case PM_SHIMMERING_DRAGON:
	    case PM_VERY_OLD_SHIMMERING_DRAGON:
	    case PM_ANCIENT_SHIMMERING_DRAGON:
	    case PM_GREAT_SHIMMERING_DRAGON:
	    case PM_GARGANTUAN_COCKATRICE:

		Your("intrinsics seem to change!");
		intrinsicgainorloss();
		break;

	    case PM_JUNOW_TRICE:
		Your("intrinsics seem to change!");
		intrinsicgainorloss();

		if (!rn2(20)) {

			if(!(HPolymorph & FROMOUTSIDE)) {
				You_feel(FunnyHallu ?
				    "able to take on different shapes! Yeah! Let's transform into something fun!" :
				    "unstable.");
				HPolymorph  |= FROMOUTSIDE;
			}

		}

		if (!Unchanging) {
		    You_feel("a change coming over you.");
		    polyself(FALSE);
		}

		break;

	    case PM_SMALL_CHICKATRICE:
	    case PM_NEWT:
	    case PM_DRAGONEWT:
	    case PM_CLAYMORE_DRAGONEWT:
	    case PM_DRACO_EQUES:
	    case PM_SUPER_DRAGONEWT:
	    case PM_PINBALL_NEWT:
	    case PM_NUUT:
	    case PM_ENERGY_TROVE:
	    case PM_GRAY_NEWT:
	    case PM_ARCH_NEWT:
	    case PM_REMORHAZ:
	    case PM_DEMINEWT:
	    case PM_DEMILOVATO_NEWT:
	    case PM_MASTER_NEWT:
	    case PM_EMPEROR_NEWT:
	    case PM_LARGE_NEWT:
	    case PM_ORE_NEWT:
	    case PM_MINNEWT:
	    case PM_SENSEI_NEWT:
	    case PM_GRANDMASTER_NEWT:
	    case PM_ASPHYNX:
	    case PM_SHOCKATRICE:
	    case PM_RUBBER_CHICKEN:
	    case PM_NASTY_CHICKEN:
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;
		/* MRKR: "eye of newt" may give small magical energy boost */

		if (issoviet && rn2(3)) {
			pline("Sovetskaya ne khochet, chtoby vy, chtoby poluchit' magicheskuyu silu! Vasha zhizn' otstoy!");
			break;
		}

		if (rn2(3)/* || 3 * u.uen <= 2 * u.uenmax*/) {
		    int old_uen = u.uen; /* Some slight changes to this code. --Amy */
		    u.uen += rnd(3);
		    /*if (u.uen > u.uenmax) { */
			if (!rn2(3)) u.uenmax++;
			/*u.uen = u.uenmax;
		    } */
		    if (u.uen > u.uenmax) u.uen = u.uenmax;
		    if (old_uen != u.uen) {
			    You_feel("a mild buzz.");
			    flags.botl = 1;
		    }
		}
		break;
	    case PM_POWER_TROVE:
	    case PM_ULTIMATE_NEWT:
	    case PM_HUGE_NEWT:
	    case PM_EIGHT_FOOTED_SNAKE:
	    case PM_NASTY_TURBO_CHICKEN:
	    case PM_WINGED_KRAKEN_LICHATRICE:
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;
	      {int old_uen = u.uen;
		if (rn2(3)) {
		    u.uen += rnd(3);
		    if (!rn2(3)) u.uenmax++;
		}
		if (rn2(3)) {
		    u.uen += rnd(3);
		    if (!rn2(3)) u.uenmax++;
		}
		if (rn2(3)) {
		    u.uen += rnd(3);
		    if (!rn2(3)) u.uenmax++;
		}
		if (u.uen > u.uenmax) u.uen = u.uenmax;
		if (old_uen != u.uen) {
		    You_feel("a mild buzz.");
		    flags.botl = 1;
		}
		}
		break;
	    case PM_GIGANTIC_NEWT:
	    case PM_BONUS_MANA_TROVE:
	    case PM_KILLERTRICE:
	    case PM_MULTITRICE:
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;
	      {int old_uen = u.uen;
		    u.uen += rnd(4);
		    if (rn2(3)) u.uenmax++;
		    u.uen += rnd(4);
		    if (rn2(3)) u.uenmax++;
		    u.uen += rnd(4);
		    if (rn2(3)) u.uenmax++;
			u.uenmax++; u.uenmax++; u.uenmax++;
		if (u.uen > u.uenmax) u.uen = u.uenmax;
		if (old_uen != u.uen) {
		    You_feel("a mild buzz.");
		    flags.botl = 1;
		}
		}
		break;
	    case PM_TWEN:
	    case PM_PINBALL_TWEN:
	    case PM_DWEN:
	    case PM_HEALTH_TROVE:
	    case PM_GRAY_TWEN:
	    case PM_DEMITWEN:
	    case PM_MASTER_TWEN:
	    case PM_GRANDMASTER_TWEN:
	    case PM_LARGE_TWEN:
	    case PM_ORE_TWEN:
	    case PM_MINTWEN:
	    case PM_EMPEROR_TWEN:
	    case PM_SENSEI_TWEN:
	    case PM_YELDUD_TWEN:
	    case PM_PORTER_RUBBER_CHICKEN:
	    case PM_CHALKATRICE:
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;
		if (Upolyd) {
			u.mh++;
			u.mhmax++;
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		} else {
			u.uhp++;
			u.uhpmax++;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		}
		if (uactivesymbiosis) {
			u.usymbiote.mhpmax++;
			if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
		}
		    You_feel("vitalized.");
		    flags.botl = 1;
		break;
	    case PM_VITALITY_TROVE:
	    case PM_MUTATED_UNDEAD_COCKATRICE:
	    case PM_ULTIMATE_TWEN:
	    case PM_HUGE_TWEN:
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;
		if (Upolyd) {
			u.mh++;
			u.mh += rnd(2);
			u.mhmax++;
			u.mhmax += rnd(2);
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		} else {
			u.uhp++;
			u.uhp += rnd(2);
			u.uhpmax++;
			u.uhpmax += rnd(2);
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		}
		if (uactivesymbiosis) {
			u.usymbiote.mhpmax++;
			u.usymbiote.mhpmax += rnd(2);
			if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
		}

		    You_feel("vitalized.");
		    flags.botl = 1;
		break;

	    case PM_GIGANTIC_TWEN:
	    case PM_KILLER_CHICKEN:
	    case PM_HITPOINT_TROVE:
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;
		if (Upolyd) {
			u.mh++;
			u.mh++;
			u.mh++;
			u.mh += rnd(10);
			u.mhmax++;
			u.mhmax++;
			u.mhmax++;
			u.mhmax += rnd(10);
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		} else {
			u.uhp++;
			u.uhp++;
			u.uhp++;
			u.uhp += rnd(10);
			u.uhpmax++;
			u.uhpmax++;
			u.uhpmax++;
			u.uhpmax += rnd(10);
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		}
		if (uactivesymbiosis) {
			u.usymbiote.mhpmax++;
			u.usymbiote.mhpmax++;
			u.usymbiote.mhpmax++;
			u.usymbiote.mhpmax += rnd(10);
			if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
		}
		    You_feel("vitalized.");
		    flags.botl = 1;
		break;

	    case PM_KILLER_TURBO_CHICKEN:
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;

	      {int old_uen = u.uen;
		    u.uen += rnd(4);
		    if (rn2(3)) u.uenmax++;
		    u.uen += rnd(4);
		    if (rn2(3)) u.uenmax++;
		    u.uen += rnd(4);
		    if (rn2(3)) u.uenmax++;
			u.uenmax++; u.uenmax++; u.uenmax++;
		if (u.uen > u.uenmax) u.uen = u.uenmax;
		if (old_uen != u.uen) {
		    You_feel("a mild buzz.");
		    flags.botl = 1;
		}
		}

		if (Upolyd) {
			u.mh++;
			u.mh++;
			u.mh++;
			u.mh += rnd(10);
			u.mhmax++;
			u.mhmax++;
			u.mhmax++;
			u.mhmax += rnd(10);
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		} else {
			u.uhp++;
			u.uhp++;
			u.uhp++;
			u.uhp += rnd(10);
			u.uhpmax++;
			u.uhpmax++;
			u.uhpmax++;
			u.uhpmax += rnd(10);
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		}
		if (uactivesymbiosis) {
			u.usymbiote.mhpmax++;
			u.usymbiote.mhpmax++;
			u.usymbiote.mhpmax++;
			u.usymbiote.mhpmax += rnd(10);
			if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
		}
		    You_feel("vitalized.");
		    flags.botl = 1;
		break;

	    case PM_PETRO_MOLD:
	    case PM_PETRO_FUNGUS:
	    case PM_PETRO_PATCH:
	    case PM_PETRO_FORCE_FUNGUS:
	    case PM_PETRO_WORT:
	    case PM_PETRO_FORCE_PATCH:
	    case PM_PETRO_WARP_FUNGUS:
	    case PM_PETRO_WARP_PATCH:
	    case PM_PETRO_STALK:
	    case PM_PETRO_SPORE:
	    case PM_PETRO_MUSHROOM:
	    case PM_PETRO_GROWTH:
	    case PM_PETRO_COLONY:
	    case PM_CLAIRVOYANT_TROVE:
	    case PM_RANDOM_COCKATRICE:
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;
		    You_feel("clairvoyant!");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vy znayete raspolozheniye, no ne lovushki, a te vse ravno budut poshel na khuy vverkh." : "Wschiiiiiiie!");
			incr_itimeout(&HClairvoyant, rnd(500));
		break;
	    case PM_STONING_SPHERE:
	    case PM_LEVELING_TROVE:
	    case PM_DREAMER_COCKATRICE:
	    case PM_DISINTITRICE:
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;
	      You_feel("that was a smart thing to do.");
		gainlevelmaybe();

		break;

	    case PM_STONE_BUG:
		You_feel("that was a bad idea.");
		losexp("eating a stone bug corpse", FALSE, TRUE);

		break;

	    case PM_GIANT_COCKATRICE:
		You_feel("that was a bad idea.");
		losexp("eating a giant cockatrice corpse", FALSE, TRUE);

		break;

	    case PM_WAFER_THIN_MINT: /* by NCommander */
			{
		/* MC: Because I love bad jokes and this was too obvious */
			char buf[BUFSZ];
			/* Because a joke is worth overdoing */

			/* The original skit had Creosote as the character who exploded */
			if (!strcmpi(plname, "Croesus")
			|| !strcmpi(plname, "Kroisos")
			|| !strcmpi(plname, "Creosote")
			|| !strcmpi(plalias, "Croesus")
			|| !strcmpi(plalias, "Kroisos")
			|| !strcmpi(plalias, "Creosote")) {
				pline("You suddenly have a flashback to that horrid restaurant!");
				u.youaredead = 1;
				sprintf(buf, "fatal post-tramatic dining experience");
				killer = buf;
				killer_format = NO_KILLER_PREFIX;
				done(DIED);
				/* note by Amy to NCommander: the 'youaredead' flag should only be set during a pending instadeath,
				 * and be reset to 0 after the instadeath has happened, so the player doesn't get killed for
				 * saving the game after lifesaving! */
				u.youaredead = 0;
			}

			/* If we're saturated, we'll puke */
			if (u.uhunger >= 3000) { /* values adjusted to slex values --Amy */
				pline("You make some room to get it down!");
				make_vomiting(Vomiting+d(10,4), TRUE);
				if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */
				pline("There we go ...");
			}

			/* If we're still oversaturated ... */
			if (u.uhunger >= 4000) {
				You_feel("like your chest is going to explode.");
				losestr(rnd(4), TRUE);
				losehp(rnd(15), "gluttony", KILLED_BY);
				pline("It did!");
			}

			/* The skit ended with Mr Creosote being presented with the check */
			pline("Check please.");

			}

		break;

	    case PM_WRAITH:
	    case PM_BITCHY_LITTLE_ONE:
	    case PM_CORONA_WRAITH:
	    case PM_WIGHTSPECTRE:
	    case PM_KING_WRAITH:
	    case PM_GUD_WRAITH:
	    case PM_CREVICE_WRAITH:
	    case PM_HUDDLED_WRAITH:
	    case PM_HITTABLE_WRAITH:
	    case PM_ACID_WRAITH:
	    case PM_HUMAN_WRAITH:
	    case PM_BIG_WRAITH:
	    case PM_HUGE_WRAITH:
	    case PM_GIGANTIC_WRAITH:
	    case PM_WRAITH_SHAMAN:
	    case PM_NASTY_WRAITH:
	    case PM_TURBO_CHICKEN:
	    case PM_GREATER_BASILISK:
	    case PM_ETHEREAL_TROVE:
	    case PM_CENTAURTRICE:
	    case PM_SPICKATRICE:
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;
		switch(rnd(10)) {                
		case 1:
		    You_feel("that was a bad idea.");
		    losexp("eating a wraith corpse", FALSE, TRUE);
		    break;
		case 2:                        
		    You("don't feel so good ...");
		    if (Upolyd) {
			u.mhmax -= 4;
			if (u.mhmax < 1) u.mhmax = 1;
		    } else {
			u.uhpmax -= 4;
			if (u.uhpmax < 1) u.uhpmax = 1;
		    }
		    u.uenmax -= 8;
		    if (u.uenmax < 1) u.uenmax = 1;
		    u.uen -= 8;
		    if (u.uen < 0) u.uen = 0;
		    losehp(4, "eating a wraith corpse", KILLED_BY);
		    break;
		case 3:                        
		case 4: 
		    You_feel("something strange for a moment.");
		    break;
		case 5: 
		    You_feel("physically and mentally stronger!");
		    if (Upolyd) {
			u.mhmax += 4;
			u.mh = u.mhmax;
		    } else {
			u.uhpmax += 4;
			u.uhp = u.uhpmax;
		    }
		    u.uenmax += 8;
		    u.uen = u.uenmax;
		    break;
		case 6:                        
		case 7: 
		case 8:
		case 9:                        
		case 10:                
		    You_feel("that was a smart thing to do.");
		gainlevelmaybe();
		break;
		default:            
		    break;
		}
		flags.botl = 1;
		break;
	    case PM_CREEPING___:
	    case PM_LEVEL___:
	    case PM_PETRO_CENTIPEDE:
	    case PM_PSEUDO_MEDUSA:
	    case PM____TROVE:
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;
		You_feel("appropriately 42!");
		switch(rnd(10)) {                
		case 1:
		    You_feel("that was a bad idea.");
		    losexp("eating a creeping 42 corpse", FALSE, TRUE);
			make_hallucinated(HHallucination+rn1(10,50),TRUE,0L);
		    break;
		case 2:                        
		    You("don't feel so good ...");
		    if (Upolyd) {
			u.mhmax -= 4;
			if (u.mhmax < 1) u.mhmax = 1;
		    } else {
			u.uhpmax -= 4;
			if (u.uhpmax < 1) u.uhpmax = 1;
		    }
		    u.uenmax -= 8;
		    if (u.uenmax < 1) u.uenmax = 1;
		    u.uen -= 8;
		    if (u.uen < 0) u.uen = 0;
		    losehp(4, "eating a creeping 42 corpse", KILLED_BY);
			make_hallucinated(HHallucination+rn1(10,50),TRUE,0L);
		    break;
		case 3:                        
		case 4: 
		    You_feel("something strange for a moment.");
			make_hallucinated(HHallucination+rn1(10,50),TRUE,0L);
		    break;
		case 5: 
		    You_feel("physically and mentally stronger!");
		    if (Upolyd) {
			u.mhmax += 4;
			u.mh = u.mhmax;
		    } else {
			u.uhpmax += 4;
			u.uhp = u.uhpmax;
		    }
		    u.uenmax += 8;
		    u.uen = u.uenmax;
			make_hallucinated(HHallucination+rn1(10,50),TRUE,0L);
		    break;
		case 6:                        
		case 7: 
		case 8:
		case 9:                        
		case 10:                
		    You_feel("that was a smart thing to do.");
		gainlevelmaybe();
		make_hallucinated(HHallucination+rn1(10,50),TRUE,0L);
		break;
		default:            
		    break;
		}
		flags.botl = 1;
		break;
	    case PM_HUMAN_WERERAT:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WERERAT;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WERESTALKER:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WERESTALKER;
			u.cnd_lycanthropecount++;
		}

		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;
		if(!Invis) {
			set_itimeout(&HInvis, (long)rn1(100, 50));
			if (!Blind && !BInvis) self_invis_message();
		} else {
			if (!(HInvis & INTRINSIC)) You_feel("hidden!");
			HInvis |= FROMOUTSIDE;
			HSee_invisible |= FROMOUTSIDE;
		}
		newsym(u.ux, u.uy);
		make_stunned(HStun + 60,FALSE);

		break;
	    case PM_HUMAN_WERERABBIT:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WERERABBIT;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREACIDBLOB:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREACIDBLOB;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREFOX:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREFOX;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREMONKEY:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREMONKEY;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREFLOATINGEYE:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREFLOATINGEYE;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREGIANTANT:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREGIANTANT;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREKOBOLD:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREKOBOLD;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WERELEPRECHAUN:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WERELEPRECHAUN;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREHORSE:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREHORSE;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WERERUSTMONSTER:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WERERUSTMONSTER;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREBLACKLIGHT:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREBLACKLIGHT;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREAUTON:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREAUTON;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREGREMLIN:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREGREMLIN;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREGREENSLIME:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREGREENSLIME;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREJELLY:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREJELLY;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREXAN:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREXAN;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREMUMAK:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREMUMAK;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WERECENTAUR:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WERECENTAUR;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WERECOUATL:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WERECOUATL;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREGELATINOUSCUBE:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREGELATINOUSCUBE;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREAIRELEMENTAL:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREAIRELEMENTAL;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREEARTHELEMENTAL:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREEARTHELEMENTAL;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREFIREELEMENTAL:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREFIREELEMENTAL;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREWATERELEMENTAL:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREWATERELEMENTAL;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREASTRALELEMENTAL:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREASTRALELEMENTAL;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREXORN:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREXORN;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREDRACONIAN:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREDRACONIAN;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREUMBERHULK:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREUMBERHULK;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREVAMPIRE:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREVAMPIRE;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREKILLERBEE:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREKILLERBEE;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREKANGAROO:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREKANGAROO;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREGRUE:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREGRUE;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WERECOINS:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WERECOINS;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WERETRAPPER:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WERETRAPPER;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WERESHADE:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WERESHADE;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREDISENCHANTER:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREDISENCHANTER;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WERENAGA:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WERENAGA;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREWORM:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREWORM;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREDRAGON:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREDRAGON;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WERELOCUST:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WERELOCUST;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREBOAR:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREBOAR;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_VORPAL_WERE_ALHOONTRICE_ZOMBIE:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_VORPAL_WERE_ALHOONTRICE_ZOMBIE;
			u.cnd_lycanthropecount++;
		}

		Your("velocity suddenly seems very uncertain!");
		if (HFast & INTRINSIC) {
			HFast &= ~INTRINSIC;
			You("seem slower.");
		} else {
			HFast |= FROMOUTSIDE;
			You("seem faster.");
		}

		break;
	    case PM_HUMAN_WEREJACKAL:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREJACKAL;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREWOLF:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREWOLF;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREPANTHER:            
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREPANTHER;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WERETIGER:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WERETIGER;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WERESNAKE:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WERESNAKE;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WERESPIDER:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WERESPIDER;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREPIRANHA:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREPIRANHA;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREEEL:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREEEL;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREFLYFISH:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREFLYFISH;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREKRAKEN:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREKRAKEN;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREGHOST:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREGHOST;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREGIANT:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREGIANT;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WERELICHEN:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WERELICHEN;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREVORTEX:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREVORTEX;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WERECOW:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WERECOW;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREBEAR:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREBEAR;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREBRONZEGRAM:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREBRONZEGRAM;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WERECHROMEGRAM:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WERECHROMEGRAM;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREDEMON:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREDEMON;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREPHANT:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREPHANT;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREWEDGESANDAL:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREWEDGESANDAL;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREHUGGINGBOOT:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREHUGGINGBOOT;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREPEEPTOE:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREPEEPTOE;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WERESEXYLEATHERPUMP:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WERESEXYLEATHERPUMP;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREBLOCKHEELEDCOMBATBOOT:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREBLOCKHEELEDCOMBATBOOT;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WERECOMBATSTILETTO:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WERECOMBATSTILETTO;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREBEAUTIFULFUNNELHEELEDPUMP:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREBEAUTIFULFUNNELHEELEDPUMP;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREPROSTITUTESHOE:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREPROSTITUTESHOE;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WERESTILETTOSANDAL:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WERESTILETTOSANDAL;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREUNFAIRSTILETTO:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREUNFAIRSTILETTO;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREWINTERSTILETTO:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREWINTERSTILETTO;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREPIERCER:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREPIERCER;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREPENETRATOR:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREPENETRATOR;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WERESMASHER:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WERESMASHER;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WERENYMPH:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WERENYMPH;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREBEE:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREBEE;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WERESOLDIERANT:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WERESOLDIERANT;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WERETROLL:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WERETROLL;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREBAT:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREBAT;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREGRIDBUG:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREGRIDBUG;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WERELICH:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WERELICH;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREJABBERWOCK:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREJABBERWOCK;
			u.cnd_lycanthropecount++;
		}
		break;

	    case PM_HUMAN_WERECOCKATRICE:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WERECOCKATRICE;
			u.cnd_lycanthropecount++;
		}

		Your("velocity suddenly seems very uncertain!");
		if (HFast & INTRINSIC) {
			HFast &= ~INTRINSIC;
			You("seem slower.");
		} else {
			HFast |= FROMOUTSIDE;
			You("seem faster.");
		}
		break;
	    case PM_HUMAN_WEREMIMIC:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREMIMIC;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_HUMAN_WEREPERMAMIMIC:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREMIMIC;
			u.cnd_lycanthropecount++;
		}
		break;
	    case PM_NOVICE_NURSE:
	    case PM_CONTROL_NURSE:
	    case PM_NURSE:
	    case PM_EXPERIENCED_NURSE:
	    case PM_SEXY_NURSE:
	    case PM_TOPLESS_NURSE:
	    case PM_HEALING_TROVE:
	    case PM_GORGON_FLY:
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;
		if (Upolyd) u.mh = u.mhmax;
		else u.uhp = u.uhpmax;
		flags.botl = 1;
		break;
	    case PM_HIDDEN_ENGULFITRICE:
	    case PM_HIDDEN_TROVE:
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;

		if (!(HInvis & INTRINSIC)) You_feel("hidden!");
		HInvis |= FROMOUTSIDE;
		HSee_invisible |= FROMOUTSIDE;

		break;

	    case PM_STALKER:
	    case PM_FORCE_STALKER:
	    case PM_LANTERN_STALKER:
	    case PM_GREEN_STALKER:
	    case PM_RED_STALKER:
	    case PM_YELLOW_STALKER:
	    case PM_WHITE_STALKER:
	    case PM_CYAN_STALKER:
	    case PM_THE_HIDDEN:
	    case PM_INVISIBLE_BADGUY:
	    case PM_UNSEEN_POTATO:
	    case PM_CAMOUFLAGED_WATCHER:
	    case PM_HIDDEN_TRACKER:
	    case PM_UNSEEN_SERVANT:
	    case PM_SCHEDAU_STALKER:
	    case PM_SILENT_KILLER:
	    case PM_STONE_STALKER:
	    case PM_ILLUSION_WEAVER:
	    case PM_INVIS_SAMER:
	    case PM_INVIS_SCORER:
	    case PM_MIRAGE_WEAVER:
	    case PM_PAIN_MASTER:
	    case PM_PAIN_MISTER:
	    case PM_COCKTAUR:
	    case PM_REVEALING_COCKATRICE:
	    case PM_HIDDEN_COCKATRICE:
	    case PM_MIMIC_CHICKEN:
	    case PM_PETRO_MIMIC:
	    case PM_PETRO_PERMAMIMIC:
	    case PM_INVISO_TROVE:
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;
		if(!Invis) {
			set_itimeout(&HInvis, (long)rn1(100, 50));
			if (!Blind && !BInvis) self_invis_message();
		} else {
			if (!(HInvis & INTRINSIC)) You_feel("hidden!");
			HInvis |= FROMOUTSIDE;
			HSee_invisible |= FROMOUTSIDE;
		}
		newsym(u.ux, u.uy);
		/* fall into next case */
	    case PM_YELLOW_LIGHT:
		/* fall into next case */
	    case PM_CHICKATRICE:
		make_stunned(HStun + 30,FALSE);

		/* bats are generalized with is_bat now --Amy */
		break;
	    case PM_QUANTUM_MECHANIC:
	    case PM_SCREWER_MECHANIC:
	    case PM_QUANTUM_CREATURE:
	    case PM_ECO_MECHANIC:
	    case PM_COMO_MECHANIC:
	    case PM_MATHEMATICIAN:
	    case PM_TOLKIEN_MECHANIC:
	    case PM_FORGER_MECHANIC:
	    case PM_IMPORTANT_MECHANIC:
	    case PM_UNSEEN_MECHANIC:
	    case PM_HASTY_MECHANIC:
	    case PM_COUNTER_MECHANIC:
	    case PM_DISMANTLER_MECHANIC:
	    case PM_PLAYER_MECHANIC:
	    case PM_QUANTUM_ABERRATION:
	    case PM_QUANTUMMOID:
	    case PM_ATOMIC_QUANTUM_MECHANIC:
	    case PM_FORCE_QUANTUM_MECHANIC:
	    case PM_METAL_MECHANIC:
	    case PM_AUTO_MECHANIC:
	    case PM_COMMUNITY_SPREAD_MECHANIC:
	    case PM_GORGON_BEETLE:
	    case PM_SPEED_TROVE:
	    case PM_MINI_CHICKATRICE:
	    case PM_SPEEDOTRICE:
	    case PM_UNCERTAINTY_DEMON:
	    case PM_HISPEED_CHICKEN:
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;
		Your("velocity suddenly seems very uncertain!");
		if (HFast & INTRINSIC) {
			HFast &= ~INTRINSIC;
			You("seem slower.");
		} else {
			HFast |= FROMOUTSIDE;
			You("seem faster.");
		}
		break;

	    case PM_RACEY_CHICKEN:
	    case PM_QUADWANGLER:

		if (!Very_fast)
			You("are suddenly moving %sfaster.", Fast ? "" : "much ");
		else {
			Your("%s get new energy.", makeplural(body_part(LEG)));
		}
		incr_itimeout(&HFast, rn1(1000, 1000));
		break;

	    case PM_PREHISTORIC_CAVE_LIZARD:
	    case PM_LIZARD_OF_YENDOR:
	    case PM_LICHZARD:
			lesshungry(500); /* fall thru */
	    case PM_GIANT_LIZARD:
	    case PM_LIZARD_MAGE:
	    case PM_BLACK_LIZARDMAN:
	    case PM_ASSASSIN_LIZARD:
	    case PM_BLIZZARD_LIZARD:
			lesshungry(300); /* fall thru */
	    case PM_CHAOS_LIZARD:
	    case PM_CHAOTIC_LIZARD:
	    case PM_CASINO_LIZARD:
	    case PM_LIZARD_MAN:
	    case PM_SKELLIZARD:
	    case PM_NORMAL_LIZARD:
	    case PM_TOUCAN:
			lesshungry(180); /* fall thru */
	    case PM_LIZARD:
	    case PM_LIZZY:
	    case PM_INNOCLIZARD:
	    case PM_FAT_LIZARD:
	    case PM_LIZARD_PRINCE:
	    case PM_WILL_STONE_LIZARD:
	    case PM_WILL_RATCH_LIZARD:
	    case PM_ROCK_LIZARD:
	    case PM_BABY_CAVE_LIZARD:
	    case PM_NIGHT_LIZARD:
	    case PM_KARMIC_LIZARD:
	    case PM_GREEN_LIZARD:
	    case PM_SCORZARD:
	    case PM_CAVE_LIZARD:
	    case PM_GRASS_LIZARD:
	    case PM_RUNE_LIZARD:
	    case PM_SPECTRAL_LIZARD:
	    case PM_BLUE_LIZARD:
	    case PM_SWAMP_LIZARD:
	    case PM_SPITTING_LIZARD:
	    case PM_LIZARD_EEL:
	    case PM_LIZARD_KING:
	    case PM_ADULT_LIZARD:
	    case PM_EEL_LIZARD:
	    case PM_ANTI_STONE_LIZARD:
	    case PM_HIDDEN_LIZARD:
	    case PM_CLINGING_LIZARD:
	    case PM_PRESSLIZARD:
	    case PM_STATIOLIZARD:
	    case PM_GIBLIZARD:
			lesshungry(20); /* fall thru */
	    case PM_UNDEAD_COCKATRICE:
		if (HStun > 2)  make_stunned(2L,FALSE);
		if (HConfusion > 2)  make_confused(2L,FALSE);
		break;

	    case PM_MONSTER_LIZARD:
			lesshungry(1500);
		if (HStun > 2)  make_stunned(2L,FALSE);
		if (HConfusion > 2)  make_confused(2L,FALSE);
		break;

	    case PM_GIANT_TOUCAN:
			lesshungry(2000);
		if (HStun > 2)  make_stunned(2L,FALSE);
		if (HConfusion > 2)  make_confused(2L,FALSE);
		break;

	    case PM_HUGE_LIZARD:
	    case PM_SAND_TIDE:
	    case PM_FBI_AGENT:
	    case PM_OWN_SMOKE:
	    case PM_GRANDPA:
	    case PM_CLOCKBACK_LIZARD:
	    case PM_FIRE_LIZARD:
	    case PM_BLACK_LIZARD:
	    case PM_LIGHTNING_LIZARD:
	    case PM_ICE_LIZARD:
			lesshungry(120);
		if (HStun > 2)  make_stunned(2L,FALSE);
		if (HConfusion > 2)  make_confused(2L,FALSE);
		break;

	    case PM_MIMIC_LIZARD:
	    case PM_HELTH_LIZARD:
	    case PM_DEFORMED_LIZARD:
	    case PM_GRATING_CHICKEN:
			lesshungry(20); /* fall thru */
		if (HStun > 2)  make_stunned(2L,FALSE);
		if (HConfusion > 2)  make_confused(2L,FALSE);

		break;

	    case PM_PYROLETROLISK:
		if ((HFire_resistance & FROMOUTSIDE) && (HShock_resistance & FROMOUTSIDE)) {
			HFire_resistance &= ~FROMOUTSIDE;
			HShock_resistance &= ~FROMOUTSIDE;
			You_feel("like you got zapped by fire!");
		} else if (HFire_resistance & FROMOUTSIDE) {
			HFire_resistance &= ~FROMOUTSIDE;
			HShock_resistance |= FROMOUTSIDE;
			You("switched fire resistance for shock resistance.");
		} else if (HShock_resistance & FROMOUTSIDE) {
			HShock_resistance &= ~FROMOUTSIDE;
			HFire_resistance |= FROMOUTSIDE;
			You("switched shock resistance for fire resistance.");
		}
		break;

	    case PM_CHAMELEON:
	    case PM_CHAMECHAUN:
	    case PM_METAMORPHOSE:
	    case PM_UNIQUE_SHIFTER:
	    case PM_GHELEON:
	    case PM_PURPLE_R:
	    case PM_VAMPSHIFTER:
	    case PM_UNGENOCIDABLE_VAMPSHIFTER:
	    case PM_CHARMONIE:
	    case PM_EDOTO:
	    case PM_COCKAMELEON:
	    case PM_GREEN_SLAAD:
	    case PM_POLYFESHNEE:
	    case PM_FOREPREACHER_CONVERTER:
	    case PM_MARTIIN:
	    case PM_IVEL_WUXTINA:
	    case PM_EARLY_LEON:
	    case PM_CHAMELON:
	    case PM_COMMA_CHAMELEON:
	    case PM_SLUMBER_HULK:
	    case PM_OFFDIVER:
	    case PM_CHANGELING:
	    case PM_PLAYER_CHANGELING:
	    case PM_CHANGELING_ZOMBIE:
	    case PM_CHANGELING_MUMMY:
	    case PM_ELEROTIC_DREAM_WOMAN:
	    case PM_TENDER_JESSE:
	    case PM_WHORED_HORE:
	    case PM_SHOEMELEON:
	    case PM_LULU_ASS:
	    case PM_DARN_DEMENTOR:
	    case PM_WILD_CHANGE_NYMPH:
	    case PM_VERY_POLY_NYMPH:
	    case PM_KARMA_CHAMELEON:
	    case PM_CHANGERING_KELPIE:
	    case PM_EVIL_CHAMELEON:
	    case PM_DOPPELGANGER:
	    case PM_METAL_DOPPELGANGER:
	    case PM_LAURA_S_PARLOR_TRICK:
	    case PM_LAURA_S_MASTERPIECE:
	    case PM_CHANGE_EXPLODER:
	    case PM_CORTEGEX:
	    case PM_TSCHANG_SEPHIRAH:
	    case PM_KUSCHOING_SEPHIRAH:
	    case PM_GLONK_SEPHIRAH:
	    case PM_DOPPLEZON:
	    case PM_ULTRA_DESTRUCTIVE_MONSTER:
	    case PM_BAM_CHAM:
	    case PM_TRANSFORMER:
	    case PM_CHAOS_SHAPECHANGER:
	    case PM_WARPER:
	    case PM_RICTIM_TERRORIZER:
	    case PM_PEANUT__BUTTER_AND_JELLY_SANDWICH:
	    case PM_SANDESTIN:
	    case PM_POLYMORPHITIC_WOLF:
	    case PM_COVETOUSLEON:
	    case PM_THE_ZRUTINATOR:
	    case PM_GIANT_CHAMELEON:
	    case PM_MEDUQUASIT:
	    case PM_FICKATRICE:
	    case PM_PUCE_JELLY:

		/* evil patch idea by jonadab: eating shapeshifters has a 5% chance of conveying polymorphitis */
		if (!rn2(20)) {

			if(!(HPolymorph & FROMOUTSIDE)) {
				You_feel(FunnyHallu ?
				    "able to take on different shapes! Yeah! Let's transform into something fun!" :
				    "unstable.");
				HPolymorph  |= FROMOUTSIDE;
			}

		}

		if (!Unchanging) {
		    You_feel("a change coming over you.");
		    polyself(FALSE);
		}
		break;

	    case PM_UMBER_MIND_FLAYER:
	    case PM_GLOCKATRICE:
	    case PM_BOSSATRICE:

		You_feel("ethereal.");
		incr_itimeout(&HPasses_walls, rn1(10, 50));

		break;

	    case PM_HENRIETTA_S_THICK_BLOCK_HEELED_BOOT:
	    case PM_VERY_STEAL_MIND_FLAYER:
	    case PM_CONTACT_BEASTLING:

		if(!(HPolymorph & FROMOUTSIDE)) {
			You_feel(FunnyHallu ?
			    "able to take on different shapes! Yeah! Let's transform into something fun!" :
			    "unstable.");
			HPolymorph  |= FROMOUTSIDE;
		}

		break;
	    case PM_GENETIC_ENGINEER: /* Robin Johnson -- special msg */
	    case PM_ARMED_COCKATRICE:
	    case PM_PORTER_GENETIC_ENGINEER:
	    case PM_PETROLICH:
		if (!Unchanging) {
		    You("undergo a freakish metamorphosis!");
		    polyself(FALSE);
		}
		break;

	    case PM_POLY_FLAYER:

		if (!rn2(20)) {

			if(!(HPolymorph & FROMOUTSIDE)) {
				You_feel(FunnyHallu ?
				    "able to take on different shapes! Yeah! Let's transform into something fun!" :
				    "unstable.");
				HPolymorph  |= FROMOUTSIDE;
			}

		}

		if (!Unchanging) {
		    You_feel("a change coming over you.");
		    polyself(FALSE);
		}

		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;

		if (ABASE(A_INT) < ATTRMAX(A_INT)) {
			if (!rn2(2)) {
				pline(FunnyHallu ? "Hmm, is that what human brain tastes like?" : "Yum! That was real brain food!");
				(void) adjattrib(A_INT, 1, FALSE, TRUE);
				break;	/* don't give them telepathy, too */
			}
		}
		else {
			pline(FunnyHallu ? "Eek, that tasted like rotten oversalted seaweed!" : "For some reason, that tasted bland.");
		}

		break;

	    case PM_FOODSTUFFS_MIMIC:
	    case PM_FOODSTUFFS_PERMAMIMIC:
		CrapEffect += rnz(10000);
		pline("Ugh-Ugh, your butt suddenly doesn't feel so good...");

		break;

	    case PM_BETTINA_S_COMBAT_BOOT:

		pline("Ulch - the heel was tainted!");
	      make_sick(rn1(25,25), "a tainted combat boot", TRUE, SICK_VOMITABLE);

		break;

	    case PM_ROTTEN_EXHAUSTED_NOODLE:

		pline("Ulch - the noodle was tainted!");
	      make_sick(rn1(25,25), "a tainted noodle", TRUE, SICK_VOMITABLE);

		break;

	    case PM_HOPE_COCKATRICE:

		pline("Ulch - the cockatrice was tainted!");
	      make_sick(rn1(25,25), "a tainted cockatrice", TRUE, SICK_VOMITABLE);

		break;

	    case PM_NAPT_GIRL:

		pline("Ulch - the asian girl was tainted!");
	      make_sick(rn1(25,25), "a tainted nude girl", TRUE, SICK_VOMITABLE);

		break;

	    case PM_STD_CONTRACTOR:

		pline("Ulch - who knows how many STDs you just contracted... anyway, the std contractor was tainted!");
	      make_sick(rn1(25,25), "a tainted std contractor", TRUE, SICK_VOMITABLE);

		break;

	    case PM_FIREBALL_LASS:

		pline("Ulch - the stupid girl was tainted!");
	      make_sick(rn1(25,25), "a tainted lass", TRUE, SICK_VOMITABLE);

		break;

	    case PM_OLOG_HAI_GORGON:
	    case PM_BEAR_TROVE:
	    case PM_SPINACH:
	    case PM_COCKERICH:
	    case PM_GRAVITY_CHICKEN:
	    case PM_MEDUSA_S_PET_FISH:
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;
		gainstr((struct obj *)0, 0);
		pline(FunnyHallu ? "You feel like ripping out some trees!" : "You feel stronger!");
		break;

	    case PM_KATOISEFUL:
	    case PM_MURDATRICE:
		pline("You'll have incessant flatulence for a while now...");
		FemaleTrapMaurah += rnz(2000);
		CrapEffect += rnz(50 * (monster_difficulty() + 1));
		break;

	    case PM_STEALATRICE:
	    case PM_LICORICE:
	    case PM_CONTACT_BEAST:
		if (u.contamination && u.contamination < 1000) {
			decontaminate(100);
		}

		break;

	    case PM_MINTY_COCKATRICE:

		if (!Antimagic) You_feel("more resistant to magic!");
		incr_itimeout(&HAntimagic, rnz(2000));
		break;

	    case PM_CHOOKATRICE:
	    case PM_ROCKATRICE:

		switch (rnd(16)) {

			case 1:	/* blueberry (cure blind) */
				/* They're good for your eyes, too. [Sakusha]*/
				pline("It was blueberry-flavored!");
				make_blinded((long)u.ucreamed,TRUE);
				break;
			case 2:	/* bitter (cure stun) */
				pline("It tasted bitter!");
				make_stunned(0L,TRUE);
				break;
			case 3:	/* wasabi (cure confuse) */
				pline("It contained all-natural wasabi extract!");
				make_confused(0L,TRUE);
				break;
			case 4:	/* mattya (cure slow and totter) */
				pline("There was some weird japanese food extras in there...");
				make_frozen(0L, TRUE);
				u.totter = 0;
				break;
			case 5:	/* ginger (cure hallucinate) */
				pline("Ginger! Who puts that in a chocolate???");
				(void) make_hallucinated(0L, TRUE, 0L);
				break;
			default: break;

		}
		break;

	    case PM_CUCKATRICE:
	    case PM_COCKAHORSE:
	    case PM_CUP_COCKATRICE:

		change_sex(); /* ignores unchanging (intentional) */
		You("are suddenly very %s!", flags.female ? "feminine" : "masculine");
		flags.botl = 1;
		break;

	    case PM_NEXUS_CHICKEN:

		pline("This corpse can grant both teleportitis and teleport control. It can also petrify you, though.");
		break;

	    case PM_ELONA__:

		pline("You dare to eat @!");
		break;

	    case PM_STONE_BACULATHRIUM:

		pline("This monster is a funny misspelling of 'baluchitherium' and can turn you to stone.");
		break;

		/* non-mind flayers that emit mental blasts sometimes give INT boosts, too --Amy */
	    case PM_ONE_EYED_ERROR:
	    case PM_ONE_EYED_BUGGER:
	    case PM_DIMMER:
	    case PM_ODDITY:
	    case PM_CURSED_BLOB:
	    case PM_GREATER_DIMMER:
	    case PM_TERRIBLE_ORB:
	    case PM_PSYCH_ORB:
	    case PM_KURROE:
	    case PM_MIND_IMP:
	    case PM_MASTER_BLASTER:
	    case PM_SANITY_BLASTER:
	    case PM_NECROMORB:
	    case PM_HAMMER_ORB:
	    case PM_STUPID_ORB:
	    case PM_EVIL_ORB:
	    case PM_NERF_ORB:
	    case PM_ABRA:
	    case PM_KADABRA:
	    case PM_ALAKAZAM:
	    case PM_SMART_TROVE:
	    case PM_FLYING_COCKATRICE:
	    case PM_CURSING_ORB: {
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;

		if (ABASE(A_INT) < ATTRMAX(A_INT)) {
			if (!rn2(5)) {
				pline(FunnyHallu ? "Hmm, is that what human brain tastes like?" : "Yum! That was real brain food!");
				(void) adjattrib(A_INT, 1, FALSE, TRUE);
				break;	/* don't give them telepathy, too */
			}
		}
		else {
			pline(FunnyHallu ? "Eek, that tasted like rotten oversalted seaweed!" : "For some reason, that tasted bland.");
		}
		}
		break;

		/* WAC all mind flayers as per mondata.h have to be here */
	    case PM_HUMAN_WEREMINDFLAYER:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) {
			u.ulycn = PM_WEREMINDFLAYER;
			u.cnd_lycanthropecount++;
		}
		/* fall through */

	    case PM_INTELLIGENT_TROVE:
	    case PM_NEOTHELID:
	    case PM_VILLITHID:
	    case PM_ILLITHILICH:
	    case PM_FEMIFLAYER:
	    case PM_ULITHARID:
	    case PM_BRAIN_SLUG:
	    case PM_ABERRATION_FLAYER:
	    case PM_MASTER_ABERRATION_FLAYER:
	    case PM_MASTER_MIND_FLAYER:
	    case PM_MINDFLAYER_MIMIC:
	    case PM_MINDFLAYER_PERMAMIMIC:
	    case PM_OOGABOOGAGOBILITGOOK_SEEKER_AREHETYPE_FUCKING_RETARD_ASS_SHIT_FLINGING_MONKEY_MONSTER:
	    case PM_YITHIAN_MIND_FLAYER:
	    case PM_VORPAL_MIND_FLAYER:
	    case PM_MEAN_FLAYER:
	    case PM_BRIGHT_MIND_FLAYER:
	    case PM_BRIGHT_MASTER_MIND_FLAYER:
	    case PM_MISTRESS_MIND_FLAYER:
	    case PM_FOOCUBUS_MIND_FLAYER:
	    case PM_HUGE_MIND_FLAYER:
	    case PM_OWNER_MIND_FLAYER:
	    case PM_MINOR_MIND_FLAYER:
	    case PM_LOW_MIND_FLAYER:
	    case PM_BWARHAR_FLAYER:
	    case PM_LARGE_MIND_FLAYER:
	    case PM_EVIL_MIND_FLAYER:
	    case PM_TELEMINDFLAYER:
	    case PM_GIANT_MIND_FLAYER:
	    case PM_PEW_PEW_MIND_FLAYER:
	    case PM_ARCH_MIND_FLAYER:
	    case PM_ARMY_MIND_FLAYER:
	    case PM_UNDEAD_MIND_FLAYER:
	    case PM_ELDRITCH_MIND_FLAYER:
	    case PM_BLANK_MIND_FLAYER:
	    case PM_SOPHISTICATED_MIND_FLAYER:
	    case PM_SWAPPED_MASTER_MIND_FLAYER:
	    case PM_GIGANTIC_MIND_FLAYER:
	    case PM_GRANDMASTER_MIND_FLAYER:
	    case PM_NASTY_MASTER_MIND_FLAYER:
	    case PM_UNSEXY_MASTER_MIND_FLAYER:
	    case PM_ILLITHID:
	    case PM_MIND_GIVER:
	    case PM_FLAYMIND_CAT:
	    case PM_DEADLY_GAZER:
	    case PM_MIND_BEAMER:
	    case PM_MINT_FLAYER:
	    case PM_UBER_MIND_FLAYER:
	    case PM_SNARE_FLAYER:
	    case PM_SANITY_FLAYER:
	    case PM_FISH_STICK:
	    case PM_OMEGA___FISH_STICK:
	    case PM_BEAMBEAM_FLAYER:
		case PM_COCKATRICE:
		case PM_DISENTITRICE:
		case PM_PETTY_COCKATRICE:
		case PM_BASILISK:
		case PM_PETTY_MIND_FLAYER:
		case PM_SENIOR_MIND_FLAYER:
		case PM_ICKY_FACE:
		case PM_BRAIN_EATER:
		case PM_BRAIN_OOZE:
		case PM_BRAIN_MOLE:
		case PM_MINDFLAYER_AMOEBA:
		case PM_MASTER_MINDFLAYER_AMOEBA:
		case PM_MINDFLAYER_AMOEBA_SWARM:
		case PM_FLOATING_BRAIN:
		case PM_MINDWITNESS:
		case PM_BRAIN_GOLEM:
		case PM_LOWER_BRAIN:
		case PM_MUMAK_FLAYER:
		case PM_IDIOT_BRAIN:
		case PM_ELDER_BRAIN:
		case PM_CHIMERIC_XAN_FLAYER:
		case PM_CHIMERIC_EKS_FLAYER:
		case PM_COILING_BRAWN:
		case PM_FUNGAL_BRAIN:
		case PM_PRESERVED_BRAIN:
		case PM_MASTER_BRAIN:
		case PM_PETTY_BRAIN_GOLEM:
		case PM_LARGE_BRAIN_EATER:
		case PM_PESKY_BRAIN_EATER:
		case PM_MIND_SUCKER:
		case PM_LARGE_MIND_SUCKER:
		case PM_PESKY_MIND_SUCKER:
		case PM_KERRISIN:
		case PM_GIANT_COCKATFLAYER_EELLICH:
		case PM_REGENERATING_MIND_FLAYER:
		case PM_PSYCHO_MIND_FLAYER:
		case PM_PSYCH_FLAYER:
		case PM_MINDGAYER:
		case PM_CLEAR_FLAYER:
		case PM_CHIEF_MIND_FLAYER:
		case PM_PASSIVE_MIND_FLAYER:
		case PM_MINDFLAY_WRAITH:
		case PM_TROLL_FLAYER:
		case PM_POSION_FLAYER:
		case PM_PARASITIC_MIND_FLAYER:
		case PM_SHIELDED_MIND_FLAYER:
		case PM_PARASITIC_MASTER_MIND_FLAYER:
		case PM_BLUE_FLAYER:
		case PM_TROUBLE_MINDFLAYER:
		case PM_MASTER_BLUE_FLAYER:
		case PM_MIND_FLAYER_TELEPATH:
		case PM_TENTACLED_ONE:
		case PM_TENTACLED_POTATO:
		case PM_ELDER_TENTACLED_ONE:
		case PM_ELDER_TENTACLED_POTATO:
		case PM_TENTACLED_ONE_LICH:
		case PM_DEATH_FLAYER:
		case PM_MIND_FLAYER_LARVA:
		case PM_EVIL_MIND_FLAYER_LARVA:
		case PM_VAMPIRIC_MIND_FLAYER:
		case PM_HAPPY_MEAL:
		case PM_CANCEROUS_MIND_FLAYER:
		case PM_JUST_STAND_THERE_MIND_FLAYER:
	    case PM_MIND_FLAYER: {
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;

		if (ABASE(A_INT) < ATTRMAX(A_INT)) {
			if (!rn2(2)) {
				pline(FunnyHallu ? "Hmm, is that what human brain tastes like?" : "Yum! That was real brain food!");
				(void) adjattrib(A_INT, 1, FALSE, TRUE);
				break;	/* don't give them telepathy, too */
			}
		}
		else {
			pline(FunnyHallu ? "Eek, that tasted like rotten oversalted seaweed!" : "For some reason, that tasted bland.");
		}
		}
		/* fall through to default case */
	    } /* case */

		/* just let the default case be valid even if you ate some special corpse. --Amy */

	    /*default: {*/
		register struct permonst *ptr = &mons[pm];
		int i, count;

		if (dmgtype(ptr, AD_STUN) || dmgtype(ptr, AD_HALU) ||
		    pm == PM_VIOLET_FUNGUS || pm == PM_VIOLET_SPORE || pm == PM_VIOLET_STALK || pm == PM_VIOLET_COLONY) {
			pline ("Oh wow!  Great stuff!");
			make_hallucinated(HHallucination + rnz(200),FALSE,0L);
		}
		if(is_giant(ptr) && !rn2(4) && !(u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) ) {gainstr((struct obj *)0, 0); pline(FunnyHallu ? "You feel like ripping out some trees!" : "You feel stronger!"); }

		/* Check the monster for all of the intrinsics.  If this
		 * monster can give more than one, pick one to try to give
		 * from among all it can give.
		 *
		 * If a monster can give 4 intrinsics then you have
		 * a 1/1 * 1/2 * 2/3 * 3/4 = 1/4 chance of getting the first,
		 * a 1/2 * 2/3 * 3/4 = 1/4 chance of getting the second,
		 * a 1/3 * 3/4 = 1/4 chance of getting the third,
		 * and a 1/4 chance of getting the fourth.
		 *
		 * And now a proof by induction:
		 * it works for 1 intrinsic (1 in 1 of getting it)
		 * for 2 you have a 1 in 2 chance of getting the second,
		 *	otherwise you keep the first
		 * for 3 you have a 1 in 3 chance of getting the third,
		 *	otherwise you keep the first or the second
		 * for n+1 you have a 1 in n+1 chance of getting the (n+1)st,
		 *	otherwise you keep the previous one.
		 * Elliott Kleinrock, October 5, 1990
		 */

	/* centaurs increase DEX occasionally --Amy */

		if (ptr->mlet == S_CENTAUR) {
		 if (ABASE(A_DEX) < ATTRMAX(A_DEX)) {
			if (!rn2(10) && !(u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) ) {
				You_feel("nimbler!");
				(void) adjattrib(A_DEX, 1, FALSE, TRUE);
			}
		  }
		}

	/* high-level dragons increase CON occasionally --Amy */

		if (ptr->mlet == S_DRAGON && ptr->mlevel >= 18) {
		 if (ABASE(A_CON) < ATTRMAX(A_CON)) {
			if (!rn2(10) && !(u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) ) {
				You_feel("hardier!");
				(void) adjattrib(A_CON, 1, FALSE, TRUE);
			}
		  }
		}

	/* very rarely, eating spellcasters increases WIS --Amy */

		if (dmgtype(ptr, AD_SPEL) || dmgtype(ptr, AD_CLRC) || dmgtype(ptr, AD_CAST) ) {
		 if (ABASE(A_WIS) < ATTRMAX(A_WIS)) {
			if (!rn2(25) && !(u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) ) {
				You_feel("wiser!");
				(void) adjattrib(A_WIS, 1, FALSE, TRUE);
			}
		  }
		}

	/* Charisma is next to impossible to raise, so eating nymphs will help now. --Amy */

		if (ptr->mlet == S_NYMPH) {
		 if (ABASE(A_CHA) < ATTRMAX(A_CHA)) {
			if (!rn2(10) && !(u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) ) {
				You_feel("more %s!", flags.female ? "pretty" : "attractive");
				(void) adjattrib(A_CHA, 1, FALSE, TRUE);
			}
		  }
		}

	/* Or eating item-stealers. --Amy */

		if (dmgtype(ptr, AD_SITM) || dmgtype(ptr, AD_SEDU) || dmgtype(ptr, AD_SSEX) ) {
		 if (ABASE(A_CHA) < ATTRMAX(A_CHA)) {
			if (!rn2(dmgtype(ptr, AD_SSEX) ? 3 : 10)  && !(u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) ) {
				You_feel("more %s!", flags.female ? "pretty" : "attractive");
				(void) adjattrib(A_CHA, 1, FALSE, TRUE);
			}
		  }
		}

	/* luck is also harder to get; eating luck-reducing monsters sometimes grants a boost --Amy */
		if (dmgtype(ptr, AD_LUCK) && (ptr->mlevel > rn2(Race_if(PM_ILLITHID) ? 105 : 35) && rn2(4) && !(u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() )  ) ) {
			change_luck(1);
			You_feel("lucky.");
		}

	/* or rarely, random attack monsters --Amy */
		if (dmgtype(ptr, AD_RBRE) && (ptr->mlevel > rn2(Race_if(PM_ILLITHID) ? 525 : 175) && rn2(4) && !(u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() )  ) ) {
			change_luck(1);
			You_feel("lucky.");
		}

	/* and since mind flayers are so rare, improve INT gain a bit --Amy */
		if (dmgtype(ptr, AD_DRIN) && (ptr->mlevel > rn2(Race_if(PM_ILLITHID) ? 105 : 35) && rn2(4) && !(u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() )  ) ) {
		 if (ABASE(A_INT) < ATTRMAX(A_INT)) {
			You_feel("smarter!");
			(void) adjattrib(A_INT, 1, 2, TRUE);
		  }
		}

	/* psi-based enemies grant INT too --Amy */
		if (dmgtype(ptr, AD_SPC2) && (ptr->mlevel > rn2(Race_if(PM_ILLITHID) ? 105 : 35) && rn2(4) && !(u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() )  ) ) {
		 if (ABASE(A_INT) < ATTRMAX(A_INT)) {
			You_feel("smarter!");
			(void) adjattrib(A_INT, 1, 2, TRUE);
		  }
		}

	/* insanity monsters should not be eaten... --Amy */
		if (dmgtype(ptr, AD_INSA)) {
			pline("It tasted really, really strange.");
			increasesanity(rnz(100));
		}
		if (dmgtype(ptr, AD_SANI)) {
			pline("It tasted really, really strange.");
			increasesanity(rnz(100));
		}

	/* tech drain monsters have a small chance of increasing the level of a random tech --Amy */
		if (dmgtype(ptr, AD_TDRA) && (ptr->mlevel > rn2(Race_if(PM_ILLITHID) ? 210 : 70) && rn2(2) && !(u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() )  ) ) {
			You_feel("very good!");
			techlevelup();
		}

	/* skill cap reducing monsters very rarely grant something good too --Amy */
		if (dmgtype(ptr, AD_SKIL) && (ptr->mlevel > rn2(Race_if(PM_ILLITHID) ? 210 : 70) && rn2(2) && !(u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() )  ) ) {

			You_feel("the RNG's touch...");

			int skillimprove = randomgoodskill();

			if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
				unrestrict_weapon_skill(skillimprove);
				pline("You can now learn the %s skill.", wpskillname(skillimprove));
			} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
				unrestrict_weapon_skill(skillimprove);
				P_MAX_SKILL(skillimprove) = P_BASIC;
				pline("You can now learn the %s skill.", wpskillname(skillimprove));
			} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
				P_MAX_SKILL(skillimprove) = P_SKILLED;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
				P_MAX_SKILL(skillimprove) = P_EXPERT;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
				P_MAX_SKILL(skillimprove) = P_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
				P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else if (!rn2(200) && P_MAX_SKILL(skillimprove) == P_GRAND_MASTER) {
				P_MAX_SKILL(skillimprove) = P_SUPREME_MASTER;
				pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
			} else gainlevelmaybe();

			if (Race_if(PM_RUSMOT)) {
				if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
					unrestrict_weapon_skill(skillimprove);
					pline("You can now learn the %s skill.", wpskillname(skillimprove));
				} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
					unrestrict_weapon_skill(skillimprove);
					P_MAX_SKILL(skillimprove) = P_BASIC;
					pline("You can now learn the %s skill.", wpskillname(skillimprove));
				} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
					P_MAX_SKILL(skillimprove) = P_SKILLED;
					pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
				} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
					P_MAX_SKILL(skillimprove) = P_EXPERT;
					pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
				} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
					P_MAX_SKILL(skillimprove) = P_MASTER;
					pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
				} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
					P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
					pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
				} else if (!rn2(200) && P_MAX_SKILL(skillimprove) == P_GRAND_MASTER) {
					P_MAX_SKILL(skillimprove) = P_SUPREME_MASTER;
					pline("Your knowledge of the %s skill increases.", wpskillname(skillimprove));
				}
			}

			gainlevelmaybe();

		}

	/* eating anything that spouts fake messages will give another one --Amy */
		if (dmgtype(ptr, AD_FAKE)) {
			pline("%s", fauxmessage());
			u.cnd_plineamount++;
			if (!rn2(3)) {
				pline("%s", fauxmessage());
				u.cnd_plineamount++;
			}
		}

	/* AD_HEAL monsters are rare; let's make them give one extra max HP --Amy */
		if (dmgtype(ptr, AD_HEAL) && !(u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) ) {
			if (Upolyd) {
				u.mh++;
				u.mhmax++;
				if (u.mh > u.mhmax) u.mh = u.mhmax;
			} else {
				u.uhp++;
				u.uhpmax++;
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			}
			if (uactivesymbiosis) {
				u.usymbiote.mhpmax++;
				if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
			}
		    You_feel("vitalized.");
		    flags.botl = 1;
		}

	/* And since I'm nice, do a similar thing for mana --Amy */
		if (dmgtype(ptr, AD_MANA) && !(u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) ) {
			u.uenmax++;
			You_feel("a mild buzz.");
		    flags.botl = 1;
		}

	/* Dream eaters, on the other hand, are bad to eat. --Amy */
		if (dmgtype(ptr, AD_DREA) && !multi) {
		    pline("Suddenly you have a nightmare!");
		    nomul(-5, "scared by a nightmare", TRUE);
		    nomovemsg = 0;
		}

	/* AD_NIVE can reduce high HP/Pw maximums... but eating them can fix low maximums --Amy */
		if (dmgtype(ptr, AD_NIVE)) {
			pline("Wow, a corona antidote!");
			upnivel(FALSE);
		}

		if (is_bat(ptr)) {
			make_stunned(HStun + 30,FALSE);
		}

		if (evilfriday && dmgtype(ptr, AD_ENCH)) {
			pline("Harharhar...");
			attrcurse();
			pline("Welcome to the evil variant, bitch!");
		}
		if (evilfriday && dmgtype(ptr, AD_NGEN)) {
			pline("Harharhar...");
			attrcurse();
			attrcurse();
			attrcurse();
			attrcurse();
			attrcurse();
			pline("Welcome to the evil variant, bitch!");
		}

		if (dmgtype(ptr, AD_AGGR)) {
			int aggroamount = rnd(6);
			if (isfriday) aggroamount *= 2;
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
			while (aggroamount) {
				u.cnd_aggravateamount++;
				makemon((struct permonst *)0, u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
				aggroamount--;
				if (aggroamount < 0) aggroamount = 0;
			}
			u.aggravation = 0;
			pline("Several monsters come out of a portal.");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

		}

		/* Eating slimy or oily corpses makes your fingers slippery in unnethack because harharhar harhar har. */
		if (evilfriday && (amorphous(&mons[pm]) || slithy(&mons[pm]) || mons[pm].mlet == S_BLOB || mons[pm].mlet == S_JELLY || mons[pm].mlet == S_FUNGUS || mons[pm].mlet == S_WORM || mons[pm].mlet == S_PUDDING)) {
			pline("Harharhar harhar har, you ate the wrong thing and therefore your %s are slippery now.", makeplural(body_part(HAND)));
			incr_itimeout(&Glib, rnd(15 + level_difficulty()));

		}

	/* HC aliens carry evil diseases. Do not eat them! --Amy */
		if (ptr->msound == MS_HCALIEN) {
			pline("Yeech - that must have been infectious!");
			make_sick(rn1(25,25), "tasting infectious hc aliens", TRUE, SICK_VOMITABLE);
			badeffect();
		}

		if (dmgtype(ptr, AD_EDGE) && !(u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) ) {
			int edgeeffect;
			if (touch_petrifies(ptr)) {
				edgeeffect = rnd(13); /* petrifying corpses are dangerous - only give positive effects --Amy */
			} else {
				edgeeffect = rnd(21); /* others are safe - give either positive or negative effects --Amy */
			}
			switch (rnd(edgeeffect)) {

			case 1:
				if (ABASE(A_CHA) < ATTRMAX(A_CHA)) {
					if (!rn2(10)) {
						You_feel("more %s!", flags.female ? "pretty" : "attractive");
						(void) adjattrib(A_CHA, 1, FALSE, TRUE);
						break;
					}
				}
			break;
			case 2:
				if (ABASE(A_CON) < ATTRMAX(A_CON)) {
					You_feel("tougher!");
					(void) adjattrib(A_CON, 1, FALSE, TRUE);
					}
			break;
			case 3:
				if (issoviet && rn2(3)) {
					pline("Sovetskaya ne khochet, chtoby vy, chtoby poluchit' magicheskuyu silu! Vasha zhizn' otstoy!");
					break;
				}

				if (rn2(3)/* || 3 * u.uen <= 2 * u.uenmax*/) {
				    int old_uen = u.uen; /* Some slight changes to this code. --Amy */
				    u.uen += rnd(3);
					if (!rn2(3)) u.uenmax++;
				    if (u.uen > u.uenmax) u.uen = u.uenmax;
				    if (old_uen != u.uen) {
					    You_feel("a mild buzz.");
					    flags.botl = 1;
				    }
				}
			break;
			case 4:
				if (Upolyd) {
					u.mh++;
					u.mhmax++;
					if (u.mh > u.mhmax) u.mh = u.mhmax;
				} else {
					u.uhp++;
					u.uhpmax++;
					if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				}
				if (uactivesymbiosis) {
					u.usymbiote.mhpmax++;
					if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
				}
				You_feel("vitalized.");
				flags.botl = 1;
			break;
			case 5:
				You_feel("clairvoyant!");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Vy znayete raspolozheniye, no ne lovushki, a te vse ravno budut poshel na khuy vverkh." : "Wschiiiiiiie!");
				incr_itimeout(&HClairvoyant, rnd(500));
			break;
			case 6:
			      You_feel("that was a smart thing to do.");
				gainlevelmaybe();
			break;
			case 7:
				if (Upolyd) u.mh = u.mhmax;
				else u.uhp = u.uhpmax;
				flags.botl = 1;
			break;
			case 8:
				if (!(HInvis & INTRINSIC)) You_feel("hidden!");
				HInvis |= FROMOUTSIDE;
				HSee_invisible |= FROMOUTSIDE;
			break;
			case 9:
				gainstr((struct obj *)0, 0);
				pline(FunnyHallu ? "You feel like ripping out some trees!" : "You feel stronger!");
				break;
			break;
			case 10:
				if (ABASE(A_INT) < ATTRMAX(A_INT)) {
					if (!rn2(2)) {
						pline(FunnyHallu ? "Hmm, is that what human brain tastes like?" : "Yum! That was real brain food!");
						(void) adjattrib(A_INT, 1, FALSE, TRUE);
						break;
					}
				}
				else {
					pline(FunnyHallu ? "Eek, that tasted like rotten oversalted seaweed!" : "For some reason, that tasted bland.");
				}
			break;
			case 11:
				if (u.contamination && u.contamination < 1000) {
				decontaminate(100);
				}
			break;
			case 12:
				if (!Very_fast)
					You("are suddenly moving %sfaster.", Fast ? "" : "much ");
				else {
					Your("%s get new energy.", makeplural(body_part(LEG)));
				}
				incr_itimeout(&HFast, rn1(1000, 1000));
			break;

			case 13:
				You_feel("ethereal.");
				incr_itimeout(&HPasses_walls, rn1(10, 50));
			break;
			case 14:
				make_stunned(HStun + 30,FALSE);
			break;
			case 15:
				if(!(HPolymorph & FROMOUTSIDE)) {
				You_feel(FunnyHallu ? "able to take on different shapes! Yeah! Let's transform into something fun!" : "unstable.");
					HPolymorph  |= FROMOUTSIDE;
				}
			break;
			case 16:
				if (!Unchanging) {
				    You_feel("a change coming over you.");
				    polyself(FALSE);
				}
			break;
			case 17:
				pline("Ulch - the edge was tainted!");
			      make_sick(rn1(25,25), "a tainted stone edge", TRUE, SICK_VOMITABLE);
			break;
			case 18:
				pline ("Oh wow!  Great stuff!");
				make_hallucinated(HHallucination + rnz(200),FALSE,0L);
			break;
			case 19:
				pushplayer(TRUE);
			break;
			case 20:
				You_feel("that was a bad idea.");
				losexp("eating an edgy corpse", FALSE, TRUE);
			break;
			case 21:
				Your("velocity suddenly seems very uncertain!");
				if (HFast & INTRINSIC) {
					HFast &= ~INTRINSIC;
					You("seem slower.");
				} else {
					HFast |= FROMOUTSIDE;
					You("seem faster.");
				}
			break;

			}
		}

	/* If you really think maprot monsters are good to eat... --Amy */
		if (dmgtype(ptr, AD_AMNE)) {
		    pline("The taste reminds you of Maud... oh wait, how come you can't remember anything?!");
		    maprot();
		    if (!rn2(3)) forget(1 + rn2(5));
		}

	/* Nastiness monsters are definitely not good to eat either. --Amy */
		if (dmgtype(ptr, AD_NAST) ) {
			if (Upolyd) {
				u.mhmax--;
			}
			u.uhpmax--;
			losehp(10, "eating a nasty corpse", KILLED_BY);
		    pline("Your health was damaged!");
		    flags.botl = 1;
		}

	/* Eating bad effect monsters causes another bad effect --Amy */
		if (dmgtype(ptr, AD_BADE) ) {
			badeffect();
		}
		if (dmgtype(ptr, AD_RBAD) ) {
			reallybadeffect();
		}

	/* Eating contaminating monsters increases your contamination --Amy */
		if (dmgtype(ptr, AD_CONT) ) {
			pline("The food was contaminated! Your contamination increases.");
			contaminate(rnz((level_difficulty() + 40) * 5), TRUE);
		}

	/* Mimics will always cause you to mimic something --Amy */

		if (standardmimic(ptr) || permamimic(ptr) || (ptr->mlet == S_MIMIC)) {
			if (youmonst.data->mlet != S_MIMIC && !Unchanging) {
			    char buf[BUFSZ];
			    You_cant("resist the temptation to mimic %s.",
				FunnyHallu ? "an orange" : "a pile of gold");
	                    /* A pile of gold can't ride. */
			    if (u.usteed) dismount_steed(DISMOUNT_FELL);
			    nomul(-(rnd(60)), "pretending to be a pile of gold", TRUE);
			    sprintf(buf, FunnyHallu ?
				"You suddenly dread being peeled and mimic %s again!" :
				"You now prefer mimicking %s again.",
				an( (Upolyd && !missingnoprotect) ? youmonst.data->mname : urace.noun));
			    eatmbuf = strcpy((char *) alloc(strlen(buf) + 1), buf);
			    nomovemsg = eatmbuf;
			    afternmv = eatmdone;
			    /* ??? what if this was set before? */
			    youmonst.m_ap_type = M_AP_OBJECT;
			    youmonst.mappearance = FunnyHallu ? ORANGE : GOLD_PIECE;
			    newsym(u.ux,u.uy);
			    curs_on_u();
			    /* make gold symbol show up now */
			    display_nhwindow(WIN_MAP, TRUE);
			}
		}

		 count = 0;	/* number of possible intrinsics */
		 tmp = 0;	/* which one we will try to give */
		 for (i = 1; i <= LAST_PROP; i++) {
			if (intrinsic_possible(i, ptr)) {
				count++;
				/* a 1 in count chance of replacing the old
				 * one with this one, and a count-1 in count
				 * chance of keeping the old one.  (note
				 * that 1 in 1 and 0 in 1 are what we want
				 * for the first one
				 */
				if (!rn2(count)) {
#ifdef DEBUG
					debugpline("Intrinsic %d replacing %d",
								i, tmp);
#endif
					tmp = i;
				}
			}
		 }

		 /* if any found try to give them one */
		 if (count) givit(tmp, ptr);
	    /*}
	    break;*/
	/*}*/

	if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC) &&
		catch_lycanthropy && defends(AD_WERE, uwep)) {
	    if (!touch_artifact(uwep, &youmonst)) {
		dropx(uwep);
		uwepgone();
	    }
	}

	return;
}

void
gluttonous()
{
	/* only happens if you were satiated, extra check by Amy to make that conduct mean more */
	if ((u.uhs == SATIATED) && ((Role_if(PM_KNIGHT) && u.ualign.type == A_LAWFUL) || (uwep && uwep->otyp == HONOR_KATANA) || (u.twoweap && uswapwep && uswapwep->otyp == HONOR_KATANA) || Role_if(PM_CHEVALIER) || Race_if(PM_VEELA) || Role_if(PM_PALADIN) || have_anorexia() || Role_if(PM_FAILED_EXISTENCE)) ) {
			adjalign(-3);		/* gluttony is unchivalrous */
		You(FunnyHallu ? "feel that your belly's gonna burst!" : "feel like a glutton!");
	}

	if ((u.uhs == SATIATED) && (Race_if(PM_VEELA) || Role_if(PM_FAILED_EXISTENCE)) ) {	badeffect();
	}

	if (u.uhs == SATIATED && have_anorexia() ) { /* They aren't used to eat much. --Amy */

	if(!rn2(4)) {
		if (FunnyHallu) You_feel("rather trippy.");
		else You_feel("rather %s.", body_part(LIGHT_HEADED));
		make_confused(HConfusion + d(2,4),FALSE);
	} else if(!rn2(5)) {
		if (FunnyHallu) You_feel("uncontrollable."); /* this and some other effects added by Amy */
		else You_feel("stunned.");
		make_stunned(HStun + d(2,4),FALSE);
	} else if(!rn2(5)) {
		if (FunnyHallu) You_feel("even weirder!");
		else You_feel("weirded out!");
		make_hallucinated(HHallucination + d(2,10),FALSE,0L);
	} else if(!rn2(4)) {
		if (!Blind) pline("Everything suddenly goes dark.");
		make_blinded(Blinded+d(2,10),FALSE);
		if (!Blind) Your("%s", vision_clears);
	} else if(!rn2(8)) {
		if (FunnyHallu) You_feel("like your body is trying to fall asleep!");
		else You_feel("numb.");
		make_numbed(HNumbed + d(2,15),FALSE);
	} else if(!rn2(10)) {
		if (FunnyHallu) pline("Crap! A %s suddenly appeared! RUN!",rndmonnam() );
		else You_feel("fear spreading through your body.");
		make_feared(HFeared + d(2,15),FALSE);
	} else if(!rn2(10)) {
		if (FunnyHallu) You_feel("like your on fire despite there being no heat!");
		else You_feel("like you're on fire!");
		make_burned(HBurned + d(2,15),FALSE);
	} else if(!rn2(20)) {
		if (FunnyHallu) You_feel("a giant ice cream cone enclosing you!");
		else pline("You're getting the chills.");
		make_frozen(HFrozen + d(2,15),FALSE);
	} else if(!rn2(20)) {
		if (FunnyHallu) You_feel("that your wife is gonna break up with you!");
		else pline("You're dimmed.");
		make_dimmed(HDimmed + d(2,15),FALSE);
	} else if(!rn2(10)) {
		if (FunnyHallu) You_feel("like you have dementia tremor!"); /* not a real name --Amy */
		else pline("Your hands start trembling violently!");
		incr_itimeout(&Glib, d(2,4) );
	} else if(!rn2(10)) {
		if (FunnyHallu) You_feel("totally down! Seems you tried some illegal shit!");
		else You_feel("like you're going to throw up.");
	      make_vomiting(Vomiting+20, TRUE);
		if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */
	} else if(!rn2(3)) {
		const char *what, *where;
		if (!Blind)
		    what = "goes",  where = "dark";
		else if (Levitation || Is_airlevel(&u.uz) ||
			 Is_waterlevel(&u.uz))
		    what = "you lose control of",  where = "yourself";
		else
		    what = "you slap against the", where = (u.usteed) ? "saddle" : surface(u.ux,u.uy);
		pline_The("world spins and %s %s.", what, where);
		flags.soundok = 0;
		nomul(-rnd(10), "unconscious from forgetting your anorexia conduct", TRUE);
		nomovemsg = "You are conscious again.";
		afternmv = Hear_again;
		return;
	} else if(!rn2(50) && !Sick) { /* The chance of this outcome !MUST! be low. Everything else would be unfair. --Amy */
	    make_sick(rn1(25,25), "rotten food", TRUE, SICK_VOMITABLE);
	} else if(!rn2(200) && !Slimed && !issoviet && !flaming(youmonst.data) && !Unchanging && !slime_on_touch(youmonst.data) ) { /* This chance should be even lower. --Amy */
			You("don't feel very well.");
			make_slimed(100);
		    killer_format = KILLED_BY_AN;
		    delayed_killer = "slimed by a slimy meal";
	}

	}



}

void
violated_vegetarian()
{
    u.uconduct.unvegetarian++;
    if (Role_if(PM_MONK) || Role_if(PM_HALF_BAKED) || have_anorexia() || Role_if(PM_FAILED_EXISTENCE) || Race_if(PM_SYLPH) ) {
	You_feel("guilty.");
	adjalign(-5);
    }
	if (Role_if(PM_FAILED_EXISTENCE)) badeffect();

	if (have_anorexia()) { /* Their metabolism isn't used to meat. --Amy */

	if(!rn2(4)) {
		if (FunnyHallu) You_feel("rather trippy.");
		else You_feel("rather %s.", body_part(LIGHT_HEADED));
		make_confused(HConfusion + d(2,4),FALSE);
	} else if(!rn2(5)) {
		if (FunnyHallu) You_feel("uncontrollable."); /* this and some other effects added by Amy */
		else You_feel("stunned.");
		make_stunned(HStun + d(2,4),FALSE);
	} else if(!rn2(5)) {
		if (FunnyHallu) You_feel("even weirder!");
		else You_feel("weirded out!");
		make_hallucinated(HHallucination + d(2,10),FALSE,0L);
	} else if(!rn2(4)) {
		if (!Blind) pline("Everything suddenly goes dark.");
		make_blinded(Blinded+d(2,10),FALSE);
		if (!Blind) Your("%s", vision_clears);
	} else if(!rn2(8)) {
		if (FunnyHallu) You_feel("like your body is trying to fall asleep!");
		else You_feel("numb.");
		make_numbed(HNumbed + d(2,15),FALSE);
	} else if(!rn2(10)) {
		if (FunnyHallu) pline("Crap! A %s suddenly appeared! RUN!",rndmonnam() );
		else You_feel("fear spreading through your body.");
		make_feared(HFeared + d(2,15),FALSE);
	} else if(!rn2(10)) {
		if (FunnyHallu) You_feel("like your on fire despite there being no heat!");
		else You_feel("like you're on fire!");
		make_burned(HBurned + d(2,15),FALSE);
	} else if(!rn2(20)) {
		if (FunnyHallu) You_feel("a giant ice cream cone enclosing you!");
		else pline("You're getting the chills.");
		make_frozen(HFrozen + d(2,15),FALSE);
	} else if(!rn2(20)) {
		if (FunnyHallu) You_feel("that your wife is gonna break up with you!");
		else pline("You're dimmed.");
		make_dimmed(HDimmed + d(2,15),FALSE);
	} else if(!rn2(10)) {
		if (FunnyHallu) You_feel("like you have dementia tremor!"); /* not a real name --Amy */
		else pline("Your hands start trembling violently!");
		incr_itimeout(&Glib, d(2,4) );
	} else if(!rn2(10)) {
		if (FunnyHallu) You_feel("totally down! Seems you tried some illegal shit!");
		else You_feel("like you're going to throw up.");
	      make_vomiting(Vomiting+20, TRUE);
		if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */
	} else if(!rn2(3)) {
		const char *what, *where;
		if (!Blind)
		    what = "goes",  where = "dark";
		else if (Levitation || Is_airlevel(&u.uz) ||
			 Is_waterlevel(&u.uz))
		    what = "you lose control of",  where = "yourself";
		else
		    what = "you slap against the", where = (u.usteed) ? "saddle" : surface(u.ux,u.uy);
		pline_The("world spins and %s %s.", what, where);
		flags.soundok = 0;
		nomul(-rnd(10), "unconscious from eating meat", TRUE);
		nomovemsg = "You are conscious again.";
		afternmv = Hear_again;
		return;
	} else if(!rn2(50) && !Sick) { /* The chance of this outcome !MUST! be low. Everything else would be unfair. --Amy */
	    make_sick(rn1(25,25), "rotten food", TRUE, SICK_VOMITABLE);
	} else if(!rn2(200) && !Slimed && !issoviet && !flaming(youmonst.data) && !Unchanging && !slime_on_touch(youmonst.data) ) { /* This chance should be even lower. --Amy */
			You("don't feel very well.");
			make_slimed(100);
		    killer_format = KILLED_BY_AN;
		    delayed_killer = "slimed by a slimy meal";
	}

	/* By the way, I'm certainly not a vegetarian myself. It's just a twist of the topmodel role. --Amy */

	}

    return;
}

/* common code to check and possibly charge for 1 context.tin.tin,
 * will split() context.tin.tin if necessary */
STATIC_PTR
void
costly_tin(verb)
	const char* verb;		/* if 0, the verb is "open" */
{
	if(((!carried(tin.tin) &&
	     costly_spot(tin.tin->ox, tin.tin->oy) &&
	     !tin.tin->no_charge)
	    || tin.tin->unpaid)) {
	    verbalize("You %s it, you bought it!", verb ? verb : "open");
	    if(tin.tin->quan > 1L) tin.tin = splitobj(tin.tin, 1L);
	    bill_dummy_object(tin.tin);
	}
}

STATIC_PTR
int
opentin()		/* called during each move whilst opening a tin */
{
	register int r;
	const char *what;
	int which;
	char buf[BUFSZ];

	if(!carried(tin.tin) && !obj_here(tin.tin, u.ux, u.uy))
					/* perhaps it was stolen? */
		return(0);		/* %% probably we should use tinoid */
	if(tin.usedtime++ >= 50) {
		You(FunnyHallu ? "get bored while playing with that dildo-like thing." : "give up your attempt to open the tin.");
		return(0);
	}
	if(tin.usedtime < tin.reqtime)
		return(1);		/* still busy */
	if(tin.tin->otrapped ||
	   (tin.tin->cursed && tin.tin->spe != -1 && !rn2(8))) {
		b_trapped("tin", 0);
		costly_tin("destroyed");
		goto use_me;
	}
	You(FunnyHallu ? "open it! Now let's see the contents, maybe it's some acid?" : "succeed in opening the tin.");
	if(tin.tin->spe < 1) {
	    if (tin.tin->corpsenm == NON_PM) {
		pline(FunnyHallu ? "Nothing in there, might as well throw it in the trash can." : "It turns out to be empty.");
		tin.tin->dknown = tin.tin->known = TRUE;
		costly_tin((const char*)0);
		goto use_me;
	    }
	    r = tin.tin->cursed ? ROTTEN_TIN :	/* always rotten if cursed */
		(FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone()) ? ROTTEN_TIN :
		    (tin.tin->spe == -1) ? /* HOMEMADE_TIN*/ rn2(TTSZ-1) :  /* player made it */
			rn2(TTSZ-1);		/* else take your pick */
	    if (r == ROTTEN_TIN && (nocorpsedecay(&mons[tin.tin->corpsenm]) ))
		r = HOMEMADE_TIN;		/* lizards don't rot */
	    else if (tin.tin->spe == -1 && !tin.tin->blessed && !rn2(7))
		r = ROTTEN_TIN;			/* some homemade tins go bad */
	    which = 0;	/* 0=>plural, 1=>as-is, 2=>"the" prefix */
	    if (Hallucination) {
		what = rndmonnam();
	    } else {
		what = mons[tin.tin->corpsenm].mname;
		if (mons[tin.tin->corpsenm].geno & G_UNIQ)
		    which = type_is_pname(&mons[tin.tin->corpsenm]) ? 1 : 2;
	    }
	    if (which == 0) what = makeplural(what);
	    /* ALI - you already know the type of the tinned meat */
	    if (tin.tin->known && mvitals[tin.tin->corpsenm].eaten < 255)
		mvitals[tin.tin->corpsenm].eaten++;
	    /* WAC - you only recognize if you've eaten this before */
	    if (!mvitals[tin.tin->corpsenm].eaten && !Hallucination) {
		if (rn2(2))
			pline ("It smells kind of like %s.",
				monexplain[mons[tin.tin->corpsenm].mlet]);
		else 
			pline_The("smell is unfamiliar.");
	    } else
	    pline("It smells like %s%s.", (which == 2) ? "the " : "", what);

	    getlin ("Eat it? [y/yes/no]",buf);
	    (void) lcase (buf);
	    if (strcmp (buf, "yes") && strcmp (buf, "y")) {
	    	/* ALI - you know the tin iff you recognized the contents */
		if (mvitals[tin.tin->corpsenm].eaten)
		if (!Hallucination) tin.tin->dknown = tin.tin->known = TRUE;
		if (flags.verbose) You("discard the open tin.");
		costly_tin((const char*)0);
		goto use_me;
	    }
	    /* in case stop_occupation() was called on previous meal */
	    victual.piece = (struct obj *)0;
	    victual.fullwarn = victual.eating = victual.doreset = FALSE;

	    /* WAC - you only recognize if you've eaten this before */
	    You("consume %s %s.", tintxts[r].txt,
				mvitals[tin.tin->corpsenm].eaten ?
				mons[tin.tin->corpsenm].mname : "food");

	    if (tin.tin && tin.tin->oartifact == ART_YASDORIAN_S_PARTLY_EATEN_T) {
			pline("YEEEEEECH! What the FUCKING HELL was in there???");
		      u.uprops[NASTINESS_EFFECTS].intrinsic |= FROMOUTSIDE;
		      u.uprops[ANTIMAGIC].intrinsic |= FROMOUTSIDE;
			u.uprops[DEAC_SICK_RES].intrinsic += 1000000; /* basically forever */
			u.uprops[DEAC_POISON_RES].intrinsic += 1000000;
	    }

	    /* KMH, conduct */
	    u.uconduct.food++;
		gluttonous();
	    if (!vegan(&mons[tin.tin->corpsenm]))
		u.uconduct.unvegan++;
	    if (!vegetarian(&mons[tin.tin->corpsenm]))
		violated_vegetarian();

		if ( (Race_if(PM_ARMED_COCKATRICE) || Race_if(PM_CORTEX)) && !lithivorous(youmonst.data) && !Upolyd && rn2(2) ) {

			if (rn2(2)) {
				pline("Ulch - that food was petrified! You're unable to swallow it.");
				badeffect();
			} else pline("The food turns to stone as you try to eat it!");

			    costly_tin((const char*)0);
			goto use_me;

		}

		if ( (Race_if(PM_ARMED_COCKATRICE) || Race_if(PM_CORTEX)) && !lithivorous(youmonst.data) && !Upolyd && rn2(2) ) {

			pline("Ulch - that food was petrified!");
			badeffect();

		}

		/* AvzinElkein wants lithivore roles to be able to eat as a cockatrice; I'll not remove the penalties
		 * completely, but they will be imposed a lot less often, and with more meaningful wording. --Amy */

		if ( (Race_if(PM_ARMED_COCKATRICE) || Race_if(PM_CORTEX)) && lithivorous(youmonst.data) && !Upolyd && !rn2(4) ) {

			if (rn2(2)) {
				pline("Ulch - that petrified food had manky bits! You're unable to swallow it.");
				badeffect();
			} else pline("The petrified food appears to be manky as you try to eat it!");

			    costly_tin((const char*)0);
			goto use_me;

		}

		if ( (Race_if(PM_ARMED_COCKATRICE) || Race_if(PM_CORTEX)) && lithivorous(youmonst.data) && !Upolyd && !rn2(4) ) {

			pline("Ulch - that petrified food had manky bits!");
			badeffect();

		}

	    if (mvitals[tin.tin->corpsenm].eaten)
	    tin.tin->dknown = tin.tin->known = TRUE;
	    cprefx(tin.tin->corpsenm); cpostfx(tin.tin->corpsenm);

	    /* charge for one at pre-eating cost */
	    costly_tin((const char*)0);

	    /* check for vomiting added by GAN 01/16/87 */
	    if(tintxts[r].nut < 0) {make_vomiting((long)rn1(15,10), FALSE);
			if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */
			if (!issoviet) lesshungry(tintxts[r].nut);
		}
	    else lesshungry((uarmf && uarmf->oartifact == ART_U_BE_CURRY) ? ((tintxts[r].nut) * 3) : tintxts[r].nut);

		if (Race_if(PM_WORM_THAT_WALKS)) { /* chance to polymorph into the tinned monster --Amy */
			if (rn2(5) ) {

				 if (polyok(&mons[tin.tin->corpsenm])) {
				u.wormpolymorph = tin.tin->corpsenm;
				polyself(FALSE);
				}
			} else polyself(FALSE);
		}

		if (Race_if(PM_WARPER)) { /* chance to polymorph into the tinned monster --Amy */
			if (!rn2(5) ) {

				u.wormpolymorph = tin.tin->corpsenm;
				polyself(FALSE);

			}
		}

	    if(r == 0 || r == FRENCH_FRIED_TIN || r == FRENCH_FRIED_TINX) {
	        /* Assume !Glib, because you can't open tins when Glib. */
		incr_itimeout(&Glib, rnd(15));
		pline("Eating deep fried food made your %s very slippery.",
		      makeplural(body_part(FINGER)));
	    }
	} else if (tin.tin->spe == 1) {
	    if (tin.tin->cursed)
		pline("It contains some decaying%s%s substance.",
			Blind ? "" : " ", Blind ? "" : hcolor(NH_GREEN));
	    else
		pline(FunnyHallu ? "It contains potato tack or something like that." : "It contains spinach.");

	    getlin ("Eat it? [y/yes/no]",buf);
	    (void) lcase (buf);
	    if (strcmp (buf, "yes") && strcmp (buf, "y")) {
		if (!Hallucination && !tin.tin->cursed)
		    tin.tin->dknown = tin.tin->known = TRUE;
		if (flags.verbose)
		    You("discard the open tin.");
		costly_tin((const char*)0);
		goto use_me;
	    }

	    tin.tin->dknown = tin.tin->known = TRUE;
	    costly_tin((const char*)0);

	    if (!tin.tin->cursed)
		pline("This makes you feel like %s!",
		      FunnyHallu ? "Swee'pea" : "Popeye");
	    lesshungry(600);
	    gainstr(tin.tin, 0);
	    u.uconduct.food++;
	} else if (tin.tin->spe == 2) { /* canned beans, idea from /rlg/ on 4chan */
	    if (tin.tin->cursed)
		pline("It contains some decaying%s%s substance.",
			Blind ? "" : " ", Blind ? "" : hcolor(NH_WHITE));
	    else
		pline(FunnyHallu ? "It contains french fries." : "It contains beans.");

	    getlin ("Eat it? [y/yes/no]",buf);
	    (void) lcase (buf);
	    if (strcmp (buf, "yes") && strcmp (buf, "y")) {
		if (!Hallucination && !tin.tin->cursed)
		    tin.tin->dknown = tin.tin->known = TRUE;
		if (flags.verbose)
		    You("discard the open tin.");
		costly_tin((const char*)0);
		goto use_me;
	    }

	    tin.tin->dknown = tin.tin->known = TRUE;
	    costly_tin((const char*)0);

	    if (!tin.tin->cursed) {
		if (!rn2(3)) (void) adjattrib(A_STR, 1, FALSE, TRUE);
		if (!rn2(3)) (void) adjattrib(A_DEX, 1, FALSE, TRUE);
		if (!rn2(3)) (void) adjattrib(A_CON, 1, FALSE, TRUE);
		if (!rn2(3)) (void) adjattrib(A_INT, 1, FALSE, TRUE);
		if (!rn2(3)) (void) adjattrib(A_WIS, 1, FALSE, TRUE);
		if (!rn2(3)) (void) adjattrib(A_CHA, 1, FALSE, TRUE);

	    } else {
		(void) adjattrib(A_STR, -1, FALSE, TRUE);
		(void) adjattrib(A_DEX, -1, FALSE, TRUE);
		(void) adjattrib(A_CON, -1, FALSE, TRUE);
		(void) adjattrib(A_INT, -1, FALSE, TRUE);
		(void) adjattrib(A_WIS, -1, FALSE, TRUE);
		(void) adjattrib(A_CHA, -1, FALSE, TRUE);

	    }
	    pline("You'll have incessant flatulence for a while now...");
	    FemaleTrapMaurah += rnz(500);

	    lesshungry(600);
	    u.uconduct.food++;
	} else if (tin.tin->spe == 3) {
	    if (tin.tin->cursed)
		pline("It contains some decaying%s%s substance.",
			Blind ? "" : " ", Blind ? "" : hcolor(NH_PURPLE));
	    else
		pline(FunnyHallu ? "It contains mangos." : "It contains peaches.");

	    getlin ("Eat it? [y/yes/no]",buf);
	    (void) lcase (buf);
	    if (strcmp (buf, "yes") && strcmp (buf, "y")) {
		if (!Hallucination && !tin.tin->cursed)
		    tin.tin->dknown = tin.tin->known = TRUE;
		if (flags.verbose)
		    You("discard the open tin.");
		costly_tin((const char*)0);
		goto use_me;
	    }

	    tin.tin->dknown = tin.tin->known = TRUE;
	    costly_tin((const char*)0);

	    if (!tin.tin->cursed) {
		(void) adjattrib(A_CON, 1, FALSE, TRUE);

	    } else {
		(void) adjattrib(A_CON, -1, FALSE, TRUE);

	    }

	    lesshungry(600);
	    u.uconduct.food++;
	} else if (tin.tin->spe == 4) {
	    if (tin.tin->cursed)
		pline("It contains something extremely stinky.");
	    else
		pline(FunnyHallu ? "It contains submarines and tadpoles." : "It contains fish.");

	    getlin ("Eat it? [y/yes/no]",buf);
	    (void) lcase (buf);
	    if (strcmp (buf, "yes") && strcmp (buf, "y")) {
		if (!Hallucination && !tin.tin->cursed)
		    tin.tin->dknown = tin.tin->known = TRUE;
		if (flags.verbose)
		    You("discard the open tin.");
		costly_tin((const char*)0);
		goto use_me;
	    }

	    tin.tin->dknown = tin.tin->known = TRUE;
	    costly_tin((const char*)0);

	    if (!tin.tin->cursed) {
		(void) adjattrib(A_INT, 1, FALSE, TRUE);

	    } else {
		(void) adjattrib(A_INT, -1, FALSE, TRUE);

	    }

	    lesshungry(600);
	    u.uconduct.food++;
	}
use_me:
	if (carried(tin.tin)) useup(tin.tin);
	else useupf(tin.tin, 1L);
	tin.tin = (struct obj *) 0;
	return(0);
}

STATIC_OVL void
start_tin(otmp)		/* called when starting to open a tin */
	register struct obj *otmp;
{
	register int tmp;

	if (metallivorous(youmonst.data)) {
		You("bite right into the metal tin...");
		tmp = 1;
	} else if (nolimbs(youmonst.data) && !Race_if(PM_TRANSFORMER) ) {
		You("cannot handle the tin properly to open it.");
		if (yn("Attempt to force it open?") == 'y') {
			if (rn2(3) && !polyskillchance()) {
				playerbleed(rnd(2 + (level_difficulty() * 10)));
				pline("Great. Now your %s is squirting everywhere.", body_part(BLOOD));
				if (!rn2(20)) badeffect();
				return;
			}

		}
		else return;
	}
	if (otmp->blessed) {
		pline_The(FunnyHallu ? "tin is opened by the little man sitting inside!" : "tin opens like magic!");
		tmp = 1;
	} else if(uwep) {
		switch(uwep->otyp) {
		case TIN_OPENER:
		case LASER_TIN_OPENER:
		case BUDO_NO_SASU:
			tmp = 1;
			break;
		case DAGGER:
		case SILVER_DAGGER:
		case ELVEN_DAGGER:
		case ORCISH_DAGGER:
		case ATHAME:
		case MERCURIAL_ATHAME:
		case CRYSKNIFE:
		case DARK_ELVEN_DAGGER:
		case GREAT_DAGGER:
		case RADIOACTIVE_DAGGER:
		case SECRETION_DAGGER:
		case WONDER_DAGGER:
		case BONE_DAGGER:
		case WRAITHBLADE:
		case TECPATL:
		case DROVEN_DAGGER:
			tmp = 3;
			break;
		case PICK_AXE:
		case CONGLOMERATE_PICK:
		case CONUNDRUM_PICK:
		case MYSTERY_PICK:
		case BRONZE_PICK:
		case BRICK_PICK:
		case NANO_PICK:
		case AXE:
			tmp = 6;
			break;
		default:
			goto no_opener;
		}
		pline("Using your %s you try to open the tin.",
			aobjnam(uwep, (char *)0));
	} else {
no_opener:
		pline(FunnyHallu ? "Using your fingernails, you try to open this tin." : "It is not so easy to open this tin.");
		if(IsGlib) {
			pline_The("tin slips from your %s.",
			      makeplural(body_part(FINGER)));
			if(otmp->quan > 1L) {
			    otmp = splitobj(otmp, 1L);
			}
			if (carried(otmp)) dropx(otmp);
			else stackobj(otmp);
			return;
		}
		tmp = rn1(1 + 500/((int)(ACURR(A_DEX) + ACURRSTR)), 10);
	}
	tin.reqtime = tmp;
	tin.usedtime = 0;
	tin.tin = otmp;
	set_occupation(opentin, "opening the tin", 0);
	return;
}

int
Hear_again()		/* called when waking up after fainting */
{
	if (!(YouAreDeaf)) flags.soundok = 1;
	return 0;
}

/* called on the "first bite" of rotten food */
STATIC_OVL int
rottenfood(obj)
struct obj *obj;
{
	pline("Blecch!  Rotten %s!", foodword(obj));
	if (issoviet) pline("Tip bloka l'da vinovat v etom!");

	if (FunnyHallu && (obj->otyp == K_RATION || obj->otyp == C_RATION)) {
		pline("That wasn't a meal, it wasn't ready, and you shouldn't have eaten it.");
	}

	if(!rn2(4)) {
		if (FunnyHallu) You_feel("rather trippy.");
		else You_feel("rather %s.", body_part(LIGHT_HEADED));
		make_confused(HConfusion + d(2,4),FALSE);
	} else if(!rn2(5)) {
		if (FunnyHallu) You_feel("uncontrollable."); /* this and some other effects added by Amy */
		else You_feel("stunned.");
		make_stunned(HStun + d(2,4),FALSE);
	} else if(!rn2(5)) {
		if (FunnyHallu) You_feel("even weirder!");
		else You_feel("weirded out!");
		make_hallucinated(HHallucination + d(2,10),FALSE,0L);
	} else if(!rn2(4)) {
		if (!Blind) pline("Everything suddenly goes dark.");
		make_blinded(Blinded+d(2,10),FALSE);
		if (!Blind) Your("%s", vision_clears);
	} else if(!rn2(8)) {
		if (FunnyHallu) You_feel("like your body is trying to fall asleep!");
		else You_feel("numb.");
		make_numbed(HNumbed + d(2,15),FALSE);
	} else if(!rn2(10)) {
		if (FunnyHallu) pline("Crap! A %s suddenly appeared! RUN!",rndmonnam() );
		else You_feel("fear spreading through your body.");
		make_feared(HFeared + d(2,15),FALSE);
	} else if(!rn2(10)) {
		if (FunnyHallu) You_feel("like your on fire despite there being no heat!");
		else You_feel("like you're on fire!");
		make_burned(HBurned + d(2,15),FALSE);
	} else if(!rn2(20)) {
		if (FunnyHallu) You_feel("a giant ice cream cone enclosing you!");
		else pline("You're getting the chills.");
		make_frozen(HFrozen + d(2,15),FALSE);
	} else if(!rn2(20)) {
		if (FunnyHallu) You_feel("that your wife is gonna break up with you!");
		else pline("You're dimmed.");
		make_dimmed(HDimmed + d(2,15),FALSE);
	} else if(!rn2(10)) {
		if (FunnyHallu) You_feel("like you have dementia tremor!"); /* not a real name --Amy */
		else pline("Your hands start trembling violently!");
		incr_itimeout(&Glib, d(2,4) );
	} else if(!rn2(10)) {
		if (FunnyHallu) You_feel("totally down! Seems you tried some illegal shit!");
		else You_feel("like you're going to throw up.");
	      make_vomiting(Vomiting+20, TRUE);
		if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */
	} else if(!rn2(3)) {
		const char *what, *where;
		if (!Blind)
		    what = "goes",  where = "dark";
		else if (Levitation || Is_airlevel(&u.uz) ||
			 Is_waterlevel(&u.uz))
		    what = "you lose control of",  where = "yourself";
		else
		    what = "you slap against the", where = (u.usteed) ? "saddle" : surface(u.ux,u.uy);
		pline_The("world spins and %s %s.", what, where);
		flags.soundok = 0;
		nomul(-rnd(10), "unconscious from rotten food", TRUE);
		nomovemsg = "You are conscious again.";
		afternmv = Hear_again;
		return(1);
	} else if(!rn2(50) && !Sick) { /* The chance of this outcome !MUST! be low. Everything else would be unfair. --Amy */
	    make_sick(rn1(25,25), "rotten food", TRUE, SICK_VOMITABLE);
	} else if(!rn2(200) && !Slimed && !issoviet && !flaming(youmonst.data) && !Unchanging && !slime_on_touch(youmonst.data) ) { /* This chance should be even lower. --Amy */
			You("don't feel very well.");
			make_slimed(100);
		    killer_format = KILLED_BY_AN;
		    delayed_killer = "slimed by a slimy meal";
	}
	return(0);
}

/* [ALI] Return codes:
 *
 *	0 - Ready to start eating
 *	1 - Corpse partly eaten, but don't start occupation
 *	2 - Corpse completely consumed, victual.piece left dangling
 *	3 - Corpse was inedible
 */

STATIC_OVL int
eatcorpse(otmp)		/* called when a corpse is selected as food */
	register struct obj *otmp;
{
	int tp = 0, mnum = otmp->corpsenm;
	long rotted = 0L;
	boolean uniq = !!(mons[mnum].geno & G_UNIQ);
	int retcode = 0;
	boolean stoneable = (touch_petrifies(&mons[mnum]) && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) &&
				!poly_when_stoned(youmonst.data));

	register struct permonst *ptr = &mons[mnum];

	/* KMH, conduct */
	if (!vegan(&mons[mnum])) u.uconduct.unvegan++;
	if (!vegetarian(&mons[mnum])) violated_vegetarian();
	gluttonous();

	if ( (Race_if(PM_ARMED_COCKATRICE) || Race_if(PM_CORTEX)) && !lithivorous(youmonst.data) && !Upolyd && rn2(2) ) {

		if (rn2(2)) {
			pline("Ulch - that food was petrified! You're unable to swallow it.");
			badeffect();
		} else pline("The food turns to stone as you try to eat it!");

		if (carried(otmp)) useup(otmp);
		else useupf(otmp, 1L);

		return 2;
	}

	if ( (Race_if(PM_ARMED_COCKATRICE) || Race_if(PM_CORTEX)) && !lithivorous(youmonst.data) && !Upolyd && rn2(2) ) {

		pline("Ulch - that food was petrified!");
		badeffect();

	}

	if ( (Race_if(PM_ARMED_COCKATRICE) || Race_if(PM_CORTEX)) && lithivorous(youmonst.data) && !Upolyd && !rn2(4) ) {

		if (rn2(2)) {
			pline("Ulch - that petrified food had manky bits! You're unable to swallow it.");
			badeffect();
		} else pline("The petrified food appears to be manky as you try to eat it!");

		if (carried(otmp)) useup(otmp);
		else useupf(otmp, 1L);

		return 2;
	}

	if ( (Race_if(PM_ARMED_COCKATRICE) || Race_if(PM_CORTEX)) && lithivorous(youmonst.data) && !Upolyd && !rn2(4) ) {

			pline("Ulch - that petrified food had manky bits!");
		badeffect();

	}

	if (!nocorpsedecay(&mons[mnum])) {
		long age = peek_at_iced_corpse_age(otmp);

		rotted = (monstermoves - age)/(10L + rn2(20));
		if (FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone()) rotted += 2L;
		if (otmp->cursed) rotted += 2L;
		else if (otmp->blessed) rotted -= 2L;
	}

	/* Vampires only drink the blood of very young, meaty corpses 
	 * is_edible only allows meaty corpses here
	 * Blood is assumed to be 1/5 of the nutrition
	 * Thus happens before the conduct checks intentionally - should it be after?
	 * Blood is assumed to be meat and flesh.
	 */
	if (is_vampire(youmonst.data) || (Role_if(PM_GOFF) && !Upolyd) ) {
	    /* oeaten is set up by touchfood */
	    if (otmp->odrained ? otmp->oeaten <= drainlevel(otmp) :
	      otmp->oeaten < mons[otmp->corpsenm].cnutrit) {
	    	pline(FunnyHallu ? "No juice left - gotta get a new bottle from your cellar!" : "There is no blood left in this corpse!");
	    	return 3;
	    } else if (rotted <= 0 &&
	      (peek_at_iced_corpse_age(otmp) + 5) >= monstermoves) {
		char buf[BUFSZ];

		/* Generate the name for the corpse */
		if (!uniq || Hallucination)
		    sprintf(buf, "%s", the(corpse_xname(otmp,TRUE)));
		else
		    sprintf(buf, "%s%s corpse",
			    !type_is_pname(&mons[mnum]) ? "the " : "",
			    s_suffix(mons[mnum].mname));

	    	pline("You drain the blood from %s.", buf);
		otmp->odrained = 1;
	    } else {
	    	pline(FunnyHallu ? "You try to suck, but it just tastes very disgusting!" : "The blood in this corpse has coagulated!");
	    	return 3;
	    }
	}
	else
	    otmp->odrained = 0;

	/* Very rotten corpse will make you sick unless you are a ghoul or a ghast */
	if (!acidic(&mons[mnum]) && !stoneable && rotted > 5L) {
	    boolean cannibal = maybe_cannibal(mnum, FALSE);
	    if (saprovorousnumber(u.umonnum) || (Race_if(PM_GASTLY) && !Upolyd) || (Race_if(PM_PLAYER_SKELETON) && !Upolyd) || (Race_if(PM_PHANTOM_GHOST) && !Upolyd) ) {
	    	pline("Yum - that %s was well aged%s!",
		      mons[mnum].mlet == S_FUNGUS ? "fungoid vegetation" :
		      !vegetarian(&mons[mnum]) ? "meat" : "protoplasm",
		      cannibal ? ", cannibal" : "");
	    } else {	    
		pline("Ulch - that %s was tainted%s!",
		      mons[mnum].mlet == S_FUNGUS ? "fungoid vegetation" :
		      !vegetarian(&mons[mnum]) ? "meat" : "protoplasm",
		      cannibal ? ", cannibal" : "");
		if (IntSick_resistance || (ExtSick_resistance && rn2(20)) ) {
			pline(FunnyHallu ? "Interesting taste, though..." : "It doesn't seem at all sickening, though...");
		} else {
			char buf[BUFSZ];
			long sick_time;

			sick_time = (long) rn1(20, 20);
			/* make sure new ill doesn't result in improvement */
			if (Sick && (sick_time > Sick))
			    sick_time = (Sick > 1L) ? Sick - 1L : 1L;
			if (!uniq || Hallucination)
			    sprintf(buf, "rotted %s", corpse_xname(otmp,TRUE));
			else
			    sprintf(buf, "%s%s rotted corpse",
				    !type_is_pname(&mons[mnum]) ? "the " : "",
				    s_suffix(mons[mnum].mname));
			make_sick(sick_time, buf, TRUE, SICK_VOMITABLE);
		}
		if (!evilfriday) {
			cpostfx(mnum);
			if (carried(otmp)) useup(otmp);
			else useupf(otmp, 1L);
			return(2);
		}
	    }
	} else if (saprovorous(youmonst.data) || (Race_if(PM_GASTLY) && !Upolyd) || (Race_if(PM_PLAYER_SKELETON) && !Upolyd) || (Race_if(PM_PHANTOM_GHOST) && !Upolyd) ) {
		pline (FunnyHallu ? "You can't seem to find any manky bits!" : "This corpse is too fresh!");
		return 3;
	} else if (acidic(&mons[mnum]) && (!Acid_resistance || (!StrongAcid_resistance && !rn2(10))) ) {
		tp++;
		You("have a very bad case of stomach acid."); /* not body_part() */
		losehp(rnd(15 + ptr->mlevel), "acidic corpse", KILLED_BY_AN);
	} else if (poisonous(&mons[mnum]) && rn2(5)) {
		tp++;
		pline(FunnyHallu ? "Feels like your face is turning green!" : "Ecch - that must have been poisonous!");
		if(!Poison_resistance) {

			/* tone down strength loss, since you have to eat many more poisonous
			 * corpses in order to get poison resistance --Amy
			 * In Soviet Russia, kobolds and certain animals are REALLY poisonous and will completely trash
			 * your strength when eaten. No one should be able to gain poison resistance from eating those without
			 * sacrificing a significant amount of strength because otherwise the communist government can't sell
			 * enough antidotes and stuff. */

			if (!rn2(3)) losestr(rnd(2), TRUE);
			if (!rn2(60)) losestr(rnd(2), TRUE);
			if (issoviet) {
				losestr(rnd(5), TRUE);
				pline("On, on, ty otravilsya i, veroyatno, poteryal silu! Nay khe khe!");
			}
			losehp(rnd(15 + ptr->mlevel), "poisonous corpse", KILLED_BY_AN);
		} else	You("seem unaffected by the poison.");
	/* now any corpse left too long will make you mildly ill */
	} else if ((rotted > 5L || (rotted > 3L && rn2(5)))
					&& !(IntSick_resistance || (ExtSick_resistance && rn2(20))) ) {
		tp++;
		You_feel("%ssick.", (Sick) ? "very " : "");
		losehp(rnd(8 + ptr->mlevel), "cadaver", KILLED_BY_AN);
	}

	/* delay is weight dependent */
	/* In Soviet Russia, food is so scarce that you will always want to take your sweet time eating it. Who cares that
	 * it takes 75 turns to eat a dragon corpse, or that you'll faint at least thrice until the slow eating speed
	 * finally bumps you out of "Fainting" and back into "Weak" territory? And who wants to defeat Famine, anyway?
	 * Nah, let's make everything completely fucked up, at least if the player has no K-rations. --Amy */

	victual.reqtime = (issoviet ? 5 : 2) + (mons[mnum].cwt >> (issoviet ? 6 : 8) ); /* speed up --Amy */
	if (otmp->odrained) {
		if (victual.reqtime) {
			victual.reqtime = rounddiv(victual.reqtime, 5);
			if (!victual.reqtime) victual.reqtime = 1;
		}
	}

	/* In Soviet Russia, food is so scarce, even if you do find some it's often rotten. And the fungi that cause the
	 * food to rot also don't care about weird stuff like "blessings", no those aren't allowed to offer protection at
	 * all. So you can save your holy waters, halleluja! --Amy */

	if (!tp && !nocorpsedecay(&mons[mnum]) && mons[mnum].mlet != S_TROVE &&
			(otmp->orotten || otmp->cursed || FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone() || (issoviet && !rn2(5)) || (!rn2(20) && !otmp->blessed) ) ) {
/* Come on, blessed food being equally susceptible to rotting is just stupid. --Amy */

	    if (rottenfood(otmp)) {
		otmp->orotten = TRUE;
		(void)touchfood(otmp);
		retcode = 1;
	    }

	    if (!mons[otmp->corpsenm].cnutrit) {
		/* no nutrution: rots away, no message if you passed out */
		if (!retcode) pline_The("corpse rots away completely.");
		if (carried(otmp)) useup(otmp);
		else useupf(otmp, 1L);
		retcode = 2;
	    }
		    
	    if (!retcode) consume_oeaten(otmp, 2);	/* oeaten >>= 2 */
	    if (retcode<2 && otmp->odrained && otmp->oeaten < drainlevel(otmp))
	        otmp->oeaten = drainlevel(otmp);
	} else if (!is_vampire(youmonst.data)) {
	    boolean pname = type_is_pname(&mons[mnum]);
	    pline("%s%s %s!",
		  !uniq ? "This " : !pname ? "The " : "",
		  uniq && pname ?
		   Food_xname(otmp, FALSE) : food_xname(otmp, FALSE),
		  (vegan(&mons[mnum]) ?
		   (!carnivorous(youmonst.data) && herbivorous(youmonst.data)) :
		   (carnivorous(youmonst.data) && !herbivorous(youmonst.data)))
		  ? "is delicious" : "tastes terrible");
	}

	/* WAC Track food types eaten */
	if (mvitals[mnum].eaten < 255) mvitals[mnum].eaten++;

	return(retcode);
}

STATIC_OVL void
start_eating(otmp)		/* called as you start to eat */
	register struct obj *otmp;
{
#ifdef DEBUG
	debugpline("start_eating: %lx (victual = %lx)", otmp, victual.piece);
	debugpline("reqtime = %d", victual.reqtime);
	debugpline("(original reqtime = %d)", objects[otmp->otyp].oc_delay);
	debugpline("nmod = %d", victual.nmod);
	debugpline("oeaten = %d", otmp->oeaten);
#endif
	victual.fullwarn = victual.doreset = FALSE;
	victual.eating = TRUE;

	if (otmp->otyp == CORPSE) {
	    cprefx(victual.piece->corpsenm);
	    if (!victual.piece || !victual.eating) {
		/* rider revived, or died and lifesaved */
		return;
	    }
	}

	if (bite()) return;

	if (++victual.usedtime >= victual.reqtime) {
	    /* print "finish eating" message if they just resumed -dlc */
	    done_eating(victual.reqtime > 1 ? TRUE : FALSE);
	    return;
	}

	sprintf(msgbuf, "%s %s", otmp->odrained ? "draining" : "eating",
	  food_xname(otmp, TRUE));
	set_occupation(eatfood, msgbuf, 0);
}


/*
 * called on "first bite" of (non-corpse) food.
 * used for non-rotten non-tin non-corpse food
 */
STATIC_OVL void
fprefx(otmp)
struct obj *otmp;
{
	switch(otmp->otyp) {

	    case BREAD:
	    case FOOD_RATION:
		if(u.uhunger <= 500)
		    pline(FunnyHallu ? "Oh wow, like, superior, man!" :
			  "That food really hit the spot!");
		else if(u.uhunger <= 1200) pline("That satiated your %s!",
						body_part(STOMACH));

		if (otmp && otmp->oartifact == ART_HOE_PA) {
			if (!Cold_resistance) You_feel("more resistant to cold!");
			incr_itimeout(&HCold_resistance, rnz(10000));
			if (!Fire_resistance) You_feel("more resistant to fire!");
			incr_itimeout(&HFire_resistance, rnz(10000));
			if (!Shock_resistance) You_feel("more resistant to shock!");
			incr_itimeout(&HShock_resistance, rnz(10000));
			if (!Poison_resistance) You_feel("more resistant to poison!");
			incr_itimeout(&HPoison_resistance, rnz(10000));
		}

		break;
	    case TRIPE_RATION:
		if (carnivorous(youmonst.data) && (!humanoid(youmonst.data)) || 
			u.ulycn != NON_PM && carnivorous(&mons[u.ulycn]) && 
			!humanoid(&mons[u.ulycn]))
		    /* Symptom of lycanthropy is starting to like your
		     * alternative form's food! 
		     */
		    pline("That tripe ration was surprisingly good!");
		else if (maybe_polyd(is_orc(youmonst.data), Race_if(PM_ORC)))
		    pline(FunnyHallu ? "Tastes great! Less filling!" :
			  "Mmm, tripe... not bad!");
		else {
		    pline("Yak - dog food!");
		    if (Role_if(PM_CONVICT))
			pline("At least it's not prison food.");
		    more_experienced(1,0);
		    newexplevel();
		    /* not cannibalism, but we use similar criteria
		       for deciding whether to be sickened by this meal */
		    if (rn2(2) && !cannibal_allowed())
		    if (!Role_if(PM_CONVICT))
			{ make_vomiting((long)rn1(victual.reqtime, 14), FALSE);
			if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */
			}
		}
		break;
	    case PILL:
		You("swallow the little pink pill.");

		if (otmp && otmp->oartifact == ART_PILL_THAT_KILLED_MICHAEL_J) {
		      make_sick(rn1(25,25), "a poisonous pill", TRUE, SICK_VOMITABLE);
			losehp(rn1(30, 30),"poisonous pill",KILLED_BY_AN);
			(void) adjattrib(A_STR, -rnd(6), FALSE, TRUE);
			(void) adjattrib(A_DEX, -rnd(6), FALSE, TRUE);
			(void) adjattrib(A_CON, -rnd(6), FALSE, TRUE);
			(void) adjattrib(A_INT, -rnd(6), FALSE, TRUE);
			(void) adjattrib(A_WIS, -rnd(6), FALSE, TRUE);
			(void) adjattrib(A_CHA, -rnd(6), FALSE, TRUE);
		}

		switch(rn2(7))
		{
		   case 0:

			if (rn2(100)) { /* make wishes much less common --Amy */

				if (rn2(2)) make_sick(rn1(15,15), "bad pill", TRUE, SICK_VOMITABLE);
				else {
					pline("Oh god, it was a sex change drug...");
					change_sex();
				}
				break;
			}

			else {

			/* [Tom] wishing pills are from the Land of Oz */
			pline ("The pink sugar coating hid a silver special pill!"); /* wish no longer guaranteed --Amy */
			if (!rn2(4)) makewish(evilfriday ? FALSE : TRUE);
			else othergreateffect();
			break;
			}
		   case 1:
			if(!Poison_resistance || (!rn2(10) && !StrongPoison_resistance) ) {
				You_feel("your stomach twinge.");
				losestr(rnd(4), TRUE);
				losehp(rnd(15), "poisonous pill", KILLED_BY_AN);
			} else  You("seem unaffected by the poison.");
			break;
		   case 2:
			pline ("Everything begins to get blurry.");
			make_stunned(HStun + 30,FALSE);
			break;
		   case 3:
			pline ("Oh wow!  Look at the lights!");
			make_hallucinated(HHallucination + 150,FALSE,0L);
			break;
		   case 4:
			pline("That tasted like vitamins...");
			lesshungry(600);
			break;
		   case 5:
			if(Sleep_resistance && (StrongSleep_resistance || rn2(10)) ) {
				pline("Hmm. Nothing happens.");
			} else {
				You_feel("drowsy...");
				nomul(-rn2(50), "sleeping from a pink pill", TRUE);
				u.usleep = 1;
				nomovemsg = "You wake up.";
			}
			break;
		   case 6:
			pline("Wow... everything is moving in slow motion...");
			/* KMH, balance patch -- Use incr_itimeout() instead of += */
			incr_itimeout(&HFast, rn1(10,200));
			break;
		}
		break;
	    case MUSHROOM:
	       pline("This %s is %s", singular(otmp, xname),
	       otmp->cursed ? (FunnyHallu ? "far-out!" : "terrible!") :
		      FunnyHallu ? "groovy!" : "delicious!");
		switch(rn2(10))
		{
		   case 0:
		   case 1:
			if(!Poison_resistance || (!rn2(10) && !StrongPoison_resistance) ) {
				You_feel("rather ill....");
				losestr(rnd(4), TRUE);
				losehp(rnd(15), "poisonous mushroom", KILLED_BY_AN);
			} else  You("burp loudly.");
			break;
		   case 2:
			pline ("That mushroom tasted a little funny.");
			make_stunned(HStun + 30,FALSE);
			break;
		   case 3:
			pline ("Whoa! Everything looks groovy!");
			make_hallucinated(HHallucination + 150,FALSE,0L);
			break;
		   case 4:
			gainstr(otmp, 1);
			pline ("You feel stronger!");
			break;                                           
		   case 5:
		   case 6:
		   case 7:
		   case 8:
		   case 9:
			break;
		}
		break;
	    case SHEAF_OF_STRAW:
	    case COTTON:
		if (herbivorous(youmonst.data) && !carnivorous(youmonst.data)) pline("That %s was tasty!", xname(otmp));
		break;
	    case MEATBALL:
	    case MEAT_STICK:
	    case HUGE_CHUNK_OF_MEAT:
	    case MEAT_RING:
		goto give_feedback;
	     /* break; */

	    case X_MAS_CAKE:

		if (!rn2(3)) pline(FunnyHallu ? "...It tastes sweet, so, too sweet!" :
			"You munched, munched, munched...It's delicious!");
		else pline(rn2(2) ? "(That's dud, tut.)  You feel that someone clucked her tongue." :
			"Very delicious, mew!  You feel that your toon seems changing, mew.");

		break;

	    case BUNNY_CAKE:
		/*can you eat it all?  I believe it not!*/
		more_experienced(0,10);
		newexplevel();
		pline(rn2(2) ? "(That's dud, tut.)  You feel that someone clucked her tongue." :
			"Very delicious, mew!  You feel that your toon seems changing, mew.");

		break;

	    case SAKURA_MOCHI:
	    case KIBI_DANGO:

		pline("You are supposed to get some kind of message here but the Japanese to English translation isn't complete yet. Sorry.");

		break;

	    case KOUHAKU_MANJYUU:

		pline(rn2(2) ? "It tastes year-crossing." : "You feel like kotatsu-snail!");

		break;

	    case YOUKAN:

		You_feel("somehow you are playing NetHack brass.");

		break;

	    case WHITE_PEACH:
	    case SENTOU:
		if (hates_silver(youmonst.data) || (uwep && uwep->oartifact == ART_PORKMAN_S_BALLS_OF_STEEL) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_PORKMAN_S_BALLS_OF_STEEL) ) {
			make_vomiting((long)rn1(victual.reqtime, 5), FALSE);
			break;
		}
		/* Fall through otherwise */
	    case BEAN:
	    case SENZU:

	    case CLOVE_OF_GARLIC:
		if (is_undead(youmonst.data)) {
			make_vomiting((long)rn1(victual.reqtime, 5), FALSE);
			if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */
			break;
		}
		/* Fall through otherwise */
	    default:

		if (otmp && otmp->oartifact == ART_DOENERTELLER_VERSACE) {
			if (!Antimagic) You_feel("more resistant to magic!");
			incr_itimeout(&HAntimagic, rnz(2000));
			if (!Drain_resistance) You_feel("more resistant to drain life!");
			incr_itimeout(&HDrain_resistance, rnz(2000));
			if (!Reflecting) You_feel("reflexive!");
			incr_itimeout(&HReflecting, rnz(2000));
		}

		if (otmp->otyp == SLIME_MOLD && !otmp->cursed && !(FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone())
			&& otmp->spe == current_fruit)
		    pline("My, that was a %s %s!",
			  FunnyHallu ? "primo" : "yummy",
			  singular(otmp, xname));
		else
#ifdef UNIX
		if (otmp->otyp == APPLE || otmp->otyp == PEAR) {
		    if (!FunnyHallu) pline("Core dumped.");
		    else {
/* This is based on an old Usenet joke, a fake a.out manual page */
			int x = rnd(100);
			if (x <= 75)
			    pline("Segmentation fault -- core dumped.");
			else if (x <= 99)
			    pline("Bus error -- core dumped.");
			else pline("Yo' mama -- core dumped.");
		    }
		} else
#endif
#ifdef MAC	/* KMH -- Why should Unix have all the fun? */
		if (otmp->otyp == APPLE) {
			pline("This Macintosh is wonderful!");
		} else
#endif
		if (otmp->otyp == EGG && otmp->corpsenm != PM_EASTER && stale_egg(otmp)) {
		    pline(FunnyHallu ? "Uaaah! That tasted like hydrogen sulfide!" : "Ugh.  Rotten egg.");	/* perhaps others like it */
		if (Role_if(PM_CONVICT) && (rn2(8) > u.ulevel)) {
		    You_feel("a slight stomach ache.");	/* prisoners are used to bad food */
		} else
		    {make_vomiting(Vomiting+d(10,4), TRUE);
			if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */
			}
		} else {
		    boolean bad_for_you;
 give_feedback:
		    bad_for_you = otmp->cursed || FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone() || (otmp->otyp == CHARRED_BREAD) ||
		      ((Race_if(PM_HUMAN_WEREWOLF) || Role_if(PM_LUNATIC) || Race_if(PM_AK_THIEF_IS_DEAD_)) &&
		      otmp->otyp == SPRIG_OF_WOLFSBANE);
		    pline("This %s is %s", singular(otmp, xname),
		      bad_for_you ? (FunnyHallu ? "grody!" : "terrible!") :
		      (otmp->otyp == CRAM_RATION
		      || otmp->otyp == K_RATION
		      || otmp->otyp == C_RATION)
		      ? (FunnyHallu ? "enjoyable." : "bland.") :
		      FunnyHallu ? "gnarly!" : "delicious!");
		}

		break;
	}
}

STATIC_OVL void
accessory_has_effect(otmp)
struct obj *otmp;
{
	pline("Magic spreads through your body as you digest the %s.",
	    otmp->oclass == RING_CLASS ? "ring" : otmp->oclass == IMPLANT_CLASS ? "implant" : "amulet");
	u.cnd_ringtrinsiccount++;
}

STATIC_OVL void
eataccessory(otmp)
struct obj *otmp;
{
	int typ = otmp->otyp;
	long oldprop;

	/* Note: rings are not so common that this is unbalancing. */
	/* (How often do you even _find_ 3 rings of polymorph in a game?) */
	/* KMH, intrinsic patch -- several changes below */
	oldprop = u.uprops[objects[typ].oc_oprop].intrinsic;
	if (otmp == uleft || otmp == uright) {
	    Ring_gone(otmp);
	    if (u.uhp <= 0) return; /* died from sink fall */
	}
	otmp->known = otmp->dknown = 1; /* by taste */
	if ((!rn2(otmp->oclass == RING_CLASS ? 5 : otmp->oclass == IMPLANT_CLASS ? 2 : 10)) || (Race_if(PM_OCTOPODE) && otmp->oclass == RING_CLASS) || (objects[(otmp)->otyp].oc_material == MT_VIVA && !rn2(5)) ) { /* lower chance, due to existence of sickness resistance etc --Amy */
	  switch (otmp->otyp) {
	    default:
	        if (!objects[typ].oc_oprop) break; /* should never happen */

		if (!(u.uprops[objects[typ].oc_oprop].intrinsic & FROMOUTSIDE))
		    accessory_has_effect(otmp);

		u.uprops[objects[typ].oc_oprop].intrinsic |= FROMOUTSIDE;

		switch (typ) {
		  case RIN_SEE_INVISIBLE:
		    set_mimic_blocking();
		    see_monsters();
		    if (Invis && !oldprop && !ESee_invisible &&
				!perceives(youmonst.data) && !Blind) {
			newsym(u.ux,u.uy);
			pline("Suddenly you can see yourself.");
			makeknown(typ);
		    }
		    break;
		  case RIN_INVISIBILITY:
		    if (!oldprop && !EInvis && !BInvis &&
					!See_invisible && !Blind) {
			newsym(u.ux,u.uy);
			Your("body takes on a %s transparency...",
				FunnyHallu ? "normal" : "strange");
			makeknown(typ);
		    }
		    break;
		  case RIN_PROTECTION_FROM_SHAPE_CHAN:
		    rescham();
		    break;
		  case RIN_DISARMING:
		    incr_itimeout(&Glib, 2);			
		    break;
		  case RIN_LEVITATION:
		    /* undo the `.intrinsic |= FROMOUTSIDE' done above */
		    u.uprops[LEVITATION].intrinsic = oldprop;
		    if (!Levitation) {
			float_up();
			incr_itimeout(&HLevitation, d(10,20));
			makeknown(typ);
		    }
		    break;
		}
		break;
	    case RIN_ADORNMENT:
		accessory_has_effect(otmp);
		if (adjattrib(A_CHA, otmp->spe, -1, TRUE))
		    makeknown(typ);
		break;
	    case RIN_GAIN_STRENGTH:
		accessory_has_effect(otmp);
		if (adjattrib(A_STR, otmp->spe, -1, TRUE))
		    makeknown(typ);
		break;
	    case RIN_GAIN_CONSTITUTION:
		accessory_has_effect(otmp);
		if (adjattrib(A_CON, otmp->spe, -1, TRUE))
		    makeknown(typ);
		break;
	    case RIN_GAIN_INTELLIGENCE:
		accessory_has_effect(otmp);
		if (adjattrib(A_INT, otmp->spe, -1, TRUE))
		    makeknown(typ);
		break;
	    case RIN_GAIN_WISDOM:
		accessory_has_effect(otmp);
		if (adjattrib(A_WIS, otmp->spe, -1, TRUE))
		    makeknown(typ);
		break;
	    case RIN_GAIN_DEXTERITY:
		accessory_has_effect(otmp);
		if (adjattrib(A_DEX, otmp->spe, -1, TRUE))
		    makeknown(typ);
		break;
	    case RIN_INCREASE_ACCURACY:
		accessory_has_effect(otmp);
		u.uhitinc += otmp->spe;
		u.uhitincxtra += otmp->spe;
		break;
	    case RIN_INCREASE_DAMAGE:
		accessory_has_effect(otmp);
		u.udaminc += otmp->spe;
		u.udamincxtra += otmp->spe;
		break;
	    case RIN_HEAVY_ATTACK:
		accessory_has_effect(otmp);
		u.uhitinc += otmp->spe;
		u.uhitincxtra += otmp->spe;
		u.udaminc += otmp->spe;
		u.udamincxtra += otmp->spe;
		break;
	    case RIN_PROTECTION:
	    case RIN_THREE_POINT_SEVEN_PROTECTI:
		accessory_has_effect(otmp);
		HProtection |= FROMOUTSIDE;
		u.ublessed += otmp->spe;
		flags.botl = 1;
		break;
	    /*case RIN_FREE_ACTION:*/
		/* Give sleep resistance instead */
		/*if (!(HSleep_resistance & FROMOUTSIDE))
		    accessory_has_effect(otmp);
		if (!Sleep_resistance)
		    You_feel("wide awake.");
		HSleep_resistance |= FROMOUTSIDE;
		break;*/
	    case AMULET_OF_CHANGE:
		accessory_has_effect(otmp);
		makeknown(typ);
		change_sex();
		You("are suddenly very %s!",
		    flags.female ? "feminine" : "masculine");
		flags.botl = 1;
		break;
	    case AMULET_OF_POLYMORPH:
		accessory_has_effect(otmp);
		makeknown(typ);
		You_feel("rather strange.");
		polyself(FALSE);
		flags.botl = 1;
		break;

	    case AMULET_OF_STONE:
		accessory_has_effect(otmp);
		makeknown(typ);
		if (!Stoned && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && !(poly_when_stoned(youmonst.data) &&
				 polymon(PM_STONE_GOLEM)) ) {
			if (Hallucination && rn2(10)) pline("Good thing you are already stoned.");
			else {
				You("start turning to stone.");
				Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
				u.cnd_stoningcount++;
				stop_occupation();
				delayed_killer = "eating a petrifying amulet";
			}
		}

		flags.botl = 1;

		break;

	    case AMULET_OF_DEPRESSION:
		accessory_has_effect(otmp);
		if (u.uluck > 0) u.uluck = -u.uluck;
		if (u.ualign.record > 0) u.ualign.record = -u.ualign.record;

		break;

	    case AMULET_OF_UNCHANGING:
		/* un-change: it's a pun */
		if (!Unchanging && Upolyd) {
		    accessory_has_effect(otmp);
		    makeknown(typ);
		    rehumanize();
		}
		break;
	    case AMULET_OF_STRANGULATION: /* bad idea! */
		/* no message--this gives no permanent effect */
		choke(otmp);
		break;
	    case AMULET_OF_RESTFUL_SLEEP: /* another bad idea! */
	    case RIN_SLEEPING:
		if (!(HSleeping & FROMOUTSIDE))
		    accessory_has_effect(otmp);
		HSleeping = FROMOUTSIDE | rnd(1000);
		break;
	    case AMULET_OF_FUMBLING: /* another bad idea! */
		if (!(HFumbling & FROMOUTSIDE))
		    accessory_has_effect(otmp);
		HFumbling = FROMOUTSIDE | rnd(5);
		incr_itimeout(&HFumbling, rnd(20));
		u.fumbleduration += rnz(1000);
		break;
	    case AMULET_OF_LIFE_SAVING:
		u.extralives++;
		pline("1-UP!");
		break;
	    case AMULET_VERSUS_STONE:
		/* no message--this gives no permanent effect */
		(void)uunstone();
		break;
	    case RIN_DOOM:
		u.uluck = LUCKMIN;
		pline("You start a trip on the road to nowhere.");
		break;
	    case RIN_ELEMENTS:
		accessory_has_effect(otmp);
		HFire_resistance |= FROMOUTSIDE;
		HCold_resistance |= FROMOUTSIDE;
		HShock_resistance |= FROMOUTSIDE;
		break;
	    case RIN_SUSTAIN_ABILITY:
	    /*case AMULET_OF_FLYING:*/ /* Intrinsic flying not supported --ALI */
	    case AMULET_OF_SECOND_CHANCE:
	    /*case AMULET_OF_REFLECTION:*/ /* nice try */
	    /*case AMULET_OF_DRAIN_RESISTANCE:*/
	    /* can't eat Amulet of Yendor or fakes,
	     * and no oc_prop even if you could -3.
	     */
		break;
	  }
	}
}

STATIC_OVL void
eatspecial() /* called after eating non-food */
{
	register struct obj *otmp = victual.piece;

	/* lesshungry wants an occupation to handle choke messages correctly */
	set_occupation(eatfood, "eating non-food", 0);
	lesshungry(victual.nmod);
	occupation = 0;
	victual.piece = (struct obj *)0;
	victual.eating = 0;
	if (otmp->oclass == COIN_CLASS) {
#ifdef GOLDOBJ
		if (carried(otmp))
		    useupall(otmp);
#else
		if (otmp->where == OBJ_FREE)
		    dealloc_obj(otmp);
#endif
		else
		    useupf(otmp, otmp->quan);
		return;
	}
	if (otmp->oclass == POTION_CLASS) {
		otmp->quan++; /* dopotion() does a useup() */
		(void)dopotion(otmp);
	}
	if (otmp->oclass == RING_CLASS || otmp->oclass == AMULET_CLASS || otmp->oclass == IMPLANT_CLASS)
		eataccessory(otmp);
	else if (otmp->otyp == LEATHER_LEASH && otmp->leashmon)
		o_unleash(otmp);
	else if (otmp->otyp == INKA_LEASH && otmp->leashmon)
		o_unleash(otmp);

	/* KMH -- idea by "Tommy the Terrorist" */
	if ((otmp->otyp == TRIDENT) && !otmp->cursed && !(FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone()) )
	{
		pline(FunnyHallu ? "Four out of five dentists agree." :
				"That was pure chewing satisfaction!");
		exercise(A_WIS, TRUE);
	}
	if ((otmp->otyp == FLINT) && !otmp->cursed && !(FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone()) )
	{
		pline(FunnyHallu ? "Whoops, what's that grating sound? Was that a piece of your tooth?" : "Yabba-dabba delicious!");
		exercise(A_CON, TRUE);
	}

	if (otmp->otyp == LUCKSTONE) {
		if (otmp->cursed) {
			change_luck(-1);
			if (otmp->spe < 0) change_luck(otmp->spe);
			You_feel("unlucky.");
		} else if (otmp->blessed && !(u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() )) {
			change_luck(2);
			if (otmp->spe > 0) change_luck(otmp->spe);
			You_feel("very lucky.");
		} else if (!(u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone()) ) {
			change_luck(1);
			if (otmp->spe > 0) change_luck(otmp->spe);
			You_feel("lucky.");
		}
	}

	if (otmp->otyp == HEALTHSTONE) {
		if (Upolyd) {
			u.mh++;
			u.mhmax++;
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		} else {
			u.uhp++;
			u.uhpmax++;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		}
		if (uactivesymbiosis) {
			u.usymbiote.mhpmax++;
			if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
		}
		You_feel("vitalized.");
		flags.botl = 1;
	}

	if (otmp->otyp == MANASTONE) {
		u.uen++;
		u.uenmax++;
		You_feel("a mild buzz.");
		flags.botl = 1;
	}

	if (otmp->otyp == STONE_OF_MAGIC_RESISTANCE && !rn2(10)) {
		if(!(HAntimagic & FROMOUTSIDE)) {
			You_feel("magic-protected.");
			HAntimagic |= FROMOUTSIDE;
		}
	}

	if (otmp->otyp == SLEEPSTONE) {
		if(HSleep_resistance & INTRINSIC) {
			HSleep_resistance &= ~INTRINSIC;
		} 
		if(HSleep_resistance & TIMEOUT) {
			HSleep_resistance &= ~TIMEOUT;
		} 
		You_feel("like you could use a nap.");
		fall_asleep(-rnd(10), TRUE);

	}

	if (otmp->otyp == LOADBOULDER || otmp->otyp == LOADSTONE || otmp->otyp == STARLIGHTSTONE) {
		You_feel("much heavier!");
		IncreasedGravity += otmp->owt;
	}

	if (otmp->otyp >= ELIF_S_JEWEL && otmp->otyp <= DORA_S_JEWEL) {
		pline("The feminine curse spreads through your body...");

		switch (otmp->otyp) {

			case ELIF_S_JEWEL:
				FemaleTrapElif |= FROMOUTSIDE; break;
			case MADELEINE_S_JEWEL:
				FemaleTrapMadeleine |= FROMOUTSIDE; break;
			case SANDRA_S_JEWEL:
				FemaleTrapSandra |= FROMOUTSIDE; break;
			case NADJA_S_JEWEL:
				FemaleTrapNadja |= FROMOUTSIDE; break;
			case SOLVEJG_S_JEWEL:
				FemaleTrapSolvejg |= FROMOUTSIDE; break;
			case THAI_S_JEWEL:
				FemaleTrapThai |= FROMOUTSIDE; break;
			case ELENA_S_JEWEL:
				FemaleTrapElena |= FROMOUTSIDE; break;
			case WENDY_S_JEWEL:
				FemaleTrapWendy |= FROMOUTSIDE; break;
			case ANASTASIA_S_JEWEL:
				FemaleTrapAnastasia |= FROMOUTSIDE; break;
			case JESSICA_S_JEWEL:
				FemaleTrapJessica |= FROMOUTSIDE; break;
			case MARLENA_S_JEWEL:
				FemaleTrapMarlena |= FROMOUTSIDE; break;
			case FEMMY_S_JEWEL:
				FemaleTrapFemmy |= FROMOUTSIDE; break;
			case NATALJE_S_JEWEL:
				FemaleTrapNatalje |= FROMOUTSIDE; break;
			case KARIN_S_JEWEL:
				FemaleTrapKarin |= FROMOUTSIDE; break;
			case JEANETTA_S_JEWEL:
				FemaleTrapJeanetta |= FROMOUTSIDE; break;
			case KATI_S_JEWEL:
				FemaleTrapKati |= FROMOUTSIDE; break;
			case CLAUDIA_S_JEWEL:
				FemaleTrapClaudia |= FROMOUTSIDE; break;
			case VICTORIA_S_JEWEL:
				FemaleTrapVictoria |= FROMOUTSIDE; break;
			case MAURAH_S_JEWEL:
				FemaleTrapMaurah |= FROMOUTSIDE; break;
			case JUEN_S_JEWEL:
				FemaleTrapJuen |= FROMOUTSIDE; break;
			case KRISTINA_S_JEWEL:
				FemaleTrapKristina |= FROMOUTSIDE; break;
			case SARAH_S_JEWEL:
				FemaleTrapSarah |= FROMOUTSIDE; break;
			case KATHARINA_S_JEWEL:
				FemaleTrapKatharina |= FROMOUTSIDE; break;
			case JULIETTA_S_JEWEL:
				FemaleTrapJulietta |= FROMOUTSIDE; break;
			case MELTEM_S_JEWEL:
				FemaleTrapMeltem |= FROMOUTSIDE; break;
			case MELISSA_S_JEWEL:
				FemaleTrapMelissa |= FROMOUTSIDE; break;
			case LUDGERA_S_JEWEL:
				FemaleTrapLudgera |= FROMOUTSIDE; break;
			case YVONNE_S_JEWEL:
				FemaleTrapYvonne |= FROMOUTSIDE; break;
			case EVELINE_S_JEWEL:
				FemaleTrapEveline |= FROMOUTSIDE; break;
			case NELLY_S_JEWEL:
				FemaleTrapNelly |= FROMOUTSIDE; break;
			case ARABELLA_S_JEWEL:
				FemaleTrapArabella |= FROMOUTSIDE; break;
			case RUEA_S_JEWEL:
				FemaleTrapRuea |= FROMOUTSIDE; break;
			case JETTE_S_JEWEL:
				FemaleTrapJette |= FROMOUTSIDE; break;
			case VERENA_S_JEWEL:
				FemaleTrapVerena |= FROMOUTSIDE; break;
			case LOU_S_JEWEL:
				FemaleTrapLou |= FROMOUTSIDE; break;
			case ANITA_S_JEWEL:
				FemaleTrapAnita |= FROMOUTSIDE; break;
			case MARIKE_S_JEWEL:
				FemaleTrapMarike |= FROMOUTSIDE; break;
			case KRISTIN_S_JEWEL:
				FemaleTrapKristin |= FROMOUTSIDE; break;
			case HENRIETTA_S_JEWEL:
				FemaleTrapHenrietta |= FROMOUTSIDE; break;
			case ALMUT_S_JEWEL:
				FemaleTrapAlmut |= FROMOUTSIDE; break;
			case ANNA_S_JEWEL:
				FemaleTrapAnna |= FROMOUTSIDE; break;
			case SING_S_JEWEL:
				FemaleTrapSing |= FROMOUTSIDE; break;
			case INA_S_JEWEL:
				FemaleTrapIna |= FROMOUTSIDE; break;
			case DORA_S_JEWEL:
				FemaleTrapDora |= FROMOUTSIDE; break;

		}
	}

	if (otmp->otyp >= RIGHT_MOUSE_BUTTON_STONE && otmp->otyp <= NASTY_STONE) {
		/* you were stupid and ate a nasty gray stone, so now you have its effect intrinsically. No message. --Amy */

		switch (otmp->otyp) {

			case RIGHT_MOUSE_BUTTON_STONE:
				    RMBLoss |= FROMOUTSIDE; break;
 			case DISPLAY_LOSS_STONE:
				    DisplayLoss |= FROMOUTSIDE; break;
 			case SPELL_LOSS_STONE:
				    SpellLoss |= FROMOUTSIDE; break;
 			case YELLOW_SPELL_STONE:
				    YellowSpells |= FROMOUTSIDE; break;
 			case AUTO_DESTRUCT_STONE:
				    AutoDestruct |= FROMOUTSIDE; break;
 			case MEMORY_LOSS_STONE:
				    MemoryLoss |= FROMOUTSIDE; break;
 			case INVENTORY_LOSS_STONE:
				    InventoryLoss |= FROMOUTSIDE; break;
 			case BLACKY_STONE:
				    BlackNgWalls |= FROMOUTSIDE; break;
 			case MENU_BUG_STONE:
				    MenuBug |= FROMOUTSIDE; break;
 			case SPEEDBUG_STONE:
				    SpeedBug |= FROMOUTSIDE; break;
 			case SUPERSCROLLER_STONE:
				    Superscroller |= FROMOUTSIDE; break;
 			case FREE_HAND_BUG_STONE:
				    FreeHandLoss |= FROMOUTSIDE; break;
 			case UNIDENTIFY_STONE:
				    Unidentify |= FROMOUTSIDE; break;
 			case STONE_OF_THIRST:
				    Thirst |= FROMOUTSIDE; break;
 			case UNLUCKY_STONE:
				    LuckLoss |= FROMOUTSIDE; break;
 			case SHADES_OF_GREY_STONE:
				    ShadesOfGrey |= FROMOUTSIDE; break;
 			case STONE_OF_FAINTING:
				    FaintActive |= FROMOUTSIDE; break;
 			case STONE_OF_CURSING:
				    Itemcursing |= FROMOUTSIDE; break;
 			case STONE_OF_DIFFICULTY:
				    DifficultyIncreased |= FROMOUTSIDE; break;
 			case DEAFNESS_STONE:
				    Deafness |= FROMOUTSIDE; break;
 			case ANTIMAGIC_STONE:
				    CasterProblem |= FROMOUTSIDE; break;
 			case WEAKNESS_STONE:
				    WeaknessProblem |= FROMOUTSIDE; break;
 			case ROT_THIRTEEN_STONE:
				    RotThirteen |= FROMOUTSIDE; break;
 			case BISHOP_STONE:
				    BishopGridbug |= FROMOUTSIDE; break;
 			case CONFUSION_STONE:
				    ConfusionProblem |= FROMOUTSIDE; break;
 			case DROPBUG_STONE:
				    NoDropProblem |= FROMOUTSIDE; break;
 			case DSTW_STONE:
				    DSTWProblem |= FROMOUTSIDE; break;
 			case STATUS_STONE:
				    StatusTrapProblem |= FROMOUTSIDE; break;
 			case ALIGNMENT_STONE:
				    AlignmentProblem |= FROMOUTSIDE; break;
 			case STAIRSTRAP_STONE:
				    StairsProblem |= FROMOUTSIDE; break;
			case UNINFORMATION_STONE:
				    UninformationProblem |= FROMOUTSIDE; break;
			case CAPTCHA_STONE:
				    CaptchaProblem |= FROMOUTSIDE; break;
			case FARLOOK_STONE:
				    FarlookProblem |= FROMOUTSIDE; break;
			case RESPAWN_STONE:
				    RespawnProblem |= FROMOUTSIDE; break;
			case AMNESIA_STONE:
				    RecurringAmnesia |= FROMOUTSIDE; break;
			case BIGSCRIPT_STONE:
				    BigscriptEffect |= FROMOUTSIDE; break;
			case BANK_STONE:
				    BankTrapEffect |= FROMOUTSIDE; break;
			case MAP_STONE:
				    MapTrapEffect |= FROMOUTSIDE; break;
			case TECHNIQUE_STONE:
				    TechTrapEffect |= FROMOUTSIDE; break;
			case DISENCHANTMENT_STONE:
				    RecurringDisenchant |= FROMOUTSIDE; break;
			case VERISIERT_STONE:
				    verisiertEffect |= FROMOUTSIDE; break;
			case CHAOS_TERRAIN_STONE:
				    ChaosTerrain |= FROMOUTSIDE; break;
			case MUTENESS_STONE:
				    Muteness |= FROMOUTSIDE; break;
			case ENGRAVING_STONE:
				    EngravingDoesntWork |= FROMOUTSIDE; break;
			case MAGIC_DEVICE_STONE:
				    MagicDeviceEffect |= FROMOUTSIDE; break;
			case BOOK_STONE:
				    BookTrapEffect |= FROMOUTSIDE; break;
			case LEVEL_STONE:
				    LevelTrapEffect |= FROMOUTSIDE; break;
			case QUIZ_STONE:
				    QuizTrapEffect |= FROMOUTSIDE; break;
			case STONE_OF_INTRINSIC_LOSS:
				    IntrinsicLossProblem |= FROMOUTSIDE; break;
			case BLOOD_LOSS_STONE:
				    BloodLossProblem |= FROMOUTSIDE; break;
			case BAD_EFFECT_STONE:
				    BadEffectProblem |= FROMOUTSIDE; break;
			case TRAP_CREATION_STONE:
				    TrapCreationProblem |= FROMOUTSIDE; break;
			case STONE_OF_VULNERABILITY:
				    AutomaticVulnerabilitiy |= FROMOUTSIDE; break;
			case ITEM_TELEPORTING_STONE:
				    TeleportingItems |= FROMOUTSIDE; break;
			case NASTY_STONE:
				    NastinessProblem |= FROMOUTSIDE; break;
			case METABOLIC_STONE:
				    FastMetabolismEffect |= FROMOUTSIDE; break;
			case STONE_OF_NO_RETURN:
				    NoReturnEffect |= FROMOUTSIDE; break;
			case EGOSTONE:
				    AlwaysEgotypeMonsters |= FROMOUTSIDE; break;
			case FAST_FORWARD_STONE:
				    TimeGoesByFaster |= FROMOUTSIDE; break;
			case ROTTEN_STONE:
				    FoodIsAlwaysRotten |= FROMOUTSIDE; break;
			case UNSKILLED_STONE:
				    AllSkillsUnskilled |= FROMOUTSIDE; break;
			case LOW_STAT_STONE:
				    AllStatsAreLower |= FROMOUTSIDE; break;
			case TRAINING_STONE:
				    PlayerCannotTrainSkills |= FROMOUTSIDE; break;
			case EXERCISE_STONE:
				    PlayerCannotExerciseStats |= FROMOUTSIDE; break;
			case TURN_LIMIT_STONE:
				    TurnLimitation |= FROMOUTSIDE; break;
			case WEAK_SIGHT_STONE:
				    WeakSight |= FROMOUTSIDE; break;
			case CHATTER_STONE:
				    RandomMessages |= FROMOUTSIDE; break;
 			case NONSACRED_STONE:
				    Desecration |= FROMOUTSIDE; break;
 			case STARVATION_STONE:
				    StarvationEffect |= FROMOUTSIDE; break;
 			case DROPLESS_STONE:
				    NoDropsEffect |= FROMOUTSIDE; break;
 			case LOW_EFFECT_STONE:
				    LowEffects |= FROMOUTSIDE; break;
 			case INVISO_STONE:
				    InvisibleTrapsEffect |= FROMOUTSIDE; break;
 			case GHOSTLY_STONE:
				    GhostWorld |= FROMOUTSIDE; break;
 			case DEHYDRATING_STONE:
				    Dehydration |= FROMOUTSIDE; break;
 			case STONE_OF_HATE:
				    HateTrapEffect |= FROMOUTSIDE; break;
 			case DIRECTIONAL_SWAP_STONE:
				    TotterTrapEffect |= FROMOUTSIDE; break;
 			case NONINTRINSICAL_STONE:
				    Nonintrinsics |= FROMOUTSIDE; break;
 			case DROPCURSE_STONE:
				    Dropcurses |= FROMOUTSIDE; break;
 			case STONE_OF_NAKED_STRIPPING:
				    Nakedness |= FROMOUTSIDE; break;
 			case ANTILEVEL_STONE:
				    Antileveling |= FROMOUTSIDE; break;
 			case STEALER_STONE:
				    ItemStealingEffect |= FROMOUTSIDE; break;
 			case REBEL_STONE:
				    Rebellions |= FROMOUTSIDE; break;
 			case SHIT_STONE:
				    CrapEffect |= FROMOUTSIDE; break;
 			case STONE_OF_MISFIRING:
				    ProjectilesMisfire |= FROMOUTSIDE; break;
 			case STONE_OF_PERMANENCE:
				    WallTrapping |= FROMOUTSIDE; break;
			case DISCONNECT_STONE:
				    DisconnectedStairs |= FROMOUTSIDE; break;
			case SCREW_STONE:
				    InterfaceScrewed |= FROMOUTSIDE; break;
			case BOSSFIGHT_STONE:
				    Bossfights |= FROMOUTSIDE; break;
			case ENTIRE_LEVEL_STONE:
				    EntireLevelMode |= FROMOUTSIDE; break;
			case BONE_STONE:
				    BonesLevelChange |= FROMOUTSIDE; break;
			case AUTOCURSE_STONE:
				    AutocursingEquipment |= FROMOUTSIDE; break;
			case HIGHLEVEL_STONE:
				    HighlevelStatus |= FROMOUTSIDE; break;
			case SPELL_MEMORY_STONE:
				    SpellForgetting |= FROMOUTSIDE; break;
			case SOUND_EFFECT_STONE:
				    SoundEffectBug |= FROMOUTSIDE; break;
			case TIME_USE_STONE:
				    TimerunBug |= FROMOUTSIDE; break;
			case LOOTCUT_STONE:
				    LootcutBug |= FROMOUTSIDE; break;
			case MONSTER_SPEED_STONE:
				    MonsterSpeedBug |= FROMOUTSIDE; break;
			case SCALING_STONE:
				    ScalingBug |= FROMOUTSIDE; break;
			case INIMICAL_STONE:
				    EnmityBug |= FROMOUTSIDE; break;
			case WHITE_SPELL_STONE:
				    WhiteSpells |= FROMOUTSIDE; break;
			case GREYOUT_STONE:
				    CompleteGraySpells |= FROMOUTSIDE; break;
			case QUASAR_STONE:
				    QuasarVision |= FROMOUTSIDE; break;
			case MOMMY_STONE:
				    MommaBugEffect |= FROMOUTSIDE; break;
			case HORROR_STONE:
				    HorrorBugEffect |= FROMOUTSIDE; break;
			case ARTIFICIAL_STONE:
				    ArtificerBug |= FROMOUTSIDE; break;
			case WEREFORM_STONE:
				    WereformBug |= FROMOUTSIDE; break;
			case ANTIPRAYER_STONE:
				    NonprayerBug |= FROMOUTSIDE; break;
			case EVIL_PATCH_STONE:
				    EvilPatchEffect |= FROMOUTSIDE; break;
			case HARD_MODE_STONE:
				    HardModeEffect |= FROMOUTSIDE; break;
			case SECRET_ATTACK_STONE:
				    SecretAttackBug |= FROMOUTSIDE; break;
			case EATER_STONE:
				    EaterBugEffect |= FROMOUTSIDE; break;
			case COVETOUS_STONE:
				    CovetousnessBug |= FROMOUTSIDE; break;
			case NON_SEEING_STONE:
				    NotSeenBug |= FROMOUTSIDE; break;
			case DARKMODE_STONE:
				    DarkModeBug |= FROMOUTSIDE; break;
			case UNFINDABLE_STONE:
				    AntisearchEffect |= FROMOUTSIDE; break;
			case HOMICIDE_STONE:
				    HomicideEffect |= FROMOUTSIDE; break;
			case MULTITRAPPING_STONE:
				    NastynationBug |= FROMOUTSIDE; break;
			case WAKEUP_CALL_STONE:
				    WakeupCallBug |= FROMOUTSIDE; break;
			case GRAYOUT_STONE:
				    GrayoutBug |= FROMOUTSIDE; break;
			case GRAY_CENTER_STONE:
				    GrayCenterBug |= FROMOUTSIDE; break;
			case CHECKERBOARD_STONE:
				    CheckerboardBug |= FROMOUTSIDE; break;
			case CLOCKWISE_STONE:
				    ClockwiseSpinBug |= FROMOUTSIDE; break;
			case COUNTERCLOCKWISE_STONE:
				    CounterclockwiseSpin |= FROMOUTSIDE; break;
			case LAG_STONE:
				    LagBugEffect |= FROMOUTSIDE; break;
			case BLESSCURSE_STONE:
				    BlesscurseEffect |= FROMOUTSIDE; break;
			case DELIGHT_STONE:
				    DeLightBug |= FROMOUTSIDE; break;
			case DISCHARGE_STONE:
				    DischargeBug |= FROMOUTSIDE; break;
			case TRASH_STONE:
				    TrashingBugEffect |= FROMOUTSIDE; break;
			case FILTERING_STONE:
				    FilteringBug |= FROMOUTSIDE; break;
			case DEFORMATTING_STONE:
				    DeformattingBug |= FROMOUTSIDE; break;
			case FLICKER_STRIP_STONE:
				    FlickerStripBug |= FROMOUTSIDE; break;
			case UNDRESSING_STONE:
				    UndressingEffect |= FROMOUTSIDE; break;
			case HYPER_BLUE_STONE:
				    Hyperbluewalls |= FROMOUTSIDE; break;
			case NO_LIGHT_STONE:
				    NoliteBug |= FROMOUTSIDE; break;
			case PARANOIA_STONE:
				    ParanoiaBugEffect |= FROMOUTSIDE; break;
			case FLEECE_STONE:
				    FleecescriptBug |= FROMOUTSIDE; break;
			case INTERRUPTION_STONE:
				    InterruptEffect |= FROMOUTSIDE; break;
			case DUSTBIN_STONE:
				    DustbinBug |= FROMOUTSIDE; break;
			case BATTERY_STONE:
				    ManaBatteryBug |= FROMOUTSIDE; break;
			case BUTTERFINGER_STONE:
				    Monsterfingers |= FROMOUTSIDE; break;
			case MISCASTING_STONE:
				    MiscastBug |= FROMOUTSIDE; break;
			case MESSAGE_SUPPRESSION_STONE:
				    MessageSuppression |= FROMOUTSIDE; break;
			case STUCK_ANNOUNCEMENT_STONE:
				    StuckAnnouncement |= FROMOUTSIDE; break;
			case STORM_STONE:
				    BloodthirstyEffect |= FROMOUTSIDE; break;
			case MAXIMUM_DAMAGE_STONE:
				    MaximumDamageBug |= FROMOUTSIDE; break;
			case LATENCY_STONE:
				    LatencyBugEffect |= FROMOUTSIDE; break;
			case STARLIT_SKY_STONE:
				    StarlitBug |= FROMOUTSIDE; break;
			case TRAP_KNOWLEDGE_STONE:
				    KnowledgeBug |= FROMOUTSIDE; break;
			case HIGHSCORE_STONE:
				    HighscoreBug |= FROMOUTSIDE; break;
			case PINK_SPELL_STONE:
				    PinkSpells |= FROMOUTSIDE; break;
			case GREEN_SPELL_STONE:
				    GreenSpells |= FROMOUTSIDE; break;
			case EVC_STONE:
				    EvencoreEffect |= FROMOUTSIDE; break;
			case UNDERLAID_STONE:
				    UnderlayerBug |= FROMOUTSIDE; break;
			case DAMAGE_METER_STONE:
				    DamageMeterBug |= FROMOUTSIDE; break;
			case WEIGHT_STONE:
				    ArbitraryWeightBug |= FROMOUTSIDE; break;
			case INFOFUCK_STONE:
				    FuckedInfoBug |= FROMOUTSIDE; break;
			case BLACK_SPELL_STONE:
				    BlackSpells |= FROMOUTSIDE; break;
			case CYAN_SPELL_STONE:
				    CyanSpells |= FROMOUTSIDE; break;
			case HEAP_STONE:
				    HeapEffectBug |= FROMOUTSIDE; break;
			case BLUE_SPELL_STONE:
				    BlueSpells |= FROMOUTSIDE; break;
			case TRON_STONE:
				    TronEffect |= FROMOUTSIDE; break;
			case RED_SPELL_STONE:
				    RedSpells |= FROMOUTSIDE; break;
			case TOO_HEAVY_STONE:
				    TooHeavyEffect |= FROMOUTSIDE; break;
			case ELONGATED_STONE:
				    ElongationBug |= FROMOUTSIDE; break;
			case WRAPOVER_STONE:
				    WrapoverEffect |= FROMOUTSIDE; break;
			case DESTRUCTION_STONE:
				    DestructionEffect |= FROMOUTSIDE; break;
			case MELEE_PREFIX_STONE:
				    MeleePrefixBug |= FROMOUTSIDE; break;
			case AUTOMORE_STONE:
				    AutomoreBug |= FROMOUTSIDE; break;
			case UNFAIR_ATTACK_STONE:
				    UnfairAttackBug |= FROMOUTSIDE; break;
			case ORANGE_SPELL_STONE:
			    OrangeSpells |= FROMOUTSIDE; break;
			case VIOLET_SPELL_STONE:
			    VioletSpells |= FROMOUTSIDE; break;
			case LONGING_STONE:
			    LongingEffect |= FROMOUTSIDE; break;
			case CURSED_PART_STONE:
			    CursedParts |= FROMOUTSIDE; break;
			case QUAVERSAL_STONE:
			    Quaversal |= FROMOUTSIDE; break;
			case APPEARANCE_SHUFFLING_STONE:
			    AppearanceShuffling |= FROMOUTSIDE; break;
			case BROWN_SPELL_STONE:
			    BrownSpells |= FROMOUTSIDE; break;
			case CHOICELESS_STONE:
			    Choicelessness |= FROMOUTSIDE; break;
			case GOLDSPELL_STONE:
			    Goldspells |= FROMOUTSIDE; break;
			case DEPROVEMENT_STONE:
			    Deprovement |= FROMOUTSIDE; break;
			case INITIALIZATION_STONE:
			    InitializationFail |= FROMOUTSIDE; break;
			case GUSHLUSH_STONE:
			    GushlushEffect |= FROMOUTSIDE; break;
			case SOILTYPE_STONE:
			    SoiltypeEffect |= FROMOUTSIDE; break;
			case DANGEROUS_TERRAIN_STONE:
			    DangerousTerrains |= FROMOUTSIDE; break;
			case FALLOUT_STONE:
			    FalloutEffect |= FROMOUTSIDE; break;
			case MOJIBAKE_STONE:
			    MojibakeEffect |= FROMOUTSIDE; break;
			case GRAVATION_STONE:
			    GravationEffect |= FROMOUTSIDE; break;
			case UNCALLED_STONE:
			    UncalledEffect |= FROMOUTSIDE; break;
			case EXPLODING_DICE_STONE:
			    ExplodingDiceEffect |= FROMOUTSIDE; break;
			case PERMACURSE_STONE:
			    PermacurseEffect |= FROMOUTSIDE; break;
			case SHROUDED_IDENTITY_STONE:
			    ShroudedIdentity |= FROMOUTSIDE; break;
			case FEELER_GAUGE_STONE:
			    FeelerGauges |= FROMOUTSIDE; break;
			case LONG_SCREWUP_STONE:
			    LongScrewup |= FROMOUTSIDE; break;
			case WING_YELLOW_STONE:
			    WingYellowChange |= FROMOUTSIDE; break;
			case LIFE_SAVING_STONE:
			    LifeSavingBug |= FROMOUTSIDE; break;
			case CURSEUSE_STONE:
			    CurseuseEffect |= FROMOUTSIDE; break;
			case CUT_NUTRITION_STONE:
			    CutNutritionEffect |= FROMOUTSIDE; break;
			case SKILL_LOSS_STONE:
			    SkillLossEffect |= FROMOUTSIDE; break;
			case AUTOPILOT_STONE:
			    AutopilotEffect |= FROMOUTSIDE; break;
			case FORCE_STONE:
			    MysteriousForceActive |= FROMOUTSIDE; break;
			case MONSTER_GLYPH_STONE:
			    MonsterGlyphChange |= FROMOUTSIDE; break;
			case CHANGING_DIRECTIVE_STONE:
			    ChangingDirectives |= FROMOUTSIDE; break;
			case CONTAINER_KABOOM_STONE:
			    ContainerKaboom |= FROMOUTSIDE; break;
			case STEAL_DEGRADE_STONE:
			    StealDegrading |= FROMOUTSIDE; break;
			case LEFT_INVENTORY_STONE:
			    LeftInventoryBug |= FROMOUTSIDE; break;
			case FLUCTUATING_SPEED_STONE:
			    FluctuatingSpeed |= FROMOUTSIDE; break;
			case TARMUSTROKINGNORA_STONE:
			    TarmuStrokingNora |= FROMOUTSIDE; break;
			case FAILURE_STONE:
			    FailureEffects |= FROMOUTSIDE; break;
			case BRIGHT_CYAN_SPELL_STONE:
			    BrightCyanSpells |= FROMOUTSIDE; break;
			case FREQUENTATION_SPAWN_STONE:
			    FrequentationSpawns |= FROMOUTSIDE; break;
			case PET_AI_STONE:
			    PetAIScrewed |= FROMOUTSIDE; break;
			case SATAN_STONE:
			    SatanEffect |= FROMOUTSIDE; break;
			case REMEMBERANCE_STONE:
			    RememberanceEffect |= FROMOUTSIDE; break;
			case POKELIE_STONE:
			    PokelieEffect |= FROMOUTSIDE; break;
			case AUTOPICKUP_STONE:
			    AlwaysAutopickup |= FROMOUTSIDE; break;
			case DYWYPI_STONE:
			    DywypiProblem |= FROMOUTSIDE; break;
			case SILVER_SPELL_STONE:
			    SilverSpells |= FROMOUTSIDE; break;
			case METAL_SPELL_STONE:
			    MetalSpells |= FROMOUTSIDE; break;
			case PLATINUM_SPELL_STONE:
			    PlatinumSpells |= FROMOUTSIDE; break;
			case MANLER_STONE:
			    ManlerEffect |= FROMOUTSIDE; break;
			case DOORNING_STONE:
			    DoorningEffect |= FROMOUTSIDE; break;
			case NOWNSIBLE_STONE:
			    NownsibleEffect |= FROMOUTSIDE; break;
			case ELM_STREET_STONE:
			    ElmStreetEffect |= FROMOUTSIDE; break;
			case MONNOISE_STONE:
			    MonnoiseEffect |= FROMOUTSIDE; break;
			case RANG_CALL_STONE:
			    RangCallEffect |= FROMOUTSIDE; break;
			case RECURRING_SPELL_LOSS_STONE:
			    RecurringSpellLoss |= FROMOUTSIDE; break;
			case ANTI_TRAINING_STONE:
			    AntitrainingEffect |= FROMOUTSIDE; break;
			case TECHOUT_STONE:
			    TechoutBug |= FROMOUTSIDE; break;
			case STAT_DECAY_STONE:
			    StatDecay |= FROMOUTSIDE; break;
			case MOVEMORK_STONE:
			    Movemork |= FROMOUTSIDE; break;
			case BAD_PART_STONE:
			    BadPartBug |= FROMOUTSIDE; break;
			case COMPLETELY_BAD_PART_STONE:
			    CompletelyBadPartBug |= FROMOUTSIDE; break;
			case EVIL_VARIANT_STONE:
			    EvilVariantActive |= FROMOUTSIDE; break;
			case SANE_TREBLE_STONE:
			    SanityTrebleEffect |= FROMOUTSIDE; break;
			case STATCREASE_STONE:
			    StatDecreaseBug |= FROMOUTSIDE; break;
			case SIMEOUT_STONE:
			    SimeoutBug |= FROMOUTSIDE; break;

		}
	}

	if (Race_if(PM_WORM_THAT_WALKS) && otmp->otyp == FIGURINE) {/* chance to polymorph into the depicted monster --Amy */
		if (rn2(5)) {

			 if (polyok(&mons[otmp->corpsenm])) {
			u.wormpolymorph = otmp->corpsenm;
			polyself(FALSE);
			}
		} else polyself(FALSE);
	}
	if (Race_if(PM_WORM_THAT_WALKS) && otmp->otyp == ENERGY_SAP) {
		if (rn2(5)) {

			 if (polyok(&mons[otmp->corpsenm])) {
			u.wormpolymorph = otmp->corpsenm;
			polyself(FALSE);
			}
		} else polyself(FALSE);
	}
	if (Race_if(PM_WORM_THAT_WALKS) && otmp->otyp == STATUE) {
		if (rn2(5)) {
			 if (polyok(&mons[otmp->corpsenm])) {
			u.wormpolymorph = otmp->corpsenm;
			polyself(FALSE);
			}
		} else polyself(FALSE);
	}

	if (Race_if(PM_WARPER) && otmp->otyp == FIGURINE) {/* chance to polymorph into the depicted monster --Amy */
		if (!rn2(5)) {

			u.wormpolymorph = otmp->corpsenm;
			polyself(FALSE);

		}
	}
	if (Race_if(PM_WARPER) && otmp->otyp == STATUE) {
		if (!rn2(5)) {

			u.wormpolymorph = otmp->corpsenm;
			polyself(FALSE);

		}
	}

	if (otmp == uwep && otmp->quan == 1L) uwepgone();
	if (otmp == uquiver && otmp->quan == 1L) uqwepgone();
	if (otmp == uswapwep && otmp->quan == 1L) uswapwepgone();

	if (otmp == uball) unpunish();
	if (otmp == uchain) unpunish(); /* but no useup() */
	else if (carried(otmp)) useup(otmp);
	else useupf(otmp, 1L);
}

/* NOTE: the order of these words exactly corresponds to the
   order of oc_material values #define'd in objclass.h. */
/* Amy grepping target: "materialeffect" */
static const char *foodwords[] = {
	"meal", "liquid", "wax", "food", "meat",
	"paper", "cloth", "leather", "wood", "bone", "scale",
	"iron", "metal", "copper", "silver", "gold", "platinum", "mithril",
	"plastic", "glass", "rich food", "stone",
	"fissile metal", "elasthan", "bitumen", "silk",
	"arcanium", "secretion", "poor food", "compost",
	"eternium", "contamination", "brick wall", "sand",
	"shadow material", "volcanic glass", "lead", "chrome",
	"porcelain", "celestial cloth", "conundrum",
	"bubbles", "meteoric steel", "antidote", "nanomachines",
	"foam", "scrap metal", "alloy", "lametta", "amber",
};

STATIC_OVL const char *
foodword(otmp)
register struct obj *otmp;
{
	if (otmp->oclass == FOOD_CLASS) return "food";
	if (otmp->oclass == GEM_CLASS &&
	    objects[otmp->otyp].oc_material == MT_GLASS &&
	    otmp->dknown)
		makeknown(otmp->otyp);
	return foodwords[objects[otmp->otyp].oc_material];
}

STATIC_OVL void
fpostfx(otmp)		/* called after consuming (non-corpse) food */
register struct obj *otmp;
{
	char buf[BUFSZ];

	switch(otmp->otyp) {
	    case SPRIG_OF_WOLFSBANE:
		if (u.ulycn >= LOW_PM || is_were(youmonst.data) || Race_if(PM_HUMAN_WEREWOLF) || Race_if(PM_AK_THIEF_IS_DEAD_) || Role_if(PM_LUNATIC) )
		    you_unwere(TRUE);
		break;

	    case CHOCOLATE:
	    case CHOCOEGG:

		switch (rnd(16)) {

			case 1:	/* blueberry (cure blind) */
				/* They're good for your eyes, too. [Sakusha]*/
				pline("It was blueberry-flavored!");
				make_blinded((long)u.ucreamed,TRUE);
				break;
			case 2:	/* bitter (cure stun) */
				pline("It tasted bitter!");
				make_stunned(0L,TRUE);
				break;
			case 3:	/* wasabi (cure confuse) */
				pline("It contained all-natural wasabi extract!");
				make_confused(0L,TRUE);
				break;
			case 4:	/* mattya (cure slow and totter) */
				pline("There was some weird japanese food extras in there...");
				make_frozen(0L, TRUE);
				u.totter = 0;
				break;
			case 5:	/* ginger (cure hallucinate) */
				pline("Ginger! Who puts that in a chocolate???");
				(void) make_hallucinated(0L, TRUE, 0L);
				break;
			default: break;

		}

		if (otmp->oartifact == ART_SCHOKOLADE_EEA) {
			pline("Auughh!");
			Deafness += rnz(500);
			flags.soundok = 0;
		}

		if (otmp->oartifact == ART_CHRISTMAS_EGG) {
			You("ate the first christmas egg.");
			incr_itimeout(&HHalf_physical_damage, rn1(1000, 1000));

		}

		break;

	    case HOLY_WAFER:            
		if (u.ualign.type == A_LAWFUL) {
			if (u.uhp < u.uhpmax) {
				You_feel("warm inside.");
				u.uhp += rn1(20,20);
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			} 
		}
		if (Sick) make_sick(0L, (char *)0, TRUE, SICK_ALL);
		if (u.ulycn != -1) {
		    you_unwere(TRUE);
		}
		if (u.ualign.type == A_CHAOTIC) {
		    You_feel("a burning inside!");
		    u.uhp -= rn1(10,10);
		    /* KMH, balance patch 2 -- should not have 0 hp */
		    if (u.uhp < 1) u.uhp = 1;
		}
		break;
	    case CARROT:
		make_blinded((long)u.ucreamed,TRUE);
		break;

		/* Adding more uses for certain food types. --Amy */

	    case BANANA:
		(void) make_hallucinated(0L, TRUE, 0L);
		break;

	    case MELON:
		make_confused(0L, TRUE);
		break;

	    case ORANGE:
		make_frozen(0L, TRUE);
		break;

	    case APPLE:
		make_numbed(0L, TRUE);
		break;

	    case LEMON:
		make_feared(0L, TRUE);
		break;

	    case CREAM_PIE:
		make_burned(0L, TRUE);
		make_dimmed(0L, TRUE);
		break;

	    case PEAR:
		make_stunned(0L, TRUE);
		break;

	    case CHERRY:
		if (PlayerBleeds) {
			PlayerBleeds = 0;
			Your("bleeding stops.");
		}
		break;

	    case ASIAN_PEAR:
		make_confused(0L, TRUE);
		make_stunned(0L, TRUE);
		break;

	    case MAGIC_BANANA: /* cure all */
		make_confused(0L, TRUE);
		make_stunned(0L, TRUE);
		make_burned(0L, TRUE);
		make_dimmed(0L, TRUE);
		make_feared(0L, TRUE);
		make_numbed(0L, TRUE);
		make_frozen(0L, TRUE);
		(void) make_hallucinated(0L, TRUE, 0L);
		make_blinded((long)u.ucreamed,TRUE);

		break;

	    /* body parts -- now checks for artifact and name*/
	    case EYEBALL:
		if (!otmp->oartifact) break;
		You_feel("a burning inside!");
		u.uhp -= rn1(50,150);
		if (u.uhp <= 0) {
		  u.youaredead = 1;
		  killer_format = KILLED_BY;
		  killer = food_xname(otmp, TRUE);
		  done(CHOKING);
		  u.youaredead = 0;
		}
		break;

	    case UGH_MEMORY_TO_CREATE_INVENTORY:

		/*u.wormpolymorph = (NUMMONS + rnd(MISSINGNORANGE));
		u.ughmemory = 1;
		polyself(FALSE);*/
		pline("Effect deactivated. Sorry.");

		break;

	    case SEVERED_HAND:
		if (!otmp->oartifact) break;
		You_feel("the hand scrabbling around inside of you!");
		u.uhp -= rn1(50,150);
		if (u.uhp <= 0) {
		  u.youaredead = 1;
		  killer_format = KILLED_BY;
		  killer = food_xname(otmp, TRUE);
		  done(CHOKING);
		  u.youaredead = 0;
		}
		break;
	    case FORTUNE_COOKIE:

		if (rn2(2)) {	/* sometimes change the player's luck --Amy */

			You_feel("your luck is turning...");
	
			if (bcsign(otmp) == -1)	change_luck(-1);
			else if (bcsign(otmp) == 0) change_luck(rn2(2) ? -1 : 1);
			else if (bcsign(otmp) == 1) change_luck(1);

		}

		getlin ("Read the fortune? [y/yes/no]",buf);
		(void) lcase (buf);
		if (!strcmp (buf, "yes") || !strcmp (buf, "y")) {

			/* reading it might influence your luck --Amy */

			if (bcsign(otmp) == -1)	change_luck(-1);
			else if (rn2(2) && bcsign(otmp) == 0) change_luck(1);
			else if (bcsign(otmp) == 1) change_luck(1);

			outrumor(bcsign(otmp), BY_COOKIE, FALSE);
			if (!Blind) u.uconduct.literate++;
		}
		break;
/* STEHPEN WHITE'S NEW CODE */            
	    case LUMP_OF_ROYAL_JELLY:
		/* This stuff seems to be VERY healthy! */
		gainstr(otmp, 1);
		if (Upolyd) {
		    u.mh += (otmp->cursed || FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone()) ? -rnd(20) : rnd(20);
		    if (u.mh > u.mhmax) {
			if (!rn2(17)) u.mhmax++;
			u.mh = u.mhmax;
		    } else if (u.mh <= 0) {
			rehumanize();
		    }
		} else {
		    u.uhp += (otmp->cursed || FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone()) ? -rnd(20) : rnd(20);
		    if (u.uhp > u.uhpmax) {
			if(!rn2(17)) u.uhpmax++;
			u.uhp = u.uhpmax;
		    } else if(u.uhp <= 0) {
			u.youaredead = 1;
			killer_format = KILLED_BY_AN;
			killer = "rotten lump of royal jelly";
			done(POISONING);
			u.youaredead = 0;
		    }
			if (uactivesymbiosis && !rn2(17)) {
				u.usymbiote.mhpmax++;
				if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
			}
		}
		if(!otmp->cursed && !(FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone()) ) heal_legs();
		break;
	    case EGG:

		if (otmp->corpsenm == PM_EASTER) {
			pline("Wow, you found an easter egg!");
			if (getmonth() == 3) pline("Happy Easter!");
		}

		/* eating your own eggs is bad luck --Amy */
		if (otmp->spe && otmp->corpsenm >= LOW_PM) {
				pline(FunnyHallu ? "Great taste, like what chewing your own nails is like!" : "How are your children, you sorry excuse for a parent?");
			    change_luck(-2);
		}

		if (touch_petrifies(&mons[otmp->corpsenm]) && otmp->corpsenm != PM_PLAYERMON) {
		    if ((!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) &&
			!(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) {
			if (!Stoned) {
				if (Hallucination && rn2(10)) pline("Good thing you are already stoned.");
				else {
					You("start turning to stone.");
					Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
					u.cnd_stoningcount++;
					killer_format = KILLED_BY_AN;
					sprintf(killer_buf, "petrifying egg (%s)", mons[otmp->corpsenm].mname);
					delayed_killer = killer_buf;
				}
			}
		    }
		}
		break;
	    case EUCALYPTUS_LEAF:
		if (Sick && !otmp->cursed && !(FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone()) )
		    make_sick(0L, (char *)0, TRUE, SICK_ALL);
		if (Vomiting && !(uarmf && uarmf->oartifact == ART_CRUEL_GODDESS_ANA) && !otmp->cursed && !(FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone()) )
		    make_vomiting(0L, TRUE);
		break;
	}

	if (!rn2(25)) { /* more random fortunes --Amy */

		getlin ("Somehow, a strip of paper appeared in your food! Read it? [y/yes/no]",buf);
		(void) lcase (buf);
		if (!strcmp (buf, "yes") || !strcmp (buf, "y")) {

		/* reading it will influence the player's luck --Amy */

		if (bcsign(otmp) == -1)	change_luck(-1);
		else if (rn2(2) && bcsign(otmp) == 0) change_luck(1);
		else if (bcsign(otmp) == 1) change_luck(1);

		outrumor(bcsign(otmp), BY_PAPER, FALSE);
		if (!Blind) u.uconduct.literate++;
		}
	}

	return;
}
/*
 * return 0 if the food was not dangerous.
 * return 1 if the food was dangerous and you chose to stop.
 * return 2 if the food was dangerous and you chose to eat it anyway.
 */
STATIC_OVL int
edibility_prompts(otmp)
struct obj *otmp;
{
	/* blessed food detection granted you a one-use
	   ability to detect food that is unfit for consumption
	   or dangerous and avoid it. */

	char buf[BUFSZ], foodsmell[BUFSZ],
	     it_or_they[QBUFSZ], eat_it_anyway[QBUFSZ];
	boolean cadaver = (otmp->otyp == CORPSE),
		stoneorslime = FALSE;
	int material = objects[otmp->otyp].oc_material,
	    mnum = otmp->corpsenm;
	long rotted = 0L;

	strcpy(foodsmell, Tobjnam(otmp, "smell"));
	strcpy(it_or_they, (otmp->quan == 1L) ? "it" : "they");
	sprintf(eat_it_anyway, "Eat %s anyway?",
		(otmp->quan == 1L) ? "it" : "one");

	if (cadaver || otmp->otyp == EGG || otmp->otyp == TIN) {
		/* These checks must match those in eatcorpse() */
	  	stoneorslime = (touch_petrifies(&mons[mnum]) && mnum != PM_PLAYERMON &&
				!Stone_resistance && !poly_when_stoned(youmonst.data));

		if (slime_on_touch(&mons[mnum]))
		    stoneorslime = (!Unchanging && !flaming(youmonst.data) && !slime_on_touch(youmonst.data) );
            if (eating_is_fatal(&mons[mnum])) /* not is_rider - we want to catch a certain 'b' too --Amy */
                    stoneorslime = TRUE; 

		if (cadaver && !nocorpsedecay(&mons[mnum]) ) {
			long age = peek_at_iced_corpse_age(otmp);
			/* worst case rather than random
			   in this calculation to force prompt */
			rotted = (monstermoves - age)/(10L + 0 /* was rn2(20) */);
			if (FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone()) rotted += 2L;
			if (otmp->cursed) rotted += 2L;
			else if (otmp->blessed) rotted -= 2L;
		}
	}

	/*
	 * These problems with food should be checked in
	 * order from most detrimental to least detrimental.
	 */

	if (cadaver && !acidic(&mons[mnum]) && rotted > 5L && !(IntSick_resistance || (ExtSick_resistance && rn2(20))) ) {
		/* Tainted meat */
		sprintf(buf, "%s like %s could be tainted! %s",
			foodsmell, it_or_they, eat_it_anyway);
		if (yn_function(buf,ynchars,'n')=='n') return 1;
		else return 2;
	}
	if (eating_is_fatal(&mons[mnum])) {
		sprintf(buf, "%s like %s could kill you outright! %s",
			foodsmell, it_or_they, eat_it_anyway);
		if (yn_function(buf,ynchars,'n')=='n') return 1;
		else return 2;
	}

	if (stoneorslime) {
		sprintf(buf, "%s like %s could be something very dangerous! %s",
			foodsmell, it_or_they, eat_it_anyway);
		if (yn_function(buf,ynchars,'n')=='n') return 1;
		else return 2;
	}

	if (dmgtype(&mons[mnum], AD_DREA)) {
		sprintf(buf, "%s like %s could cause nightmares! %s",
			foodsmell, it_or_they, eat_it_anyway);
		if (yn_function(buf,ynchars,'n')=='n') return 1;
		else return 2;
	}

	if (is_bat(&mons[mnum])) {
		sprintf(buf, "%s like %s could make you batty! %s",
			foodsmell, it_or_they, eat_it_anyway);
		if (yn_function(buf,ynchars,'n')=='n') return 1;
		else return 2;
	}

	if (dmgtype(&mons[mnum], AD_NAST)) {
		sprintf(buf, "%s like %s could be rather nasty! %s",
			foodsmell, it_or_they, eat_it_anyway);
		if (yn_function(buf,ynchars,'n')=='n') return 1;
		else return 2;
	}

	if (dmgtype(&mons[mnum], AD_BADE)) {
		sprintf(buf, "%s like %s could be bad for you! %s",
			foodsmell, it_or_they, eat_it_anyway);
		if (yn_function(buf,ynchars,'n')=='n') return 1;
		else return 2;
	}

	if (dmgtype(&mons[mnum], AD_RBAD)) {
		sprintf(buf, "%s like %s could be really bad for you! %s",
			foodsmell, it_or_they, eat_it_anyway);
		if (yn_function(buf,ynchars,'n')=='n') return 1;
		else return 2;
	}

	if (otmp->orotten || (cadaver && rotted > 3L)) {
		/* Rotten */
		sprintf(buf, "%s like %s could be rotten! %s",
			foodsmell, it_or_they, eat_it_anyway);
		if (yn_function(buf,ynchars,'n')=='n') return 1;
		else return 2;
	}
	if (cadaver && poisonous(&mons[mnum]) && !Poison_resistance) {
		/* poisonous */
		sprintf(buf, "%s like %s might be poisonous! %s",
			foodsmell, it_or_they, eat_it_anyway);
		if (yn_function(buf,ynchars,'n')=='n') return 1;
		else return 2;
	}
	if (cadaver && !vegetarian(&mons[mnum]) &&
	    !u.uconduct.unvegetarian && (Role_if(PM_MONK) || Role_if(PM_HALF_BAKED) || have_anorexia() || Role_if(PM_FAILED_EXISTENCE) || Race_if(PM_SYLPH) ) ) {
		sprintf(buf, "%s unhealthy. %s",
			foodsmell, eat_it_anyway);
		if (yn_function(buf,ynchars,'n')=='n') return 1;
		else return 2;
	}
	if (cadaver && acidic(&mons[mnum]) && !Acid_resistance) {
		sprintf(buf, "%s rather acidic. %s",
			foodsmell, eat_it_anyway);
		if (yn_function(buf,ynchars,'n')=='n') return 1;
		else return 2;
	}

	if (dmgtype(&mons[mnum], AD_STUN) || dmgtype(&mons[mnum], AD_HALU) || 
		    mnum == PM_VIOLET_FUNGUS || mnum == PM_VIOLET_STALK || mnum == PM_VIOLET_SPORE || mnum == PM_VIOLET_COLONY) { 
		sprintf(buf, "%s like %s may be %s! %s", 
			foodsmell, it_or_they, 
			FunnyHallu ? "some real hard stuff" 
			: "hallucinogenic", eat_it_anyway); 
		if (yn_function(buf,ynchars,'n')=='n') return 1; 
		/*adjalign(-sgn(u.ualign.type));*/ /* you took it knowingly */ /*what the heck??? --Amy */
		return 2; 
	} 
	if (is_were(&mons[mnum]) && u.ulycn != mnum) { 
		sprintf(buf, "%s like %s might be diseased. %s", 
			foodsmell, it_or_they, eat_it_anyway); 
		if (yn_function(buf,ynchars,'n')=='n') return 1; 
		else return 2; 
	} 

	/*
	 * Breaks conduct, but otherwise safe.
	 */
	 
	if (!u.uconduct.unvegan &&
	    ((material == MT_LEATHER || material == MT_BONE ||
	      otmp->otyp == EYEBALL || otmp->otyp == SEVERED_HAND ||
	      material == MT_DRAGON_HIDE || material == MT_WAX) ||
	     (cadaver && !vegan(&mons[mnum])))) {
		sprintf(buf, "%s foul and unfamiliar to you. %s",
			foodsmell, eat_it_anyway);
		if (yn_function(buf,ynchars,'n')=='n') return 1;
		else return 2;
	}
	if (!u.uconduct.unvegetarian &&
	    ((material == MT_LEATHER || material == MT_BONE ||
	      otmp->otyp == EYEBALL || otmp->otyp == SEVERED_HAND ||
	      material == MT_DRAGON_HIDE) ||
	     (cadaver && !vegetarian(&mons[mnum])))) {
		sprintf(buf, "%s unfamiliar to you. %s",
			foodsmell, eat_it_anyway);
		if (yn_function(buf,ynchars,'n')=='n') return 1;
		else return 2;
	}

	if (cadaver && !acidic(&mons[mnum]) && rotted > 5L && Sick_resistance) {
		/* Tainted meat with Sick_resistance */
		sprintf(buf, "%s like %s could be tainted! %s",
			foodsmell, it_or_they, eat_it_anyway);
		if (yn_function(buf,ynchars,'n')=='n') return 1;
		else return 2;
	}
	return 0;
}

int
doeat()		/* generic "eat" command funtion (see cmd.c) */
{
	register struct obj *otmp;
	int basenutrit;			/* nutrition of full item */
	int nutrit;			/* nutrition available */
	char qbuf[QBUFSZ];
	char c;
	
	boolean dont_start = FALSE;
	if (Strangled) {
		pline(FunnyHallu ? "You just can't seem to get it down your throat - is it too big?!" : "If you can't breathe air, how can you consume solids?");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	}
	if (uarmh && (uarmh->otyp == PLASTEEL_HELM || uarmh->otyp == HELM_OF_NO_DIGESTION || uarmh->otyp == HELM_OF_STORMS || uarmh->otyp == HELM_OF_DETECT_MONSTERS) ){
		pline("The %s covers your whole face.", xname(uarmh));
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	}
	if (!(otmp = floorfood("eat"))) return 0;
	if (check_capacity((char *)0)) return 0;

	if (u.urealedibility || Role_if(PM_COOK) ) {
		int res = edibility_prompts(otmp);
		if (res) {
		    if (u.urealedibility) {u.urealedibility -= 1;
		    if (u.urealedibility == 0) Your("%s stops tingling and your sense of smell returns to normal.",
			body_part(NOSE));
			}
		    if (res == 1) return 0;
		}
	}

	/* We have to make non-foods take 1 move to eat, unless we want to
	 * do ridiculous amounts of coding to deal with partly eaten plate
	 * mails, players who polymorph back to human in the middle of their
	 * metallic meal, etc....
	 */
	if (!is_edible(otmp)) {
	    You("cannot eat that!");
	    return 0;
	} else if ((otmp->owornmask & (W_ARMOR|W_TOOL|W_AMUL|W_IMPLANT|W_SADDLE)) != 0) {
	    /* let them eat rings */
	    You_cant("eat %s you're wearing.", something);
	    return 0;
	}
	if (otmp->otyp == EYEBALL || otmp->otyp == SEVERED_HAND) {
	    strcpy(qbuf,"Are you sure you want to eat that?");
	    if ((c = yn_function(qbuf, ynqchars, 'n')) != 'y') return 0;
	}
	if (otmp->otyp == UGH_MEMORY_TO_CREATE_INVENTORY) {
	    strcpy(qbuf,"Did you read this item's description? Do you really want to eat it?");
	    if ((c = yn_function(qbuf, ynqchars, 'n')) != 'y') return 0;
	}
	if ((otmp->oclass != FOOD_CLASS || InventoryDoesNotGo) && flags.eatingconfirm) {
	    strcpy(qbuf,"Really eat that item?");
	    if ((c = yn_function(qbuf, ynqchars, 'n')) != 'y') return 0;
	}

	/* KMH -- Slow digestion is... indigestible */
	if (otmp->otyp == RIN_SLOW_DIGESTION) {
		pline("This ring is indigestible!");
		(void) rottenfood(otmp);
		if (otmp->dknown && !objects[otmp->otyp].oc_name_known
				&& !objects[otmp->otyp].oc_uname)
			docall(otmp);
		return (1);
	}

	if ( (Race_if(PM_ARMED_COCKATRICE) || Race_if(PM_CORTEX)) && !lithivorous(youmonst.data) && !Upolyd && (otmp->otyp != TIN) && (otmp->otyp != CORPSE) && rn2(2) ) {

		if (rn2(2)) {
			pline("Ulch - that food was petrified! You're unable to swallow it.");
			badeffect();
		} else pline("The food turns to stone as you try to eat it!");

		if (otmp == uwep && otmp->quan == 1L) uwepgone();
		if (otmp == uquiver && otmp->quan == 1L) uqwepgone();
		if (otmp == uswapwep && otmp->quan == 1L) uswapwepgone();
	
		if (otmp == uball) unpunish();
		if (otmp == uchain) unpunish(); /* but no useup() */
		else if (carried(otmp)) useup(otmp);
		else useupf(otmp, 1L);
		return 1;
	}

	if ( (Race_if(PM_ARMED_COCKATRICE) || Race_if(PM_CORTEX)) && !lithivorous(youmonst.data) && !Upolyd && (otmp->otyp != TIN) && (otmp->otyp != CORPSE) && rn2(2) ) {

		pline("Ulch - that food was petrified!");
		badeffect();

	}

	if ( (Race_if(PM_ARMED_COCKATRICE) || Race_if(PM_CORTEX)) && lithivorous(youmonst.data) && !Upolyd && (otmp->otyp != TIN) && (otmp->otyp != CORPSE) && !rn2(4) ) {

		if (rn2(2)) {
			pline("Ulch - that petrified food had manky bits! You're unable to swallow it.");
			badeffect();
		} else pline("The petrified food appears to be manky as you try to eat it!");

		if (otmp == uwep && otmp->quan == 1L) uwepgone();
		if (otmp == uquiver && otmp->quan == 1L) uqwepgone();
		if (otmp == uswapwep && otmp->quan == 1L) uswapwepgone();
	
		if (otmp == uball) unpunish();
		if (otmp == uchain) unpunish(); /* but no useup() */
		else if (carried(otmp)) useup(otmp);
		else useupf(otmp, 1L);
		return 1;
	}

	if ( (Race_if(PM_ARMED_COCKATRICE) || Race_if(PM_CORTEX)) && lithivorous(youmonst.data) && !Upolyd && (otmp->otyp != TIN) && (otmp->otyp != CORPSE) && !rn2(4) ) {

		pline("Ulch - that petrified food had manky bits!");
		badeffect();

	}

	if (otmp->oclass != FOOD_CLASS) {
	    int material;
	    victual.reqtime = 1;
	    victual.piece = otmp;
		/* Don't split it, we don't need to if it's 1 move */
	    victual.usedtime = 0;
	    victual.canchoke = (u.uhs == SATIATED);
		/* Note: gold weighs 1 pt. for each 1000 pieces (see */
		/* pickup.c) so gold and non-gold is consistent. */
	    if (otmp->oclass == COIN_CLASS)
		basenutrit = ((otmp->quan > 20000L) ? 2000
			: (int)(otmp->quan/10L));
	    else if(otmp->oclass == BALL_CLASS || otmp->oclass == CHAIN_CLASS)
		basenutrit = weight(otmp);
	    /* oc_nutrition is usually weight anyway */
	    else basenutrit = objects[otmp->otyp].oc_nutrition;

		/* Let's throw metallivores a bone here, considering most metal items weigh less now. --Amy
		   also lithivores, which have to eat low-weight rocks and gems. */

		if (otmp->oclass != COIN_CLASS && (u.uhunger < 2500) ) {
			if (basenutrit && basenutrit < 500) {basenutrit *= 3;}
			if (basenutrit && basenutrit < 10) {basenutrit = 10;}
			if (basenutrit < 3) {basenutrit = 3;}
		}

		if (otmp->oartifact == ART_PAWNERMASTER) {
			makemon(&mons[PM_GYPSY], u.ux, u.uy, MM_ADJACENTOK);
		}

	    victual.nmod = basenutrit;
	    victual.eating = TRUE; /* needed for lesshungry() */

	    material = objects[otmp->otyp].oc_material;
	    if (material == MT_LEATHER ||
		otmp->otyp == EYEBALL || otmp->otyp == SEVERED_HAND ||
		material == MT_BONE || material == MT_DRAGON_HIDE) {
	 		u.uconduct.unvegan++;
	    		violated_vegetarian();
	    } else if (material == MT_WAX)
			u.uconduct.unvegan++;
	    u.uconduct.food++;
		gluttonous();

	    
	    if (otmp->cursed || FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone())
		(void) rottenfood(otmp);

	    if (otmp->oclass == WEAPON_CLASS && otmp->opoisoned) {
		pline(FunnyHallu ? "Urgh - that tastes like cactus juice with full-length thorns in it!" : "Ecch - that must have been poisonous!");
		if(!Poison_resistance) {
		    if (!rn2(2)) losestr(rnd(2), TRUE);
		    if (!rn2(15)) losestr(rnd(2), TRUE);
		    if (issoviet) {
				losestr(rnd(5), TRUE);
				pline("On, on, ty otravilsya i, veroyatno, poteryal silu! Nay khe khe!");
		    }
		    losehp(rnd(15), xname(otmp), KILLED_BY_AN);
		} else
		    You("seem unaffected by the poison.");
	    } else if (!otmp->cursed && material != MT_SECREE && material != MT_ETHER && !(FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone()) ) {
		pline("This %s is delicious!",
		      otmp->oclass == COIN_CLASS ? foodword(otmp) :
		      singular(otmp, xname));
	    }

	    if (material == MT_SECREE) {
			pline("Ulch - this %s tastes like secretion!", otmp->oclass == COIN_CLASS ? foodword(otmp) : singular(otmp, xname));
			badeffect();
	    }
	    if (material == MT_ETHER) {
			pline("The contamination spreads through your body.");
			contaminate(rnz((level_difficulty() + 40) * 5), TRUE);
	    }
	    if (material == MT_ANTIDOTIUM) {
			pline("It was a covid-19 antidote!");
			upnivel(TRUE);
	    }
	    if (material == MT_VIVA) {
			pline("Eating radioactive metal is a bad idea.");
			ABASE(A_STR)--;
			ABASE(A_DEX)--;
			ABASE(A_CON)--;
			ABASE(A_WIS)--;
			ABASE(A_INT)--;
			ABASE(A_CHA)--;
			if(ABASE(A_STR) < ATTRMIN(A_STR)) {losehp(rnd(15), "eating radioactive food", KILLED_BY); ABASE(A_STR) = ATTRMIN(A_STR);}
			if(ABASE(A_DEX) < ATTRMIN(A_DEX)) {losehp(rnd(15), "eating radioactive food", KILLED_BY); ABASE(A_DEX) = ATTRMIN(A_DEX);}
			if(ABASE(A_CON) < ATTRMIN(A_CON)) {losehp(rnd(15), "eating radioactive food", KILLED_BY); ABASE(A_CON) = ATTRMIN(A_CON);}
			if(ABASE(A_WIS) < ATTRMIN(A_WIS)) {losehp(rnd(15), "eating radioactive food", KILLED_BY); ABASE(A_WIS) = ATTRMIN(A_WIS);}
			if(ABASE(A_INT) < ATTRMIN(A_INT)) {losehp(rnd(15), "eating radioactive food", KILLED_BY); ABASE(A_INT) = ATTRMIN(A_INT);}
			if(ABASE(A_CHA) < ATTRMIN(A_CHA)) {losehp(rnd(15), "eating radioactive food", KILLED_BY); ABASE(A_CHA) = ATTRMIN(A_CHA);}
	    }

	    if (otmp->otyp == PETRIFYIUM_BAR) {
		if (!Stoned && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && !(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM)) ) {
			if (Hallucination && rn2(10)) pline("Good thing you are already stoned.");
			else {
				You("start turning to stone.");
				Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
				u.cnd_stoningcount++;
				stop_occupation();
				delayed_killer = "eating a petrifying weapon";
			}
		}

	    }

	    if (otmp->otyp == PETRIFYIUM_BRA) {
		if (!Stoned && (!Stone_resistance || (!IntStone_resistance && !rn2(20)) ) && !(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM)) ) {
			if (Hallucination && rn2(10)) pline("Good thing you are already stoned.");
			else {
				You("start turning to stone.");
				Stoned = Race_if(PM_EROSATOR) ? 3 : 7;
				u.cnd_stoningcount++;
				stop_occupation();
				delayed_killer = "eating a petrifying bra";
			}
		}

	    }

	    if (material == MT_INKA) {
			pline("Urgh... your %s is turning as it's having difficulties digesting inka leather.", body_part(STOMACH));
			nomul(-20, "trying to digest an inka object", TRUE);
			/* This ignores free action. --Amy */
	    }

	    if (practicantterror && material == MT_SILK) {
			pline("%s booms: 'You insolent maggot! You're worse than a goddamn cat, eating valuable clothing like that! I'll punish you by forcing you to bring me 20 arrows, you have ten minutes to comply or you'll also pay a bunch of money.'", noroelaname());
			fineforpracticant(0, 0, 20);
	    }

	    eatspecial();

	    return 1;
	}

	/* [ALI] Hero polymorphed in the meantime.
	 */
	if (otmp == victual.piece &&
	  (is_vampire(youmonst.data) || Role_if(PM_GOFF) ) != otmp->odrained)
	    victual.piece = (struct obj *)0;	/* Can't resume */

	/* [ALI] Blood can coagulate during the interruption
	 *       but not during the draining process.
	 */
	if(otmp == victual.piece && otmp->odrained &&
	  (peek_at_iced_corpse_age(otmp) + victual.usedtime + 5) < monstermoves)
	    victual.piece = (struct obj *)0;	/* Can't resume */

	if(otmp == victual.piece) {
	/* If they weren't able to choke, they don't suddenly become able to
	 * choke just because they were interrupted.  On the other hand, if
	 * they were able to choke before, if they lost food it's possible
	 * they shouldn't be able to choke now.
	 */
	    if (u.uhs != SATIATED) victual.canchoke = FALSE;
	    victual.piece = touchfood(otmp);
	    You("resume your meal.");
	    start_eating(victual.piece);
	    return(1);
	}

	/* nothing in progress - so try to find something. */
	/* tins are a special case */
	/* tins must also check conduct separately in case they're discarded */
	if(otmp->otyp == TIN) {
	    start_tin(otmp);
	    return(1);
	}

	/* KMH, conduct */
	u.uconduct.food++;

	victual.piece = otmp = touchfood(otmp);
	victual.usedtime = 0;

	/* Now we need to calculate delay and nutritional info.
	 * The base nutrition calculated here and in eatcorpse() accounts
	 * for normal vs. rotten food.  The reqtime and nutrit values are
	 * then adjusted in accordance with the amount of food left.
	 */
	if(otmp->otyp == CORPSE) {
	    int tmp = eatcorpse(otmp);
	    if (tmp == 3) {
		/* inedible */
		victual.piece = (struct obj *)0;
		/*
		 * The combination of odrained == TRUE and oeaten == cnutrit
		 * represents the case of starting to drain a corpse but not
		 * getting any further (eg., loosing consciousness due to
		 * rotten food). We must preserve this case to avoid corpses
		 * changing appearance after a failed attempt to eat.
		 */
		if (!otmp->odrained &&
			otmp->oeaten == mons[otmp->corpsenm].cnutrit)
		    otmp->oeaten = 0;
		/* ALI, conduct: didn't eat it after all */
		u.uconduct.food--;
		return /*0*/1; /* This needs to consume a turn. Otherwise, gastly players might
		* repeatedly try to eat an oldish corpse, since the degree of rot gets calculated for each attempt.
		* Now, this is still possible, but at least the player will lose a turn if the corpse isn't old enough. --Amy*/
	    } else if (tmp == 2) {
		/* used up */
		victual.piece = (struct obj *)0;
		return(1);
	    } else if (tmp)
		dont_start = TRUE;
	    /* if not used up, eatcorpse sets up reqtime and may modify
	     * oeaten */
	} else {
	    /* No checks for WAX, LEATHER, BONE, DRAGON_HIDE.  These are
	     * all handled in the != FOOD_CLASS case, above */
	    switch (objects[otmp->otyp].oc_material) {
	    case MT_FLESH:
		u.uconduct.unvegan++;
		gluttonous();
		if (otmp->otyp != EGG && otmp->otyp != CHEESE) {
		    violated_vegetarian();
		}
		break;

	    default:
		gluttonous();
		if (otmp->otyp == PANCAKE ||
		    otmp->otyp == FORTUNE_COOKIE || /* eggs */
		    otmp->otyp == CREAM_PIE ||
		    otmp->otyp == CHOCOLATE ||
		    otmp->otyp == CHOCOEGG ||
		    otmp->otyp == SLICE_OF_PIZZA ||
		    otmp->otyp == PIZZA ||
		    otmp->otyp == LUNCH_OF_BOILED_EGG ||
		    otmp->otyp == X_MAS_CAKE ||
		    otmp->otyp == BUNNY_CAKE ||
		    otmp->otyp == PARFAIT ||
		    otmp->otyp == CANDY_BAR || /* milk */
		    otmp->otyp == LUMP_OF_ROYAL_JELLY)
		    u.uconduct.unvegan++;
		break;
	    }

	    victual.reqtime = objects[otmp->otyp].oc_delay;
	    if (otmp->otyp != FORTUNE_COOKIE &&
		(otmp->cursed || FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone() ||
		 (((monstermoves - otmp->age) > (int) otmp->blessed ? 50:30) &&
		(otmp->orotten || (issoviet && !rn2(5)) || (!rn2(20) && !otmp->blessed) )))) {

		if (rottenfood(otmp)) {
		    otmp->orotten = TRUE;
		    dont_start = TRUE;
		}
		if (otmp->oeaten < 2) {
		    victual.piece = (struct obj *)0;
		    if (carried(otmp)) useup(otmp);
		    else useupf(otmp, 1L);
		    return 1;
		} else
		consume_oeaten(otmp, 1);	/* oeaten >>= 1 */
	    } else fprefx(otmp);
	}

	if (otmp->otyp == EGG && otmp->oartifact == ART_EGG_OF_SPLAT) {
	    pline("Ulch - that was a contaminated egg!");
	    make_sick(rn1(25,25), "Team Splat egg (Go Team Hardcore Autism!)", TRUE, SICK_VOMITABLE);
	}

	if (practicantterror && (otmp->otyp == SLICE_OF_PIZZA || otmp->otyp == PIZZA)) {
		pline("%s booms: 'You're not allowed to eat pizza in my lab! That makes 50 zorkmids, maggot.'", noroelaname());
		fineforpracticant(50, 0, 0);
	}

	/* re-calc the nutrition */
	if (otmp->otyp == CORPSE) basenutrit = mons[otmp->corpsenm].cnutrit;
	else basenutrit = objects[otmp->otyp].oc_nutrition;
	nutrit = otmp->oeaten;
	if (otmp->otyp == CORPSE && otmp->odrained) {
	    nutrit -= drainlevel(otmp);
	    basenutrit -= drainlevel(otmp);
	}

#ifdef DEBUG
	debugpline("before rounddiv: victual.reqtime == %d", victual.reqtime);
	debugpline("oeaten == %d, basenutrit == %d", otmp->oeaten, basenutrit);
	debugpline("nutrit == %d, cnutrit == %d", nutrit, otmp->otyp == CORPSE ?
	  mons[otmp->corpsenm].cnutrit : objects[otmp->otyp].oc_nutrition);
#endif
	victual.reqtime = (basenutrit == 0 ? 0 :
		rounddiv(victual.reqtime * (long)nutrit, basenutrit));
#ifdef DEBUG
	debugpline("after rounddiv: victual.reqtime == %d", victual.reqtime);
#endif
	/* calculate the modulo value (nutrit. units per round eating)
	 * [ALI] Note: although this is not exact, the remainder is
	 *       now dealt with in done_eating().
	 */
	if (victual.reqtime == 0 || nutrit == 0)
	    /* possible if most has been eaten before */
	    victual.nmod = 0;
	else if (nutrit >= victual.reqtime)
	    victual.nmod = -(nutrit / victual.reqtime);
	else
	    victual.nmod = victual.reqtime % nutrit;
	victual.canchoke = (u.uhs == SATIATED);

	if (!dont_start) start_eating(otmp);
	return(1);
}

/* Take a single bite from a piece of food, checking for choking and
 * modifying usedtime.  Returns 1 if they choked and survived, 0 otherwise.
 */
STATIC_OVL int
bite()
{
	int vampirenutrition = 0;
	if ( (is_vampire(youmonst.data) || (Role_if(PM_GOFF) && !Upolyd) ) && (u.uhunger < 2500) ) vampirenutrition += rn2(6);
	if ( (saprovorousnumber(u.umonnum) || (Race_if(PM_GASTLY) && !Upolyd) || (Race_if(PM_PLAYER_SKELETON) && !Upolyd) || (Race_if(PM_PHANTOM_GHOST) && !Upolyd) ) && (u.uhunger < 2500) ) vampirenutrition += rn2(3);

	if(victual.canchoke && u.uhunger >= 5000) { /* allowing players to eat more --Amy */
		choke(victual.piece);
		return 1;
	}
	if (victual.doreset) {
		do_reset_eat();
		return 0;
	}
	force_save_hs = TRUE;
	if(victual.nmod < 0) {
		lesshungry(-victual.nmod);
		if (vampirenutrition) lesshungry(-(vampirenutrition * victual.nmod) );
		consume_oeaten(victual.piece, victual.nmod); /* -= -nmod */
	} else if(victual.nmod > 0 && (victual.usedtime % victual.nmod)) {
		lesshungry(1);
		consume_oeaten(victual.piece, -1);		  /* -= 1 */
	}
	force_save_hs = FALSE;
	recalc_wt();
	return 0;
}

#endif /* OVLB */
#ifdef OVL0

void
gethungry()	/* as time goes by - called by moveloop() and domove() */
{
	if (u.uinvulnerable) return;	/* you don't feel hungrier */

	if ( (have_anorexia() || Role_if(PM_FAILED_EXISTENCE)) && ( (rn2(2) && u.uhs == HUNGRY) || (rn2(4) && u.uhs == WEAK) || (rn2(8) && u.uhs == FAINTING) || (rn2(16) && u.uhs == FAINTED) ) ) return; /* They are used to eating very little. --Amy */

	if ((!u.usleep || !rn2(10))	/* slow metabolic rate while asleep */
		&& (carnivorous(youmonst.data) || herbivorous(youmonst.data) || metallivorous(youmonst.data) || lithivorous(youmonst.data))
        /* Convicts can last twice as long at hungry and below */
        && (!Role_if(PM_CONVICT) || (rn2(2)) || (u.uhs < HUNGRY))
		&& !Slow_digestion && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500) )
	    u.uhunger--;		/* ordinary food consumption */

	/* cloak of slow digestion is not supposed to be cloak of no digestion --Amy */
	if (uarmc && uarmc->otyp == CLOAK_OF_SLOW_DIGESTION && !rn2(10) && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500)) u.uhunger--;

	if (u.uprops[FAST_METABOLISM].extrinsic && !(StrongSlow_digestion && rn2(3)) && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500)) u.uhunger--;
	if (have_metabolicstone() && !(StrongSlow_digestion && rn2(3)) && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500)) u.uhunger--;
	if (FastMetabolismEffect) {

		int extrahungerpoints;

		if (!(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongSlow_digestion && rn2(3)) ) u.uhunger--;

		extrahungerpoints = FastMetabolismEffect;
		if (extrahungerpoints >= 67108864) extrahungerpoints -= 67108864;
		if (extrahungerpoints >= 33554432) extrahungerpoints -= 33554432;
		if (extrahungerpoints >= 16777216) extrahungerpoints -= 16777216;
		extrahungerpoints /= 5000;
		if (extrahungerpoints > 0 && !(StrongSlow_digestion && rn2(3)) && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500)) u.uhunger -= extrahungerpoints;
	}
	if (uarmc && itemhasappearance(uarmc, APP_AVENGER_CLOAK) && !(StrongSlow_digestion && rn2(3)) && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500)) u.uhunger -= 2;

	/* ancipital's slow digestion is not supposed to be no digestion --Amy */
	if (Race_if(PM_ANCIPITAL) && !rn2(20) && !(StrongSlow_digestion && rn2(3)) && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500)) u.uhunger--;

	/* having a symbiote uses your nutrition to feed it, especially if it has HP regeneration --Amy
	 * regenerating ones sap more nutrition if you're less skilled */
	if (uactivesymbiosis && !(StrongSlow_digestion && rn2(3)) && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500)) {
		if (carnivorous(&mons[u.usymbiote.mnum]) || herbivorous(&mons[u.usymbiote.mnum]) || metallivorous(&mons[u.usymbiote.mnum]) || organivorous(&mons[u.usymbiote.mnum]) || lithivorous(&mons[u.usymbiote.mnum])) u.uhunger--;
		if (regenerates(&mons[u.usymbiote.mnum])) {

			if ((u.uhunger >= 2500) || PlayerCannotUseSkills || (P_SKILL(P_SYMBIOSIS) == P_ISRESTRICTED)) {
				u.uhunger -= 5;
			} else switch (P_SKILL(P_SYMBIOSIS)) {
		      	case P_BASIC:	u.uhunger -= (2 + rn2(2)); break;
		      	case P_SKILLED:	u.uhunger -= 2; break;
		      	case P_EXPERT:	u.uhunger -= rnd(2); break;
		      	case P_MASTER:	u.uhunger--; break;
		      	case P_GRAND_MASTER:	if (!rn2(2)) u.uhunger--; break;
		      	case P_SUPREME_MASTER:	if (!rn2(3)) u.uhunger--; break;
		      	default: u.uhunger -= 3; break;
			}
		}
	}

	if (!rn2(2)) {	/* used to be odd turns, but nasty traps that speed up turncount exist --Amy */
	    /* Regeneration uses up food, unless due to an artifact */
	    /*if (HRegeneration || ((ERegeneration & (~W_ART)) &&
				(ERegeneration != W_WEP || !uwep->oartifact)))
			u.uhunger--;*/

		/* rewrite by Amy */
		if (Regeneration && !(StrongSlow_digestion && rn2(3)) && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500)) u.uhunger--;
		if (StrongRegeneration && !(StrongSlow_digestion && rn2(3)) && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500)) u.uhunger--;
		if (Energy_regeneration && !(StrongSlow_digestion && rn2(3)) && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500)) { u.uhunger--; u.uhunger--;}
		if (StrongEnergy_regeneration && !(StrongSlow_digestion && rn2(3)) && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500)) { u.uhunger--; u.uhunger--;}

	    if (near_capacity() > SLT_ENCUMBER && !(StrongSlow_digestion && rn2(3)) && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500)) u.uhunger--;
	} else {		/* even turns */
	    if (Hunger && !(StrongSlow_digestion && rn2(3)) && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500)) u.uhunger--;
	    if (StrongHunger && !(StrongSlow_digestion && rn2(3)) && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500)) u.uhunger--;
		if (Race_if(PM_CLOCKWORK_AUTOMATON) && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500)) u.uhunger--; /* to prevent =oSD from being overpowered --Amy */
	    if (uleft && uleft->oartifact == ART_RING_OF_FAST_LIVING && !(StrongSlow_digestion && rn2(3)) && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500)) { u.uhunger--; u.uhunger--; u.uhunger--; }
	    if (uright && uright->oartifact == ART_RING_OF_FAST_LIVING && !(StrongSlow_digestion && rn2(3)) && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500)) { u.uhunger--; u.uhunger--; u.uhunger--; }
	    /* Conflict uses up food too */
		/* and a lot of it because conflict is so overpowered --Amy */
	    if (Conflict && !(StrongSlow_digestion && rn2(3)) && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500)) { u.uhunger--; u.uhunger--; u.uhunger--; u.uhunger--; u.uhunger--; }
	    if (StrongConflict && !(StrongSlow_digestion && rn2(3)) && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500)) { u.uhunger--; u.uhunger--; u.uhunger--; u.uhunger--; u.uhunger--; }
	    if (uwep && uwep->oartifact == ART_TENSA_ZANGETSU && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500)) u.uhunger -= 10;
	    if (uwep && uwep->oartifact == ART_ZANKAI_HUNG_ZE_TUNG_DO_HAI && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500)) u.uhunger -= 10;
	    if (uwep && uwep->oartifact == ART_GARNET_ROD && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500)) u.uhunger -= 3;
	    if (u.twoweap && uswapwep && uswapwep->oartifact == ART_TENSA_ZANGETSU && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500)) u.uhunger -= 10;
	    if (u.twoweap && uswapwep && uswapwep->oartifact == ART_ZANKAI_HUNG_ZE_TUNG_DO_HAI && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500)) u.uhunger -= 10;
	    if (u.twoweap && uswapwep && uswapwep->oartifact == ART_GARNET_ROD && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500)) u.uhunger -= 3;
	    /* +0 charged rings don't do anything, so don't affect hunger */
	    /* Slow digestion still uses ring hunger */
	    switch (rnd(10)) {	/* used to depend on turn count too... */
	     case  4: if (uleft &&
			  (uleft->spe || !objects[uleft->otyp].oc_charged) && !(StrongSlow_digestion && rn2(3)) && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500))
			    u.uhunger--;
		    break;
	     case  8: if (uamul && !(StrongSlow_digestion && rn2(3)) && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500)) u.uhunger--;
		    break;
	     case 12: if (uright &&
			  (uright->spe || !objects[uright->otyp].oc_charged) && !(StrongSlow_digestion && rn2(3)) && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500))
			    u.uhunger--;
		    break;
	     case 16: if (u.uhave.amulet && !u.freeplaymode && (u.amuletcompletelyimbued || !rn2(5)) && !(StrongSlow_digestion && rn2(3)) && !(Full_nutrient && !rn2(2) && u.uhunger < 2500) && !(StrongFull_nutrient && !rn2(2) && u.uhunger < 2500)) u.uhunger--;
		    break;
	     default: break;
	    }
	}
	newuhs(TRUE);
}

#endif /* OVL0 */
#ifdef OVLB

void
morehungry(num)	/* called after vomiting and after performing feats of magic */
register int num;
{
	if (Full_nutrient && num == 1 && rn2(2) && u.uhunger < 2500) num = 0;
	if (StrongFull_nutrient && num == 1 && rn2(2) && u.uhunger < 2500) num = 0;
	if (Full_nutrient && num > 1 && u.uhunger < 2500) num /= 2;
	if (StrongFull_nutrient && num > 1 && u.uhunger < 2500) num /= 2;

	if (num < 0 && (CutNutritionEffect || u.uprops[CUT_NUTRITION].extrinsic || have_cutnutritionstone() || (uwep && uwep->oartifact == ART_HAVANA_NERO) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_HAVANA_NERO) ) ) num /= 3;

	u.uhunger -= num;
	newuhs(TRUE);
}


void
lesshungry(num)	/* called after eating (and after drinking fruit juice) */
register int num;
{
	/* See comments in newuhs() for discussion on force_save_hs */
	boolean iseating = occupation == eatfood || force_save_hs;
#ifdef DEBUG
	debugpline("lesshungry(%d)", num);
#endif
	if (num > 0 && (CutNutritionEffect || u.uprops[CUT_NUTRITION].extrinsic || have_cutnutritionstone() || (uwep && uwep->oartifact == ART_HAVANA_NERO) || (u.twoweap && uswapwep && uswapwep->oartifact == ART_HAVANA_NERO) ) ) num /= 3;

	if (Race_if(PM_GERTEUT)) {
		num *= 4;
		num /= 3;
	}

	if (Role_if(PM_ANACHRONIST) && (uarmh && itemhasappearance(uarmh, APP_ANACHRO_HELMET))) {
		num *= 6;
		num /= 5;
	}

	if (Race_if(PM_SERB)) {
		num *= 3;
		num /= 2;
	}

	/* eating heals symbiote a bit --Amy */
	if (num >= 100 && uactivesymbiosis) {
		if (u.usymbiote.mhp < u.usymbiote.mhpmax) {
			u.usymbiote.mhp += (num / 100);
			if (u.usymbiote.mhp > u.usymbiote.mhpmax) u.usymbiote.mhp = u.usymbiote.mhpmax;
			if (flags.showsymbiotehp) flags.botl = TRUE;
		}

	}

	u.uhunger += num;
	if(u.uhunger >= 5000) {
	    if (!iseating || victual.canchoke) {
		if (iseating) {
		    choke(victual.piece);
		    reset_eat();
		} else
		    choke(occupation == opentin ? tin.tin : (struct obj *)0);
		/* no reset_eat() */
	    }
	} else {
	    /* Have lesshungry() report when you're nearly full so all eating
	     * warns when you're about to choke.
	     */
	    if (u.uhunger >= 4500) {
		if (!victual.eating || (victual.eating && !victual.fullwarn)) {
		    pline(FunnyHallu ? "You feel like taking the All-You-Can-Eat challenge." : "You're having a hard time getting all of it down.");
		    nomovemsg = "You're finally finished.";
		    if (!victual.eating)
			multi = -2;
		    else {
			victual.fullwarn = TRUE;
			if (victual.canchoke && victual.reqtime > 1) {
			    /* a one-gulp food will not survive a stop */
			    if (yn_function("Stop eating?",ynchars,'y')=='y') {
				reset_eat();
				nomovemsg = (char *)0;
			    }
			}
		    }
		}
	    }

		/* In Soviet Russia, food is so scarce, people will gladly overeat themselves to death. After all, it's only
		 * the dumb people who want to eat a black dragon corpse while satiated, and no dumb people live in Russia.
		 * So it's completely fine if everything always gives full nutrition. --Amy */

	    else if (!issoviet && (u.uhs == SATIATED)) { /* less nutrition if already satiated --Amy */
		int xtranut = num;
		xtranut /= 3;
		if (u.uhunger >= 3250) {xtranut *= 3; xtranut /= 2;}
		if (u.uhunger >= 4000) {xtranut *= 7; xtranut /= 6;}
		u.uhunger -= xtranut;
	    }
	    else if (!issoviet && (u.uhs != SATIATED)) { /* extra nutrition because corpses are more rare now --Amy */
		int xtranut = num;
		xtranut *= rnd((u.uhs >= FAINTING) ? 100 : (u.uhs == WEAK) ? 85 : (u.uhs == HUNGRY) ? 70 : 1);
		xtranut /= 100;
		u.uhunger += xtranut;
	    }
	}
	newuhs(FALSE);
}

STATIC_PTR
int
unfaint()
{
	(void) Hear_again();
	if(u.uhs > FAINTING)
		u.uhs = FAINTING;
	stop_occupation();
	flags.botl = 1;
	return 0;
}

#endif /* OVLB */
#ifdef OVL0

boolean
is_fainted()
{
	return((boolean)(u.uhs == FAINTED));
}

void
reset_faint()	/* call when a faint must be prematurely terminated */
{
	if(is_fainted()) nomul(0, 0, FALSE);
}

#if 0
void
sync_hunger()
{
	if(is_fainted()) {
		flags.soundok = 0;
		nomul(-10+(u.uhunger/10), "fainted from lack of food", TRUE);
		nomovemsg = "You regain consciousness.";
		afternmv = unfaint;
	}
}
#endif

void
newuhs(incr)		/* compute and comment on your (new?) hunger status */
boolean incr;
{
	unsigned newhs;
	static unsigned save_hs;
	static boolean saved_hs = FALSE;
	int h = u.uhunger;

	newhs = (h > 2500) ? SATIATED : /* used to be 1000 --Amy */
		(h > 500) ? NOT_HUNGRY :
		(h > 200) ? HUNGRY :
		(h > 0) ? WEAK : FAINTING;

	/* While you're eating, you may pass from WEAK to HUNGRY to NOT_HUNGRY.
	 * This should not produce the message "you only feel hungry now";
	 * that message should only appear if HUNGRY is an endpoint.  Therefore
	 * we check to see if we're in the middle of eating.  If so, we save
	 * the first hunger status, and at the end of eating we decide what
	 * message to print based on the _entire_ meal, not on each little bit.
	 */
	/* It is normally possible to check if you are in the middle of a meal
	 * by checking occupation == eatfood, but there is one special case:
	 * start_eating() can call bite() for your first bite before it
	 * sets the occupation.
	 * Anyone who wants to get that case to work _without_ an ugly static
	 * force_save_hs variable, feel free.
	 */
	/* Note: If you become a certain hunger status in the middle of the
	 * meal, and still have that same status at the end of the meal,
	 * this will incorrectly print the associated message at the end of
	 * the meal instead of the middle.  Such a case is currently
	 * impossible, but could become possible if a message for SATIATED
	 * were added or if HUNGRY and WEAK were separated by a big enough
	 * gap to fit two bites.
	 */
	if (occupation == eatfood || force_save_hs) {
		if (!saved_hs) {
			save_hs = u.uhs;
			saved_hs = TRUE;
		}
		u.uhs = newhs;
		return;
	} else {
		if (saved_hs) {
			u.uhs = save_hs;
			saved_hs = FALSE;
		}
	}

	if(newhs == FAINTING) {
		if(is_fainted()) newhs = FAINTED;
		if(u.uhs <= WEAK || rn2(50-u.uhunger/50) >= 49) {
			if(!is_fainted() && multi >= 0 /* %% */) {
				/* stop what you're doing, then faint */
				stop_occupation();
				You(FunnyHallu ? "pass out due to those damn munchies." : (have_anorexia()) ? "enter a state of trance." : "faint from lack of food.");

	/* warn player if starvation will happen soon, that is, less than 200 nutrition remaining --Amy */
			if(u.uhunger < -(int)(800 + 50*ACURR(A_CON))) { You(FunnyHallu ? "sense the Grim Reaper approaching." : (have_anorexia()) ? "sense that you're getting closer to your deity." : "are close to starvation.");
		if (have_anorexia()) adjalign(5);
		}
				flags.soundok = 0;
				nomul(-3+(u.uhunger/200), "fainted from lack of food", TRUE);
				nomovemsg = "You regain consciousness.";
				afternmv = unfaint;
				newhs = FAINTED;
				if (have_anorexia()) adjalign(1);
			}
		} else
		if(u.uhunger < -(int)(1000 + 50*ACURR(A_CON))) {
			u.youaredead = 1;
			u.uhs = STARVED;
			flags.botl = 1;
			bot();
			You(FunnyHallu ? "are taken away by the grim reaper..." : (have_anorexia()) ? "meet your deity at last." : "die from starvation.");
			killer_format = KILLED_BY;
			killer = "starvation";
			done(STARVING);
			u.youaredead = 0;
			/* if we return, we lifesaved, and that calls newuhs */
			return;
		}
	}

	if(newhs != u.uhs) {
		if(newhs >= WEAK && u.uhs < WEAK) {
			losestr(1, FALSE);	/* this may kill you -- see below */
			if (SustainAbilityOn) u.weakcheat++; /* cheater! */
			}
		else if(newhs < WEAK && u.uhs >= WEAK) {
			if (!u.weakcheat) losestr(-1, TRUE); /* otherwise this could be exploited until you have 25 str --Amy */
			else u.weakcheat--;
			}
		switch(newhs){
		case HUNGRY:
			if (FunnyHallu) {
			    You((!incr) ?
				"now have a lesser case of the munchies." :
				"are getting the munchies.");
			} else
			    You((!incr) ? "only feel hungry now." :
				  (u.uhunger < 495) ? "feel hungry." :
				   "are beginning to feel hungry.");
			if (incr && occupation &&
			    (occupation != eatfood && occupation != opentin))
			    stop_occupation();
			break;
		case WEAK:
			if (FunnyHallu)
			    pline((!incr) ?
				  "You still have the munchies." :
      "The munchies are interfering with your motor capabilities.");
			else if (incr &&
				(Role_if(PM_WIZARD) || Role_if(PM_WARRIOR) || Role_if(PM_ELPH) || Role_if(PM_TWELPH) || Role_if(PM_COURIER) || Role_if(PM_SPACEWARS_FIGHTER) || Role_if(PM_CONVICT) || Role_if(PM_CAMPERSTRIKER) || Race_if(PM_ELF) || Race_if(PM_ASGARDIAN) || ishaxor || isheretic || Race_if(PM_ALBAE) || Race_if(PM_RODNEYAN) ||
				 Role_if(PM_VALKYRIE)))
			    pline("%s needs food, badly!", (!rn2(10)) ? urace.noun/*urole.name.m*/ :
				  (Role_if(PM_WIZARD) || Role_if(PM_WARRIOR) || Role_if(PM_ELPH) || Role_if(PM_TWELPH) || Role_if(PM_COURIER) || Role_if(PM_SPACEWARS_FIGHTER) || Role_if(PM_CONVICT) || Role_if(PM_CAMPERSTRIKER) || Race_if(PM_ASGARDIAN) || ishaxor || isheretic || Race_if(PM_ALBAE) || Role_if(PM_VALKYRIE)) ?
				  urole.name.m : Race_if(PM_RODNEYAN) ? "Wizard of Yendor" : "Elf");
			else
			    You((!incr) ? "feel weak now." :
				  (u.uhunger < 195) ? "feel weak." :
				   "are beginning to feel weak.");
			if (incr && occupation &&
			    (occupation != eatfood && occupation != opentin))
			    stop_occupation();
			break;
		}
		u.uhs = newhs;
		flags.botl = 1;
		bot();
		if ((Upolyd ? u.mh : u.uhp) < 1) {
			u.youaredead = 1;
			You(FunnyHallu ? "pass away like a filthy bum." : (have_anorexia()) ? "are embraced by the shadowy figure of your deity..." : "die from hunger and exhaustion.");
			killer_format = KILLED_BY;
			killer = "exhaustion";
			done(STARVING);
			u.youaredead = 0;
			return;
		}
	}
}

#endif /* OVL0 */
#ifdef OVLB

boolean can_reach_floorobj()
{
    return can_reach_floor() &&
	  !((is_waterypool(u.ux, u.uy) || is_watertunnel(u.ux, u.uy) || is_lava(u.ux, u.uy)) &&
	    (Wwalking || Race_if(PM_KORONST) || is_clinger(youmonst.data) || (Flying && !Breathless)));
}

/* Returns an object representing food.  Object may be either on floor or
 * in inventory.
 */
STATIC_OVL struct obj *
floorfood(verb)		/* get food from floor or pack */
	const char *verb;
{
	register struct obj *otmp;
	/* We cannot use ALL_CLASSES since that causes getobj() to skip its
	 * "ugly checks" and we need to check for inedible items.
	 */
	const char *edibles = (const char *)allobj;
	char qbuf[QBUFSZ];
	char c;

	if (u.usteed)	/* can't eat off floor while riding */
	    edibles++;
	else
	if (metallivorous(youmonst.data)) {
	    struct trap *ttmp = t_at(u.ux, u.uy);

	    if (ttmp && ttmp->tseen && ttmp->ttyp == BEAR_TRAP) {
		/* If not already stuck in the trap, perhaps there should
		   be a chance to becoming trapped?  Probably not, because
		   then the trap would just get eaten on the _next_ turn... */
		sprintf(qbuf, "There is a bear trap here (%s); eat it?",
			(u.utrap && u.utraptype == TT_BEARTRAP) ?
				"holding you" : "armed");
		if ((c = yn_function(qbuf, ynqchars, 'n')) == 'y') {
		    u.utrap = u.utraptype = 0;
		    deltrap(ttmp);
		    return mksobj(BEARTRAP, TRUE, FALSE, FALSE);
		} else if (c == 'q') {
		    return (struct obj *)0;
		}
	    }
	}

	otmp = getobj(edibles, verb);
	if (otmp && otmp->oclass == COIN_CLASS)
	    obj_extract_self(otmp);
	return otmp;
}

/* Side effects of vomiting */
/* added nomul (MRS) - it makes sense, you're too busy being sick! */
void
vomit()		/* A good idea from David Neves */
{
	u.cnd_vomitingcount++;

	if (isevilvariant) {
		register struct monst *shkp = shop_keeper(*u.ushops);
		if (shkp) {
			verbalize("You dare vomiting all over my shop, motherfucker???");
			make_angry_shk(shkp, 0, 0);
		}
	}

	if (u.inasuppression) {

		FemaleTrapIna += u.inasuppression;
		FemaleTrapIna += rnz(20 * (monster_difficulty() + 1));
		if (rn2(3)) FemaleTrapIna += 100;
		if (!rn2(3)) FemaleTrapIna += rnz(500);
		u.inasuppression = 0;
		pline("Oh my god, you are now afflicted with the life-threatening disease known as anorexia!");
	}
	make_sick(0L, (char *) 0, TRUE, SICK_VOMITABLE);
	nomul(-2, "vomiting", TRUE);
	u_wipe_engr(100);
	nomovemsg = 0;

	if (IS_ALTAR(levl[u.ux][u.uy].typ)) { /* whoops... --Amy */
		u.ugangr++;
		if (!rn2(5)) u.ugangr++;
		if (!rn2(25)) u.ugangr++;
		prayer_done();
	}
}

int
eaten_stat(base, obj)
register int base;
register struct obj *obj;
{
	long uneaten_amt, full_amount;

	uneaten_amt = (long)obj->oeaten;
	full_amount = (obj->otyp == CORPSE) ? (long)mons[obj->corpsenm].cnutrit
					: (long)objects[obj->otyp].oc_nutrition;
	if (uneaten_amt > full_amount) {
	    impossible(
	  "partly eaten food (%ld) more nutritious than untouched food (%ld)",
		       uneaten_amt, full_amount);
	    uneaten_amt = full_amount;
	}

	base = (int)(full_amount ? (long)base * uneaten_amt / full_amount : 0L);
	return (base < 1) ? 1 : base;
}

void
energysap(obj)
struct obj *obj;
{

	You("quaff the sap.");

	if (mons[obj->corpsenm].cnutrit > 0) lesshungry((mons[obj->corpsenm].cnutrit) / 5);

	cprefx(obj->corpsenm);
	cpostfx(obj->corpsenm);

	if (obj && obj->oartifact == ART_MULTIJUICE && rn2(5)) return;
	useup(obj);

}

/* reduce obj's oeaten field, making sure it never hits or passes 0 */
void
consume_oeaten(obj, amt)
struct obj *obj;
int amt;
{
    /*
     * This is a hack to try to squelch several long standing mystery
     * food bugs.  A better solution would be to rewrite the entire
     * victual handling mechanism from scratch using a less complex
     * model.  Alternatively, this routine could call done_eating()
     * or food_disappears() but its callers would need revisions to
     * cope with victual.piece unexpectedly going away.
     *
     * Multi-turn eating operates by setting the food's oeaten field
     * to its full nutritional value and then running a counter which
     * independently keeps track of whether there is any food left.
     * The oeaten field can reach exactly zero on the last turn, and
     * the object isn't removed from inventory until the next turn
     * when the "you finish eating" message gets delivered, so the
     * food would be restored to the status of untouched during that
     * interval.  This resulted in unexpected encumbrance messages
     * at the end of a meal (if near enough to a threshold) and would
     * yield full food if there was an interruption on the critical
     * turn.  Also, there have been reports over the years of food
     * becoming massively heavy or producing unlimited satiation;
     * this would occur if reducing oeaten via subtraction attempted
     * to drop it below 0 since its unsigned type would produce a
     * huge positive value instead.  So far, no one has figured out
     * _why_ that inappropriate subtraction might sometimes happen.
     */

    if (amt > 0) {
	/* bit shift to divide the remaining amount of food */
	obj->oeaten >>= amt;
    } else {
	/* simple decrement; value is negative so we actually add it */
	if ((int) obj->oeaten > -amt)
	    obj->oeaten += amt;
	else
	    obj->oeaten = 0;
    }

    if (obj->oeaten == 0) {
	if (obj == victual.piece)	/* always true unless wishing... */
	    victual.reqtime = victual.usedtime;	/* no bites left */
	obj->oeaten = 1;	/* smallest possible positive value */
    }
}

#endif /* OVLB */
#ifdef OVL1

/* called when eatfood occupation has been interrupted,
   or in the case of theft, is about to be interrupted */
boolean
maybe_finished_meal(stopping)
boolean stopping;
{
	/* in case consume_oeaten() has decided that the food is all gone */
	if (occupation == eatfood && victual.usedtime >= victual.reqtime) {
	    if (stopping) occupation = 0;	/* for do_reset_eat */
	    (void) eatfood(); /* calls done_eating() to use up victual.piece */
	    return TRUE;
	}
	return FALSE;
}

#endif /* OVL1 */

/*eat.c*/

