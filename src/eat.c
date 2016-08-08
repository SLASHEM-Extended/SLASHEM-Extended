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

#define PN_POLEARMS		(-1)
#define PN_SABER		(-2)
#define PN_HAMMER		(-3)
#define PN_WHIP			(-4)
#define PN_PADDLE		(-5)
#define PN_FIREARMS		(-6)
#define PN_ATTACK_SPELL		(-7)
#define PN_HEALING_SPELL	(-8)
#define PN_DIVINATION_SPELL	(-9)
#define PN_ENCHANTMENT_SPELL	(-10)
#define PN_PROTECTION_SPELL	(-11)
#define PN_BODY_SPELL		(-12)
#define PN_MATTER_SPELL		(-13)
#define PN_BARE_HANDED		(-14)
#define PN_HIGH_HEELS		(-15)
#define PN_MARTIAL_ARTS		(-16)
#define PN_RIDING		(-17)
#define PN_TWO_WEAPONS		(-18)
#ifdef LIGHTSABERS
#define PN_LIGHTSABER		(-19)
#endif

#ifndef OVLB

STATIC_DCL NEARDATA const short skill_names_indices[];
STATIC_DCL NEARDATA const char *odd_skill_names[];

#else	/* OVLB */

/* KMH, balance patch -- updated */
STATIC_OVL NEARDATA const short skill_names_indices[P_NUM_SKILLS] = {
	0,                DAGGER,         KNIFE,        AXE,
	PICK_AXE,         SHORT_SWORD,    BROADSWORD,   LONG_SWORD,
	TWO_HANDED_SWORD, SCIMITAR,       PN_SABER,     CLUB,
	PN_PADDLE,        MACE,           MORNING_STAR,   FLAIL,
	PN_HAMMER,        QUARTERSTAFF,   PN_POLEARMS,  SPEAR,
	JAVELIN,          TRIDENT,        LANCE,        BOW,
	SLING,            PN_FIREARMS,    CROSSBOW,       DART,
	SHURIKEN,         BOOMERANG,      PN_WHIP,      UNICORN_HORN,
#ifdef LIGHTSABERS
	PN_LIGHTSABER,
#endif
	PN_ATTACK_SPELL,     PN_HEALING_SPELL,
	PN_DIVINATION_SPELL, PN_ENCHANTMENT_SPELL,
	PN_PROTECTION_SPELL,            PN_BODY_SPELL,
	PN_MATTER_SPELL,
	PN_BARE_HANDED,	PN_HIGH_HEELS, 		PN_MARTIAL_ARTS, 
	PN_TWO_WEAPONS,
#ifdef STEED
	PN_RIDING,
#endif
};


STATIC_OVL NEARDATA const char * const odd_skill_names[] = {
    "no skill",
    "polearms",
    "saber",
    "hammer",
    "whip",
    "paddle",
    "firearms",
    "attack spells",
    "healing spells",
    "divination spells",
    "enchantment spells",
    "protection spells",
    "body spells",
    "matter spells",
    "bare-handed combat",
    "high heels",
    "martial arts",
    "riding",
    "two-handed combat",
#ifdef LIGHTSABERS
    "lightsaber"
#endif
};

#endif	/* OVLB */

#define P_NAME(type) (skill_names_indices[type] > 0 ? \
		      OBJ_NAME(objects[skill_names_indices[type]]) : \
			odd_skill_names[-skill_names_indices[type]])

STATIC_PTR int NDECL(eatmdone);
STATIC_PTR int NDECL(eatfood);
STATIC_PTR void FDECL(costly_tin, (const char*));
STATIC_PTR int NDECL(opentin);
STATIC_PTR int NDECL(unfaint);

#ifdef OVLB
STATIC_DCL const char *FDECL(food_xname, (struct obj *,BOOLEAN_P));
STATIC_DCL const char *FDECL(Food_xname, (struct obj *,BOOLEAN_P));
STATIC_DCL void FDECL(choke, (struct obj *));
STATIC_DCL void NDECL(recalc_wt);
STATIC_DCL struct obj *FDECL(touchfood, (struct obj *));
STATIC_DCL void NDECL(do_reset_eat);
STATIC_DCL void FDECL(done_eating, (BOOLEAN_P));
STATIC_DCL void FDECL(cprefx, (int));
STATIC_DCL int FDECL(intrinsic_possible, (int,struct permonst *));
STATIC_DCL void FDECL(givit, (int,struct permonst *));
STATIC_DCL void FDECL(cpostfx, (int));
STATIC_DCL void FDECL(start_tin, (struct obj *));
STATIC_DCL int FDECL(eatcorpse, (struct obj *));
STATIC_DCL void FDECL(start_eating, (struct obj *));
STATIC_DCL void FDECL(fprefx, (struct obj *));
STATIC_DCL void FDECL(accessory_has_effect, (struct obj *));
STATIC_DCL void FDECL(fpostfx, (struct obj *));
STATIC_DCL int NDECL(bite);
STATIC_DCL int FDECL(edibility_prompts, (struct obj *));
STATIC_DCL int FDECL(rottenfood, (struct obj *));
STATIC_DCL void NDECL(eatspecial);
STATIC_DCL void FDECL(eataccessory, (struct obj *));
STATIC_DCL const char *FDECL(foodword, (struct obj *));
STATIC_DCL boolean FDECL(maybe_cannibal, (int,BOOLEAN_P));
STATIC_DCL struct obj *FDECL(floorfood, (const char *));

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

/* also used to see if you're allowed to eat cats and dogs */
#define CANNIBAL_ALLOWED() (Role_if(PM_CAVEMAN) || Role_if(PM_LUNATIC) || Race_if(PM_ORC) || \
Race_if(PM_CURSER) || Race_if(PM_ALIEN) || Race_if(PM_TROLLOR) || Race_if(PM_VORTEX) || Race_if(PM_CORTEX) || Race_if(PM_HUMANOID_DEVIL) || Race_if(PM_MUMMY) || Race_if(PM_LICH_WARRIOR) || Race_if(PM_KOBOLT) || Race_if(PM_PHANTOM_GHOST) || Race_if(PM_GIGANT) || Race_if(PM_RODNEYAN) || Race_if(PM_OGRO) || Race_if(PM_WEAPON_TRAPPER) || \
 Race_if(PM_INSECTOID) || Race_if(PM_MOULD) || Race_if(PM_MISSINGNO) || Race_if(PM_HUMANLIKE_DRAGON) || Race_if(PM_HUMANLIKE_NAGA) || Race_if(PM_DEATHMOLD) || Race_if(PM_AQUATIC_MONSTER) || Race_if(PM_WORM_THAT_WALKS) || Race_if(PM_UNGENOMOLD) || Race_if(PM_UNALIGNMENT_THING) || Race_if(PM_HUMAN_WEREWOLF) || Race_if(PM_AK_THIEF_IS_DEAD_) || \
 Race_if(PM_SNAKEMAN) || Race_if(PM_SPIDERMAN) || Race_if(PM_RACE_X) || Race_if(PM_VAMPIRE) || Race_if(PM_VAMGOYLE) || Race_if(PM_SUCKING_FIEND) || Race_if(PM_LEVITATOR) || Race_if(PM_CLOCKWORK_AUTOMATON) || Race_if(PM_ARMED_COCKATRICE) || Race_if(PM_ELEMENTAL) || Race_if(PM_WEAPON_BUG) || Race_if(PM_HUMANOID_LEPRECHAUN) || Race_if(PM_NYMPH) || Race_if(PM_TURTLE) || Race_if(PM_LOWER_ENT) || Race_if(PM_SPRIGGAN) || Race_if(PM_JELLY) || Race_if(PM_WEAPON_CUBE) || Race_if(PM_WEAPON_IMP) || Race_if(PM_DRYAD) || Race_if(PM_AUREAL) || Race_if(PM_MAZKE) || Race_if(PM_BORG) || Race_if(PM_ELONA_SNAIL) || Race_if(PM_PLAYER_UNICORN) )

#ifndef OVLB

STATIC_DCL NEARDATA const char comestibles[];
STATIC_DCL NEARDATA const char allobj[];
STATIC_DCL boolean force_save_hs;

#else

STATIC_OVL NEARDATA const char comestibles[] = { FOOD_CLASS, 0 };

/* Gold must come first for getobj(). */
STATIC_OVL NEARDATA const char allobj[] = { COIN_CLASS, ALLOW_FLOOROBJ,
	WEAPON_CLASS, ARMOR_CLASS, POTION_CLASS, SCROLL_CLASS,
	WAND_CLASS, RING_CLASS, AMULET_CLASS, FOOD_CLASS, TOOL_CLASS,
	GEM_CLASS, ROCK_CLASS, BALL_CLASS, CHAIN_CLASS, SPBOOK_CLASS, 0 };

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
	/* above also prevents the Amulet from being eaten, so we must never
	   allow fake amulets to be eaten either [which is already the case] */

	/* Clockwork automatons can't eat anything at all, they need to use booze or oil --Amy */
	if (Race_if(PM_CLOCKWORK_AUTOMATON) && !Upolyd) return 0;

	/* Spirits can't eat corpses --Amy */
	if (Race_if(PM_SPIRIT) && obj->otyp == CORPSE && !Upolyd) return 0;

	/* Incantifier only eats stone and metal --Amy */
	if (Race_if(PM_INCANTIFIER) && !Upolyd) return (boolean) (is_metallic(obj) || is_lithic(obj));

	/* Unicorn only eats stone --Amy */
	if (Race_if(PM_PLAYER_UNICORN) && !Upolyd) return (boolean) (is_lithic(obj));

	if (metallivorous(youmonst.data) && is_metallic(obj) &&
	    (youmonst.data != &mons[PM_RUST_MONSTER] || is_rustprone(obj)))
		return TRUE;

	if (lithivorous(youmonst.data) && is_lithic(obj) )
		return TRUE;

	/* KMH -- Taz likes organics, too! */
	if (( (!Upolyd && Race_if(PM_JELLY) ) || (!Upolyd && Race_if(PM_WEAPON_CUBE) ) || u.umonnum == PM_GELATINOUS_CUBE || u.umonnum == PM_FLYING_GELATINOUS_CUBE || u.umonnum == PM_STOUT_GELATINOUS_CUBE || u.umonnum == PM_FANTASTIC_GELATINOUS_CUBE || u.umonnum == PM_GELATINOUS_GLOB || u.umonnum == PM_FAT_BULLY || u.umonnum == PM_OOZE_ELEMENTAL || u.umonnum == PM_AMUSING_TYPE || u.umonnum == PM_MINOCUBE || u.umonnum == PM_THEME_TERMITE || u.umonnum == PM_ROOMBA || u.umonnum == PM_GELATINOUS_DICE || u.umonnum == PM_WEAPON_CUBE || u.umonnum == PM_KING_GORGE__LORD_OF_THE_GLUTTONS || u.umonnum == PM_GELATINOUS_THIEF || u.umonnum == PM_TASMANIAN_ZOMBIE ||
			u.umonnum == PM_TASMANIAN_DEVIL) && is_organic(obj) &&
		/* [g.cubes can eat containers and retain all contents
		    as engulfed items, but poly'd player can't do that] */
	    !Has_contents(obj))
		return TRUE;

	/* Koalas only eat Eucalyptus leaves */
	if (u.umonnum == PM_KOALA)
		return (boolean)(obj->otyp == EUCALYPTUS_LEAF);
	if (u.umonnum == PM_GIANT_KOALA)
		return (boolean)(obj->otyp == EUCALYPTUS_LEAF);

	/* Ghouls, ghasts only eat corpses */
	if (u.umonnum == PM_GHOUL || u.umonnum == PM_GHAST || u.umonnum == PM_GASTLY || u.umonnum == PM_PHANTOM_GHOST || u.umonnum == PM_HAUNTER
	|| u.umonnum == PM_GENGAR || (Race_if(PM_GASTLY) && !Upolyd) || (Race_if(PM_PHANTOM_GHOST) && !Upolyd) )
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
	return (boolean)((obj->oclass == FOOD_CLASS) || (objects[(obj)->otyp].oc_material == VEGGY) || (objects[(obj)->otyp].oc_material == FLESH) ) ;
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
	    free((genericptr_t)eatmbuf),  eatmbuf = 0;
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

	    Sprintf(bufp, "%s%s corpse",
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
	} else if ((Role_if(PM_KNIGHT) && u.ualign.type == A_LAWFUL) || Role_if(PM_CHEVALIER) || Race_if(PM_VEELA) || Role_if(PM_PALADIN) || Role_if(PM_TOPMODEL) || Role_if(PM_FAILED_EXISTENCE) || Role_if(PM_GOFF)) {
			adjalign(-3);		/* gluttony is unchivalrous */
		You(Hallucination ? "feel that your belly's gonna burst!" : "feel like a glutton!");
	}

	if (Race_if(PM_VEELA) || Role_if(PM_FAILED_EXISTENCE) ) badeffect();

	if (Role_if(PM_TOPMODEL) || Role_if(PM_GOFF)) { /* They aren't used to eat much. --Amy */

	if(!rn2(4)) {
		if (Hallucination) You_feel("rather trippy.");
		else You_feel("rather %s.", body_part(LIGHT_HEADED));
		make_confused(HConfusion + d(2,4),FALSE);
	} else if(!rn2(5)) {
		if (Hallucination) You_feel("uncontrollable."); /* this and some other effects added by Amy */
		else You_feel("stunned.");
		make_stunned(HStun + d(2,4),FALSE);
	} else if(!rn2(5)) {
		if (Hallucination) You_feel("even weirder!");
		else You_feel("weirded out!");
		make_hallucinated(HHallucination + d(2,10),FALSE,0L);
	} else if(!rn2(4)) {
		if (!Blind) pline("Everything suddenly goes dark.");
		make_blinded(Blinded+d(2,10),FALSE);
		if (!Blind) Your(vision_clears);
	} else if(!rn2(8)) {
		if (Hallucination) You_feel("like your body is trying to fall asleep!");
		else You_feel("numb.");
		make_numbed(HNumbed + d(2,15),FALSE);
	} else if(!rn2(10)) {
		if (Hallucination) pline("Crap! A %s suddenly appeared! RUN!",rndmonnam() );
		else You_feel("fear spreading through your body.");
		make_feared(HFeared + d(2,15),FALSE);
	} else if(!rn2(10)) {
		if (Hallucination) You_feel("like your on fire despite there being no heat!");
		else You_feel("like you're on fire!");
		make_burned(HBurned + d(2,15),FALSE);
	} else if(!rn2(20)) {
		if (Hallucination) You_feel("a giant ice cream cone enclosing you!");
		else pline("You're getting the chills.");
		make_frozen(HFrozen + d(2,15),FALSE);
	} else if(!rn2(10)) {
		if (Hallucination) You_feel("like you have dementia tremor!"); /* not a real name --Amy */
		else pline("Your hands start trembling violently!");
		incr_itimeout(&Glib, d(2,4) );
	} else if(!rn2(10)) {
		if (Hallucination) You_feel("totally down! Seems you tried some illegal shit!");
		else pline("You feel like you're going to throw up.");
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
		    what = "you slap against the", where =
#ifdef STEED
			   (u.usteed) ? "saddle" :
#endif
			   surface(u.ux,u.uy);
		pline_The("world spins and %s %s.", what, where);
		flags.soundok = 0;
		nomul(-rnd(10), "unconscious from breaking anorexia conduct");
		nomovemsg = "You are conscious again.";
		afternmv = Hear_again;
		return;
	} else if(!rn2(50) && !Sick) { /* The chance of this outcome !MUST! be low. Everything else would be unfair. --Amy */
	    make_sick(rn1(25,25), "rotten food", TRUE, SICK_VOMITABLE);

	/* "No longer possible to get slimed from eating food. Wayyyyyy too evil" In Soviet Russia, people don't know that scrolls of fire, wands of fireball, fire traps and prayer exist! Out of general paranoia, they have to remove the incredibly low chance of getting slimed, even though it basically never happened anyway. Chances are they just looked at the code and decided that it must be evil because it's in there. Come on! It has to pass the !rn2(200) chance AND all the "else if"s above, so it's bloody unlikely for it to happen, and even on the off chance it does, that's why you have scrolls of fire/cure! --Amy */

	} else if(!rn2(200) && !Slimed && !issoviet && !flaming(youmonst.data) && !Unchanging && !slime_on_touch(youmonst.data) ) { /* This chance should be even lower. --Amy */
		    Slimed = 100L;
		    flags.botl = 1;
		    killer_format = KILLED_BY_AN;
		    delayed_killer = "slimy meal";
	}

	}

	exercise(A_CON, FALSE);

	if (Breathless || (!Strangled && !rn2(2))) { /* much higher chance to survive --Amy */
		/* choking by eating AoS doesn't involve stuffing yourself */
		/* ALI - nor does other non-food nutrition (eg., life-blood) */
		if (!food || food->otyp == AMULET_OF_STRANGULATION) {
			nomovemsg = "You recover your composure.";
			You(Hallucination ? "spew bits of puke everywhere." : "choke over it.");
			morehungry(100);	/* remove a bit of nutrition so you don't choke again instantly --Amy */
			nomul(-2, "vomiting");
			return;
		}
		You(Hallucination ? "vomit all over the place. Shit, now your clothes are a huge mess!" : "stuff yourself and then vomit voluminously.");
		if (Role_if(PM_TOPMODEL) || Role_if(PM_FAILED_EXISTENCE) || Role_if(PM_GOFF)) {adjalign(-20);	/* overeating doesn't befit a topmodel */
		pline(Hallucination ? "Uaargh - maybe you should order some smaller meals next time?" : "Bleeargh! You feel very bad for trying to overeat."); }
		morehungry(2000);	/* you just got *very* sick! */
		nomovemsg = 0;
		vomit();
	} else {
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
		if (message) pline(nomovemsg);
		nomovemsg = 0;
	} else if (message)
		You("finish %s %s.", victual.piece->odrained ? "draining" :
		  "eating", food_xname(victual.piece, TRUE));

	/* Bleeders who can drain corpses will benefit from doing so */
	if(victual.piece->otyp == CORPSE && victual.piece->odrained && Role_if(PM_BLEEDER) ) {

		pline(Hallucination ? "It tastes like sweet, delicious nectar!" : "That blood really hit the spot!");
		healup(d(8,8) + rnz(u.ulevel), 0, TRUE, TRUE);
		exercise(A_CON, TRUE);

	}

	if(victual.piece->otyp == CORPSE && victual.piece->odrained && Race_if(PM_HEMOPHAGE) ) {

		pline(Hallucination ? "It tastes like sweet, delicious nectar!" : "That blood really hit the spot!");
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
	    if (!CANNIBAL_ALLOWED()) {
		if (allowmsg) {
		    if (Upolyd)
			You("have a bad feeling deep inside.");
		    You("cannibal!  You will regret this!");
		}
		HAggravate_monster |= FROMOUTSIDE;
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
	if (touch_petrifies(&mons[pm]) || pm == PM_MEDUSA) {
	    if (!Stone_resistance &&
		!(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) {
		/* Sprintf(killer_buf, "tasting petrifying meat (%s)", mons[pm].mname);
		killer_format = KILLED_BY;
		killer = killer_buf;
		You("turn to stone.");
		done(STONING);
		if (victual.piece)
		    victual.eating = FALSE;
		return; */
		if (!Stoned) Stoned = 7;
		Sprintf(killer_buf, "tasting petrifying meat (%s)", mons[pm].mname);
		delayed_killer = killer_buf;
		
	    }
	}

	switch(pm) {
	    case PM_LITTLE_DOG:
	    case PM_DOG:
	    case PM_LARGE_DOG:
	    case PM_KITTEN:
	    case PM_HOUSECAT:
	    case PM_LARGE_CAT:
	    case PM_DOMESTIC_COCKATRICE:
		if (!CANNIBAL_ALLOWED()) {
		    You_feel("that %s the %s%s was a bad idea.",
		      victual.eating ? "eating" : "biting",
		      occupation == opentin ? "tinned " : "", mons[pm].mname);
		    HAggravate_monster |= FROMOUTSIDE;
		}
		break;
	    case PM_LIZARD:
	    case PM_ROCK_LIZARD:
	    case PM_BLACK_LIZARD:
	    case PM_BABY_CAVE_LIZARD:
	    case PM_NIGHT_LIZARD:
	    case PM_LIZARD_MAN:
	    case PM_LIZARD_KING:
	    case PM_CAVE_LIZARD:
	    case PM_PREHISTORIC_CAVE_LIZARD:
	    case PM_CHAOS_LIZARD:
	    case PM_HUGE_LIZARD:
	    case PM_SAND_TIDE:
	    case PM_FBI_AGENT:
	    case PM_OWN_SMOKE:
	    case PM_GRANDPA:
	    case PM_KARMIC_LIZARD:
	    case PM_MONSTER_LIZARD:
	    case PM_FIRE_LIZARD:
	    case PM_LIGHTNING_LIZARD:
	    case PM_ICE_LIZARD:
	    case PM_GIANT_LIZARD:
	    case PM_LIZARD_EEL:
	    case PM_EEL_LIZARD:
	    case PM_GRASS_LIZARD:
	    case PM_BLUE_LIZARD:
	    case PM_SWAMP_LIZARD:
	    case PM_SPITTING_LIZARD:
	    case PM_ANTI_STONE_LIZARD:
	    case PM_MIMIC_LIZARD:
	    case PM_HIDDEN_LIZARD:
	    case PM_CLINGING_LIZARD:
	    case PM_DEFORMED_LIZARD:
	    case PM_MEDUSA:
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
	    case PM_IGUANA:
	    case PM_BIG_IGUANA:
	    case PM_HELPFUL_SQUIRREL:
		make_hallucinated(0L,TRUE,0L);
		make_confused(0L,TRUE);
		make_stunned(0L,TRUE);
		make_blinded(0L,TRUE);
		break;
	    case PM_RHAUMBUSUN:
	    case PM_BIG_RHAUMBUSUN:
		if (Slimed) {pline("The slime disappears.");
		    Slimed = 0;
		    flags.botl = 1;
		    delayed_killer = 0;
			}
		break;
	    case PM_SALAMANDER:
	    case PM_FROST_SALAMANDER:
	    case PM_KOMODO_DRAGON:
	    case PM_PETTY_KOMODO_DRAGON:
		make_numbed(0L,TRUE);
		make_frozen(0L,TRUE);
		make_burned(0L,TRUE);
		make_feared(0L,TRUE);
		break;
	    case PM_GECKO:
	    case PM_FLYING_GECKO:
	    case PM_GIANT_GECKO:
		if (Sick)
		    make_sick(0L, (char *)0, TRUE, SICK_ALL);
		if (Vomiting)
		    make_vomiting(0L, TRUE);
		break;
	    case PM_DEATH:
	    case PM_PESTILENCE:
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
		{ char buf[BUFSZ];
		    pline("Eating that is instantly fatal.");
		    Sprintf(buf, "unwisely ate the body of a monster (%s)",
			    mons[pm].mname);
		    killer = buf;
		    killer_format = NO_KILLER_PREFIX;
		    done(DIED);
		    /* It so happens that since we know these monsters */
		    /* cannot appear in tins, victual.piece will always */
		    /* be what we want, which is not generally true. */
		    if (revive_corpse(victual.piece, FALSE))
			victual.piece = (struct obj *)0;
		    return;
		}
		/* Fall through */
	    default:

		if (!Slimed && !Unchanging && !flaming(youmonst.data) && slime_on_touch(&mons[pm]) &&
			!slime_on_touch(youmonst.data) ) {
		    You("don't feel very well.");
		    Slimed = 100L;
		    flags.botl = 1;
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
	case PM_ROCK_LIZARD:
	case PM_BABY_CAVE_LIZARD:
	case PM_NIGHT_LIZARD:
	case PM_LIZARD_MAN:
	case PM_LIZARD_KING:
	case PM_CAVE_LIZARD:
	case PM_PREHISTORIC_CAVE_LIZARD:
	case PM_CHAOS_LIZARD:
	case PM_LIZARD_EEL:
	case PM_EEL_LIZARD:
	case PM_GRASS_LIZARD:
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
	    case PM_OWN_SMOKE:
	    case PM_GRANDPA:
	case PM_KARMIC_LIZARD:
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
	    pline("Unfortunately, eating any of it is fatal.");
	    done_in_by(mon);
	    return TRUE;		/* lifesaved */

	    /* Fall through */
	default:
	    if (!Unchanging && youmonst.data != &mons[PM_FIRE_VORTEX] &&
			    youmonst.data != &mons[PM_FIRE_ELEMENTAL] &&
			    !slime_on_touch(youmonst.data) && slime_on_touch(mon->data) ) {
		You("don't feel very well.");
		Slimed = 100L;
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
	if (Hallucination)
	    pline("What a pity - you just ruined a future piece of %sart!",
		  ACURR(A_CHA) > 15 ? "fine " : "");
	else
	    You_feel("limber!");
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
			chance = (Race_if(PM_ILLITHID) ? 105 : 35); /*much lower chance now --Amy */
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
			chance = (Race_if(PM_ILLITHID) ? 35 : 15);
			break;
	}

	}

	if ( (ptr->mlevel <= rn2(chance) ) || !rn2(4) )
		return;		/* failed die roll */

	switch (type) {
	    case ACID_RES:
#ifdef DEBUG
		debugpline("Trying to give acid resistance");
#endif
		if(!(Acid_resistance & INTRINSIC)) {
			You(Hallucination ? "wanna do more acid!" :
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
			You(Hallucination ? "feel stony and groovy, man." :
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
			You(Hallucination ? "be chillin'." :
			    "feel a momentary chill.");
			HFire_resistance |= FROMOUTSIDE;
		}
		break;
	    case SLEEP_RES:
#ifdef DEBUG
		debugpline("Trying to give sleep resistance");
#endif
		if(!(HSleep_resistance & FROMOUTSIDE)) {
			You_feel("wide awake.");
			HSleep_resistance |= FROMOUTSIDE;
		}
		break;
	    case COLD_RES:
#ifdef DEBUG
		debugpline("Trying to give cold resistance");
#endif
		if(!(HCold_resistance & FROMOUTSIDE)) {
			You_feel("full of hot air.");
			HCold_resistance |= FROMOUTSIDE;
		}
		break;
	    case DISINT_RES:
#ifdef DEBUG
		debugpline("Trying to give disintegration resistance");
#endif
		if(!(HDisint_resistance & FROMOUTSIDE)) {
			You_feel(Hallucination ?
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
			if (Hallucination)
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
			You_feel(Hallucination ? "diffuse." :
			    "very jumpy.");
			HTeleportation |= FROMOUTSIDE;
		}
		break;
	    case TELEPORT_CONTROL:
#ifdef DEBUG
		debugpline("Trying to give teleport control");
#endif
		if(!(HTeleport_control & FROMOUTSIDE)) {
			You_feel(Hallucination ?
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
			You_feel(Hallucination ?
			    "in touch with the cosmos." :
			    "a strange mental acuity.");
			HTelepat |= FROMOUTSIDE;
			/* If blind, make sure monsters show up. */
			if (Blind) see_monsters();
		}
		break;

	    case HALLU_PARTY:
		if(!(HHallu_party & FROMOUTSIDE)) {
			You_feel(Hallucination ?
			    "like throwing wild parties with lots of sexy girls! Yeah!" :
			    "a strange desire to celebrate.");
			HHallu_party |= FROMOUTSIDE;
		}
		break;
	    case DRUNKEN_BOXING:
		if(!(HDrunken_boxing & FROMOUTSIDE)) {
			You_feel(Hallucination ?
			    "like Mike Tyson!" :
			    "ready for a good brawl.");
			HDrunken_boxing |= FROMOUTSIDE;
		}
		break;
	    case STUNNOPATHY:
		if(!(HStunnopathy & FROMOUTSIDE)) {
			You_feel(Hallucination ?
			    "a controlled wobbling! Feels like being on a bouncy ship!" :
			    "steadily observant.");
			HStunnopathy |= FROMOUTSIDE;
		}
		break;
	    case NUMBOPATHY:
		if(!(HNumbopathy & FROMOUTSIDE)) {
			You_feel(Hallucination ?
			    "as if a sweet woman were clamping your toes with a block-heeled combat boot!" :
			    "a numb feeling spreading through your body. Somehow, it doesn't feel bad at all...");
			HNumbopathy |= FROMOUTSIDE;
		}
		break;
	    case FREEZOPATHY:
		if(!(HFreezopathy & FROMOUTSIDE)) {
			You_feel(Hallucination ?
			    "like eating a big cone of ice-cream - mmmmmmmm!" :
			    "icy.");
			HFreezopathy |= FROMOUTSIDE;
		}
		break;
	    case STONED_CHILLER:
		if(!(HStoned_chiller & FROMOUTSIDE)) {
			You_feel(Hallucination ?
			    "that you're simply the best - yeah, no shit, man!" :
			    "like relaxing on a couch.");
			HStoned_chiller |= FROMOUTSIDE;
		}
		break;
	    case CORROSIVITY:
		if(!(HCorrosivity & FROMOUTSIDE)) {
			You_feel(Hallucination ?
			    "like you just got splashed with gunks of acid!" :
			    "an acidic burning.");
			HCorrosivity |= FROMOUTSIDE;
		}
		break;
	    case FEAR_FACTOR:
		if(!(HFear_factor & FROMOUTSIDE)) {
			You_feel(Hallucination ?
			    "like you're always running - from something! And the 'something' is a prostitute." :
			    "ready to face your fears.");
			HFear_factor |= FROMOUTSIDE;
		}
		break;
	    case BURNOPATHY:
		if(!(HBurnopathy & FROMOUTSIDE)) {
			You_feel(Hallucination ?
			    "super burninated by enemy with very tired!" :
			    "a burning inside. Strangely, it feels quite soothing.");
			HBurnopathy |= FROMOUTSIDE;
		}
		break;
	    case SICKOPATHY:
		if(!(HSickopathy & FROMOUTSIDE)) {
			You_feel(Hallucination ?
			    "that you just ate some really wacky stuff! What the heck was in there?" :
			    "ill for a moment, but get the feeling that you know more about diseases now.");
			HSickopathy |= FROMOUTSIDE;
		}
		break;
	    case WONDERLEGS:
		if(!(HWonderlegs & FROMOUTSIDE)) {
			You_feel(Hallucination ?
			    "a wonderful sensation in your shins, like they were just kicked by female hugging boots! How lovely!" :
			    "like having your legs scratched up and down by sexy leather pumps.");
			HWonderlegs |= FROMOUTSIDE;
		}
		break;
	    case GLIB_COMBAT:
		if(!(HGlib_combat & FROMOUTSIDE)) {
			You_feel(Hallucination ?
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
	    case PM_IGUANA:
	    case PM_GECKO:
	    case PM_FLYING_GECKO:
			lesshungry(20);
		break;
	    case PM_RHAUMBUSUN:
			lesshungry(40);
	    case PM_SQUIRREL:
	    case PM_HELPFUL_SQUIRREL:
			lesshungry(50);
		break;
	    case PM_BIG_IGUANA:
	    case PM_GIANT_GECKO:
			lesshungry(120);
	    case PM_BIG_RHAUMBUSUN:
			lesshungry(160);
		break;
	    case PM_SALAMANDER:
	    case PM_FROST_SALAMANDER:
	    case PM_KOMODO_DRAGON:
	    case PM_PETTY_KOMODO_DRAGON:
			lesshungry(400);
		break;
	    case PM_GORGON:
	    case PM_ATTRACTIVE_TROVE:

		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;
		if (ABASE(A_CHA) < ATTRMAX(A_CHA)) {
			if (!rn2(10)) {
				pline("You feel more %s!", flags.female ? "pretty" : "attractive");
				(void) adjattrib(A_CHA, 1, FALSE);
				break;
			}
		}
		break;

	    case PM_DRACOLISK:
	    case PM_CHARISMA_TROVE:
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;
		if (ABASE(A_CHA) < ATTRMAX(A_CHA)) {
			pline("You feel more %s!", flags.female ? "pretty" : "attractive");
			(void) adjattrib(A_CHA, 1, FALSE);
			}
		break;

	    case PM_STONING_MONSTER:
	    case PM_TOUGH_TROVE:
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;
		if (ABASE(A_CON) < ATTRMAX(A_CON)) {
			pline("You feel tougher!");
			(void) adjattrib(A_CON, 1, FALSE);
			}

		break;

	    case PM_SMALL_CHICKATRICE:
	    case PM_NEWT:
	    case PM_ENERGY_TROVE:
	    case PM_GRAY_NEWT:
	    case PM_ARCH_NEWT:
	    case PM_REMORHAZ:
	    case PM_DEMINEWT:
	    case PM_MASTER_NEWT:
	    case PM_EMPEROR_NEWT:
	    case PM_LARGE_NEWT:
	    case PM_ORE_NEWT:
	    case PM_SENSEI_NEWT:
	    case PM_GRANDMASTER_NEWT:
	    case PM_ASPHYNX:
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
		if (old_uen != u.uen) {
		    You_feel("a mild buzz.");
		    flags.botl = 1;
		}
		}
		break;
	    case PM_GIGANTIC_NEWT:
	    case PM_BONUS_MANA_TROVE:
	    case PM_KILLERTRICE:
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;
	      {int old_uen = u.uen;
		    u.uen += rnd(4);
		    if (rn2(3)) u.uenmax++;
		    u.uen += rnd(4);
		    if (rn2(3)) u.uenmax++;
		    u.uen += rnd(4);
		    if (rn2(3)) u.uenmax++;
			u.uenmax++; u.uenmax++; u.uenmax++;
		if (old_uen != u.uen) {
		    You_feel("a mild buzz.");
		    flags.botl = 1;
		}
		}
		break;
	    case PM_TWEN:
	    case PM_HEALTH_TROVE:
	    case PM_GRAY_TWEN:
	    case PM_DEMITWEN:
	    case PM_MASTER_TWEN:
	    case PM_GRANDMASTER_TWEN:
	    case PM_LARGE_TWEN:
	    case PM_ORE_TWEN:
	    case PM_EMPEROR_TWEN:
	    case PM_SENSEI_TWEN:
	    case PM_YELDUD_TWEN:
	    case PM_PORTER_RUBBER_CHICKEN:
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;
		if (Upolyd) {
			u.mh++;
			u.mhmax++;
		} else {
			u.uhp++;
			u.uhpmax++;
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
		    You_feel("vitalized.");
		    flags.botl = 1;
		break;

	    case PM_PETRO_MOLD:
	    case PM_PETRO_FUNGUS:
	    case PM_PETRO_PATCH:
	    case PM_PETRO_FORCE_FUNGUS:
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
		    You("feel clairvoyant!");
			incr_itimeout(&HClairvoyant, rnd(500));
		break;
	    case PM_STONING_SPHERE:
	    case PM_LEVELING_TROVE:
	    case PM_DREAMER_COCKATRICE:
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;
	      You("feel that was a smart thing to do.");
		pluslvl(FALSE);

		break;

	    case PM_STONE_BUG:
		You("feel that was a bad idea.");
		losexp("eating a stone bug corpse", FALSE, TRUE);

		break;

	    case PM_WRAITH:
	    case PM_NASTY_WRAITH:
	    case PM_TURBO_CHICKEN:
	    case PM_GREATER_BASILISK:
	    case PM_ETHEREAL_TROVE:
	    case PM_CENTAURTRICE:
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;
		switch(rnd(10)) {                
		case 1:
		    You("feel that was a bad idea.");
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
		    You("feel something strange for a moment.");
		    break;
		case 5: 
		    You("feel physically and mentally stronger!");
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
		    You("feel that was a smart thing to do.");
		pluslvl(FALSE);
		break;
		default:            
		    break;
		}
		flags.botl = 1;
		break;
	    case PM_CREEPING___:
	    case PM_PETRO_CENTIPEDE:
	    case PM_PSEUDO_MEDUSA:
	    case PM____TROVE:
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;
		pline("You feel appropriately 42!");
		switch(rnd(10)) {                
		case 1:
		    You("feel that was a bad idea.");
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
		    You("feel something strange for a moment.");
			make_hallucinated(HHallucination+rn1(10,50),TRUE,0L);
		    break;
		case 5: 
		    You("feel physically and mentally stronger!");
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
		    You("feel that was a smart thing to do.");
		pluslvl(FALSE);
		make_hallucinated(HHallucination+rn1(10,50),TRUE,0L);
		break;
		default:            
		    break;
		}
		flags.botl = 1;
		break;
	    case PM_HUMAN_WERERAT:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WERERAT;
		break;
	    case PM_HUMAN_WERERABBIT:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WERERABBIT;
		break;
	    case PM_HUMAN_WERELOCUST:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WERELOCUST;
		break;
	    case PM_HUMAN_WEREBOAR:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WEREBOAR;
		break;
	    case PM_HUMAN_WEREJACKAL:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WEREJACKAL;
		break;
	    case PM_HUMAN_WEREWOLF:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WEREWOLF;
		break;
	    case PM_HUMAN_WEREPANTHER:            
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WEREPANTHER;
		break;
	    case PM_HUMAN_WERETIGER:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WERETIGER;
		break;
	    case PM_HUMAN_WERESNAKE:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WERESNAKE;
		break;
	    case PM_HUMAN_WERESPIDER:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WERESPIDER;
		break;
	    case PM_HUMAN_WEREPIRANHA:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WEREPIRANHA;
		break;
	    case PM_HUMAN_WEREEEL:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WEREEEL;
		break;
	    case PM_HUMAN_WEREFLYFISH:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WEREFLYFISH;
		break;
	    case PM_HUMAN_WEREKRAKEN:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WEREKRAKEN;
		break;
	    case PM_HUMAN_WEREGHOST:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WEREGHOST;
		break;
	    case PM_HUMAN_WEREGIANT:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WEREGIANT;
		break;
	    case PM_HUMAN_WERELICHEN:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WERELICHEN;
		break;
	    case PM_HUMAN_WEREVORTEX:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WEREVORTEX;
		break;
	    case PM_HUMAN_WERECOW:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WERECOW;
		break;
	    case PM_HUMAN_WEREBEAR:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WEREBEAR;
		break;
	    case PM_HUMAN_WEREPIERCER:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WEREPIERCER;
		break;
	    case PM_HUMAN_WEREPENETRATOR:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WEREPENETRATOR;
		break;
	    case PM_HUMAN_WERESMASHER:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WERESMASHER;
		break;
	    case PM_HUMAN_WERENYMPH:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WERENYMPH;
		break;
	    case PM_HUMAN_WEREBEE:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WEREBEE;
		break;
	    case PM_HUMAN_WERESOLDIERANT:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WERESOLDIERANT;
		break;
	    case PM_HUMAN_WEREBAT:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WEREBAT;
		break;
	    case PM_HUMAN_WEREGRIDBUG:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WEREGRIDBUG;
		break;
	    case PM_HUMAN_WERELICH:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WERELICH;
		break;
	    case PM_HUMAN_WEREJABBERWOCK:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WEREJABBERWOCK;
		break;

	    case PM_HUMAN_WERECOCKATRICE:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WERECOCKATRICE;

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
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WEREMIMIC;
		tmp += 30;
		if (youmonst.data->mlet != S_MIMIC && !Unchanging) {
		    char buf[BUFSZ];
		    You_cant("resist the temptation to mimic %s.",
			Hallucination ? "an orange" : "a pile of gold");
#ifdef STEED
                    /* A pile of gold can't ride. */
		    if (u.usteed) dismount_steed(DISMOUNT_FELL);
#endif
		    nomul(-tmp, "pretending to be a pile of gold");
		    Sprintf(buf, Hallucination ?
			"You suddenly dread being peeled and mimic %s again!" :
			"You now prefer mimicking %s again.",
			an( (Upolyd && !missingnoprotect) ? youmonst.data->mname : urace.noun));
		    eatmbuf = strcpy((char *) alloc(strlen(buf) + 1), buf);
		    nomovemsg = eatmbuf;
		    afternmv = eatmdone;
		    /* ??? what if this was set before? */
		    youmonst.m_ap_type = M_AP_OBJECT;
		    youmonst.mappearance = Hallucination ? ORANGE : GOLD_PIECE;
		    newsym(u.ux,u.uy);
		    curs_on_u();
		    /* make gold symbol show up now */
		    display_nhwindow(WIN_MAP, TRUE);
		}
		break;
	    case PM_HUMAN_WEREPERMAMIMIC:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WEREMIMIC;
		tmp += 30;
		if (youmonst.data->mlet != S_MIMIC && !Unchanging) {
		    char buf[BUFSZ];
		    You_cant("resist the temptation to mimic %s.",
			Hallucination ? "an orange" : "a pile of gold");
#ifdef STEED
                    /* A pile of gold can't ride. */
		    if (u.usteed) dismount_steed(DISMOUNT_FELL);
#endif
		    nomul(-tmp, "pretending to be a pile of gold");
		    Sprintf(buf, Hallucination ?
			"You suddenly dread being peeled and mimic %s again!" :
			"You now prefer mimicking %s again.",
			an( (Upolyd && !missingnoprotect) ? youmonst.data->mname : urace.noun));
		    eatmbuf = strcpy((char *) alloc(strlen(buf) + 1), buf);
		    nomovemsg = eatmbuf;
		    afternmv = eatmdone;
		    /* ??? what if this was set before? */
		    youmonst.m_ap_type = M_AP_OBJECT;
		    youmonst.mappearance = Hallucination ? ORANGE : GOLD_PIECE;
		    newsym(u.ux,u.uy);
		    curs_on_u();
		    /* make gold symbol show up now */
		    display_nhwindow(WIN_MAP, TRUE);
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
	    case PM_THE_HIDDEN:
	    case PM_INVISIBLE_BADGUY:
	    case PM_UNSEEN_POTATO:
	    case PM_CAMOUFLAGED_WATCHER:
	    case PM_HIDDEN_TRACKER:
	    case PM_UNSEEN_SERVANT:
	    case PM_SILENT_KILLER:
	    case PM_STONE_STALKER:
	    case PM_ILLUSION_WEAVER:
	    case PM_PAIN_MASTER:
	    case PM_COCKTAUR:
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
	    case PM_GIANT_BAT:
	    case PM_CHICKATRICE:
		make_stunned(HStun + 30,FALSE);
		/* fall into next case */
	    case PM_BAT:
		make_stunned(HStun + 30,FALSE);
		break;
	    case PM_BURNING_MIMIC:
	    case PM_SCROLL_MIMIC:
	    case PM_WAND_MIMIC:
	    case PM_WAND_PERMAMIMIC:
	    case PM_RING_MIMIC:
	    case PM_STAIRWAY_TO_HELL:
	    case PM_POTION_MIMIC:
	    case PM_TOME_OF_DESTRUCTION:
	    case PM_TOME_OF_DIVINE_WRATH:
	    case PM_LESSER_MIMIC:
	    case PM_ULTIMATE_MIMIC:
	    case PM_EMPEROR_MIMIC:
	    case PM_DOOR_MIMIC:
	    case PM_DEMONIC_DOOR:
	    case PM_SATANIC_DOOR:
	    case PM_CLOAKER:
	    case PM_CHEST_MIMIC:
	    case PM_UNKNOWN_MIMIC:
	    case PM_UNKNOWN_MIMIC_X:
	    case PM_UNKNOWN_PERMAMIMIC_X:
	    case PM_BULLY_MIMIC:
	    case PM_UNDEAD_MIMIC:
	    case PM_ICE_MIMIC:
	    case PM_TAP_MIMIC:
	    case PM_FREEZER_MIMIC:
	    case PM_STATIC_MIMIC:
	    case PM_DARK_MIMIC:
	    case PM_CONTROLLER_MIMIC:
	    case PM_BOOBED_MIMIC:
	    case PM_SUXXOR_MIMIC:
	    case PM_MINUSCULE_MIMIC:
	    case PM_WITHERING_MIMIC:
	    case PM_ACID_MIMIC:
	    case PM_DECAYING_MIMIC:
	    case PM_RUSTY_MIMIC:
	    case PM_PETTY_BOOBED_MIMIC:
	    case PM_SUCKING_MIMIC:
	    case PM_VAMP_MIMIC:
	    case PM_KILLER_MIMIC:
	    case PM_FOUL_MIMIC:
	    case PM_PORTER_MIMIC:
	    case PM_STEALER_MIMIC:
	    case PM_BURNING_PERMAMIMIC:
	    case PM_SCROLL_PERMAMIMIC:
	    case PM_STAFF_PERMAMIMIC:
	    case PM_STAFF_MIMIC:
	    case PM_ROD_PERMAMIMIC:
	    case PM_ROD_MIMIC:
	    case PM_MAGIC_BOOK_PERMAMIMIC:
	    case PM_MAGIC_BOOK_MIMIC:
	    case PM_PRAYER_BOOK_PERMAMIMIC:
	    case PM_PRAYER_BOOK_MIMIC:
	    case PM_SONG_BOOK_PERMAMIMIC:
	    case PM_SONG_BOOK_MIMIC:
	    case PM_RING_PERMAMIMIC:
	    case PM_MAP_PERMAMIMIC:
	    case PM_MAP_MIMIC:
	    case PM_RUNESTONE_PERMAMIMIC:
	    case PM_RUNESTONE_MIMIC:
	    case PM_CLOAK_PERMAMIMIC:
	    case PM_CLOAK_MIMIC:
	    case PM_STAIRWAY_TO_HEAVEN:
	    case PM_POTION_PERMAMIMIC:
	    case PM_TOME_OF_ANNIHILATION:
	    case PM_TOME_OF_NETHER_WRATH:
	    case PM_LESSER_PERMAMIMIC:
	    case PM_ULTIMATE_PERMAMIMIC:
	    case PM_EMPEROR_PERMAMIMIC:
	    case PM_DOOR_PERMAMIMIC:
	    case PM_SMALL_DOOR_PERMAMIMIC:
	    case PM_SMALL_DOOR_MIMIC:
	    case PM_PERMACLOAKER:
	    case PM_CHEST_PERMAMIMIC:
	    case PM_UNKNOWN_PERMAMIMIC:
	    case PM_BULLY_PERMAMIMIC:
	    case PM_UNDEAD_PERMAMIMIC:
	    case PM_ICE_PERMAMIMIC:
	    case PM_TAP_PERMAMIMIC:
	    case PM_FREEZER_PERMAMIMIC:
	    case PM_STATIC_PERMAMIMIC:
	    case PM_DARK_PERMAMIMIC:
	    case PM_CONTROLLER_PERMAMIMIC:
	    case PM_BOOBED_PERMAMIMIC:
	    case PM_SUXXOR_PERMAMIMIC:
	    case PM_MINUSCULE_PERMAMIMIC:
	    case PM_WITHERING_PERMAMIMIC:
	    case PM_ACID_PERMAMIMIC:
	    case PM_DECAYING_PERMAMIMIC:
	    case PM_RUSTY_PERMAMIMIC:
	    case PM_PETTY_BOOBED_PERMAMIMIC:
	    case PM_SUCKING_PERMAMIMIC:
	    case PM_VAMP_PERMAMIMIC:
	    case PM_KILLER_PERMAMIMIC:
	    case PM_FOUL_PERMAMIMIC:
	    case PM_PORTER_PERMAMIMIC:
	    case PM_STEALER_PERMAMIMIC:
	    case PM_MIMIC_MUMMY:
	    case PM_MIMIC_NYMPH:
	    case PM_MIMIC_ANT:
	    case PM_MIMICRY_RUBBER:
	    case PM_UNKNOWN_ESCALATION_RUBBER:
	    case PM_CAMO_FISH:
	    case PM_MINOTAUR_MIMIC:
	    case PM_FLYING_CAMO_FISH:
	    case PM_DEFORMED_FISH:
	    case PM_KEYSTONE_INSPECTOR:
	    case PM_INSPECTOR_SERGEANT:
	    case PM_INSPECTOR_LIEUTENANT:
	    case PM_INSPECTOR_KAPTAIN:
	    case PM_INSPECTOR_KOMMISSIONER:
	    case PM_INSPECTOR_KCHIEF:
	    case PM_INSPECTOR_KATCHER:
	    case PM_INSPECTOR_KRIMINOLOGIST:
	    case PM_FATA_MORGANA:
	    case PM_UNIDENTIFIED_KOBOLD_SHAMAN:
	    case PM_CURSED_SPIRIT:
	    case PM_HIDDEN_RUTTERKIN:
	    case PM_DEVILISH_SPIRIT:
	    case PM_OSCILLATOR:
	    case PM_ALIENATED_UFO_PART:
	    case PM_VAMPIRE_SHADOWCLOAK:
	    case PM_MULTI_HUED_NAGA:
	    case PM_MIMIC_VORTEX:
	    case PM_VOLTORB:
	    case PM_GIANT_EGO_MIMIC:
	    case PM_GIANT_EGO_PERMAMIMIC:
	    case PM_KEYSTONE_WARDER:
	    case PM_WARDER_SERGEANT:
	    case PM_WARDER_LIEUTENANT:
	    case PM_WARDER_KAPTAIN:
	    case PM_WARDER_KOMMISSIONER:
	    case PM_WARDER_KCHIEF:
	    case PM_WARDER_KATCHER:
	    case PM_WARDER_KRIMINOLOGIST:
	    case PM_HEHEHE_HE_GUY:
	    case PM_ELECTRODE:
	    case PM_DEFORMED_CAT:
	    case PM_DEFORMED_GALLTRIT:
	    case PM_DISGUISED_SOLDIER_ANT:
	    case PM_NETZAH_SEPHIRAH:
	    case PM_AMORPHOUS_FISH:
	    case PM_GREATER_MIMIC:
	    case PM_GREATER_PERMAMIMIC:
	    case PM_DECEPTIVE_ORC:
	    case PM_MASTER_MIMIC:
	    case PM_UNDEAD_SWARM_MIMIC:
	    case PM_FLOATING_MIMIC:
	    case PM_MASTER_PERMAMIMIC:
	    case PM_UNDEAD_SWARM_PERMAMIMIC:
	    case PM_FLOATING_PERMAMIMIC:
	    case PM_OLOG_HAI_MIMIC:
	    case PM_OLOG_HAI_PERMAMIMIC:
	    case PM_DWARF_KING_PERMAMIMIC:
	    case PM_PLAYER_MIMIC:
	    case PM_PLAYER_PERMAMIMIC:
	    case PM_VESTY:
	    case PM_CAMO_DWARF:
	    case PM_COCOON_IMP:
	    case PM_HALLUCINATION_IMAGE:
	    case PM_CAMO_RUBBER:
	    case PM_WIPER_RUBBER:
	    case PM_BLENDING_TITANOTHERE:
	    case PM_DRONING_UFO_PART:
	    case PM_GNOSIS_SEPHIRAH:
	    case PM_HIDDEN_MOLD:
	    case PM_SECLUDED_MOLD:
	    case PM_HIDDEN_FUNGUS:
	    case PM_SECLUDED_FUNGUS:
	    case PM_HIDDEN_PATCH:
	    case PM_SECLUDED_PATCH:
	    case PM_HIDDEN_FORCE_FUNGUS:
	    case PM_SECLUDED_FORCE_FUNGUS:
	    case PM_HIDDEN_FORCE_PATCH:
	    case PM_SECLUDED_FORCE_PATCH:
	    case PM_HIDDEN_WARP_FUNGUS:
	    case PM_SECLUDED_WARP_FUNGUS:
	    case PM_HIDDEN_WARP_PATCH:
	    case PM_SECLUDED_WARP_PATCH:
	    case PM_HIDDEN_STALK:
	    case PM_SECLUDED_STALK:
	    case PM_HIDDEN_SPORE:
	    case PM_SECLUDED_SPORE:
	    case PM_HIDDEN_MUSHROOM:
	    case PM_SECLUDED_MUSHROOM:
	    case PM_HIDDEN_GROWTH:
	    case PM_SECLUDED_GROWTH:
	    case PM_HIDDEN_COLONY:
	    case PM_SECLUDED_COLONY:
	    case PM_MARSUPILAMI:
	    case PM_DISGUISED_UNDEAD_UNICORN:
	    case PM_UPGRADED_SECRET_CAR:
	    case PM_EXPENSIVE_SECRET_CAR:
	    case PM_MAELSTROM:
	    case PM_TROLL_PERMAMIMIC_MUMMY:
	    case PM_CHEATING_BLACK_LIGHT:
	    case PM_MIMIC_HIVEMIND:
	    case PM_PERMAMIMIC_HIVEMIND:
	    case PM_UNIDENTIFIED_RAVEN:
	    case PM_ARTILLERY_DRAGON:
	    case PM_EVIL_PATCH_MINOTAUR:
	    case PM_MIMIC_LICH:
	    case PM_CAMO_RUST_MONSTER:
	    case PM_DEMILICH_PERMAMIMIC:
	    case PM_RUNNING_COBRA:
	    case PM_UMBER_MIMIC:
	    case PM_PORTER_XORN_PERMAMIMIC:
	    case PM_ROTTEN_FOOD_RATION:
	    case PM_DSCHIIM_RUBBER:

	    case PM_LESSER_INTRINSIC_EATING_MIMIC:
	    case PM_INTRINSIC_EATING_MIMIC:
	    case PM_MASTER_INTRINSIC_EATING_MIMIC:
	    case PM_LESSER_INTRINSIC_EATING_PERMAMIMIC:
	    case PM_INTRINSIC_EATING_PERMAMIMIC:
	    case PM_MASTER_INTRINSIC_EATING_PERMAMIMIC:
		tmp += 10;
	    case PM_GIANT_MIMIC:
	    case PM_GIANT_GROUP_MIMIC:
	    case PM_GIANT_SPAWN_MIMIC:
	    case PM_GIANT_PERMAMIMIC:
	    case PM_GIANT_GROUP_PERMAMIMIC:
	    case PM_GIANT_SPAWN_PERMAMIMIC:
		tmp += 10;
		/* fall into next case */
	    case PM_LARGE_MIMIC:
	    case PM_LARGE_PHASING_MIMIC:
	    case PM_LARGE_SPAWN_MIMIC:
	    case PM_LARGE_PERMAMIMIC:
	    case PM_LARGE_PHASING_PERMAMIMIC:
	    case PM_LARGE_SPAWN_PERMAMIMIC:
		tmp += 10;
		/* fall into next case */
	    case PM_MIMIC:
	    case PM_PETIT_MIMIC:
	    case PM_SMALL_MIMIC:
	    case PM_SMALL_HEALING_MIMIC:
	    case PM_SMALL_FLYING_MIMIC:
	    case PM_PERMAMIMIC:
	    case PM_PETIT_PERMAMIMIC:
	    case PM_SMALL_PERMAMIMIC:
	    case PM_SMALL_HEALING_PERMAMIMIC:
	    case PM_SMALL_FLYING_PERMAMIMIC:
		tmp += 10;
		if (youmonst.data->mlet != S_MIMIC && !Unchanging) {
		    char buf[BUFSZ];
		    You_cant("resist the temptation to mimic %s.",
			Hallucination ? "an orange" : "a pile of gold");
#ifdef STEED
                    /* A pile of gold can't ride. */
		    if (u.usteed) dismount_steed(DISMOUNT_FELL);
#endif
		    nomul(-tmp, "pretending to be a pile of gold");
		    Sprintf(buf, Hallucination ?
			"You suddenly dread being peeled and mimic %s again!" :
			"You now prefer mimicking %s again.",
			an( (Upolyd && !missingnoprotect) ? youmonst.data->mname : urace.noun));
		    eatmbuf = strcpy((char *) alloc(strlen(buf) + 1), buf);
		    nomovemsg = eatmbuf;
		    afternmv = eatmdone;
		    /* ??? what if this was set before? */
		    youmonst.m_ap_type = M_AP_OBJECT;
		    youmonst.mappearance = Hallucination ? ORANGE : GOLD_PIECE;
		    newsym(u.ux,u.uy);
		    curs_on_u();
		    /* make gold symbol show up now */
		    display_nhwindow(WIN_MAP, TRUE);
		}
		break;
	    case PM_QUANTUM_MECHANIC:
	    case PM_GORGON_BEETLE:
	    case PM_SPEED_TROVE:
	    case PM_MINI_CHICKATRICE:
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
	    case PM_PREHISTORIC_CAVE_LIZARD:
			lesshungry(500); /* fall thru */
	    case PM_GIANT_LIZARD:
			lesshungry(300); /* fall thru */
	    case PM_CHAOS_LIZARD:
	    case PM_LIZARD_MAN:
			lesshungry(180); /* fall thru */
	    case PM_LIZARD:
	    case PM_ROCK_LIZARD:
	    case PM_BABY_CAVE_LIZARD:
	    case PM_NIGHT_LIZARD:
	    case PM_KARMIC_LIZARD:
	    case PM_CAVE_LIZARD:
	    case PM_GRASS_LIZARD:
	    case PM_BLUE_LIZARD:
	    case PM_SWAMP_LIZARD:
	    case PM_SPITTING_LIZARD:
	    case PM_LIZARD_EEL:
	    case PM_LIZARD_KING:
	    case PM_EEL_LIZARD:
	    case PM_ANTI_STONE_LIZARD:
	    case PM_HIDDEN_LIZARD:
	    case PM_CLINGING_LIZARD:
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

	    case PM_HUGE_LIZARD:
	    case PM_SAND_TIDE:
	    case PM_FBI_AGENT:
	    case PM_OWN_SMOKE:
	    case PM_GRANDPA:
	    case PM_FIRE_LIZARD:
	    case PM_BLACK_LIZARD:
	    case PM_LIGHTNING_LIZARD:
	    case PM_ICE_LIZARD:
			lesshungry(120);
		if (HStun > 2)  make_stunned(2L,FALSE);
		if (HConfusion > 2)  make_confused(2L,FALSE);
		break;

	    case PM_MIMIC_LIZARD:
	    case PM_DEFORMED_LIZARD:
	    case PM_GRATING_CHICKEN:
			lesshungry(20); /* fall thru */
		if (HStun > 2)  make_stunned(2L,FALSE);
		if (HConfusion > 2)  make_confused(2L,FALSE);

		tmp += 10;
		if (youmonst.data->mlet != S_MIMIC && !Unchanging) {
		    char buf[BUFSZ];
		    You_cant("resist the temptation to mimic %s.",
			Hallucination ? "an orange" : "a pile of gold");
#ifdef STEED
                    /* A pile of gold can't ride. */
		    if (u.usteed) dismount_steed(DISMOUNT_FELL);
#endif
		    nomul(-tmp, "pretending to be a pile of gold");
		    Sprintf(buf, Hallucination ?
			"You suddenly dread being peeled and mimic %s again!" :
			"You now prefer mimicking %s again.",
			an( (Upolyd && !missingnoprotect) ? youmonst.data->mname : urace.noun));
		    eatmbuf = strcpy((char *) alloc(strlen(buf) + 1), buf);
		    nomovemsg = eatmbuf;
		    afternmv = eatmdone;
		    /* ??? what if this was set before? */
		    youmonst.m_ap_type = M_AP_OBJECT;
		    youmonst.mappearance = Hallucination ? ORANGE : GOLD_PIECE;
		    newsym(u.ux,u.uy);
		    curs_on_u();
		    /* make gold symbol show up now */
		    display_nhwindow(WIN_MAP, TRUE);
		}
		break;

	    case PM_CHAMELEON:
	    case PM_DOPPELGANGER:
	    case PM_DOPPLEZON:
	    case PM_TRANSFORMER:
	    case PM_CHAOS_SHAPECHANGER:
	    case PM_WARPER:
	 /* case PM_SANDESTIN: */
	    case PM_GIANT_CHAMELEON:
	    case PM_MEDUQUASIT:

		/* evil patch idea by jonadab: eating shapeshifters has a 5% chance of conveying polymorphitis */
		if (!rn2(20)) {

			if(!(HPolymorph & FROMOUTSIDE)) {
				You_feel(Hallucination ?
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
	    case PM_HENRIETTA_S_THICK_BLOCK_HEELED_BOOT:

		if(!(HPolymorph & FROMOUTSIDE)) {
			You_feel(Hallucination ?
			    "able to take on different shapes! Yeah! Let's transform into something fun!" :
			    "unstable.");
			HPolymorph  |= FROMOUTSIDE;
		}

		break;
	    case PM_GENETIC_ENGINEER: /* Robin Johnson -- special msg */
	    case PM_ARMED_COCKATRICE:
	    case PM_PETROLICH:
		if (!Unchanging) {
		    You("undergo a freakish metamorphosis!");
		    polyself(FALSE);
		}
		break;

	    case PM_BETTINA_S_COMBAT_BOOT:

		pline("Ulch - the heel was tainted!");
	      make_sick(rn1(25,25), "a tainted combat boot", TRUE, SICK_VOMITABLE);

		break;

	    case PM_OLOG_HAI_GORGON:
	    case PM_BEAR_TROVE:
	    case PM_GRAVITY_CHICKEN:
	    case PM_MEDUSA_S_PET_FISH:
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;
		gainstr((struct obj *)0, 0);
		pline(Hallucination ? "You feel like ripping out some trees!" : "You feel stronger!");
		break;

	    case PM_NEXUS_CHICKEN:

		pline("This corpse can grant both teleportitis and teleport control. It can also petrify you, though.");
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
	    case PM_MASTER_BLASTER:
	    case PM_NECROMORB:
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
				pline(Hallucination ? "Hmm, is that what human brain tastes like?" : "Yum! That was real brain food!");
				(void) adjattrib(A_INT, 1, FALSE);
				break;	/* don't give them telepathy, too */
			}
		}
		else {
			pline(Hallucination ? "Eek, that tasted like rotten oversalted seaweed!" : "For some reason, that tasted bland.");
		}
		}
		break;

		/* WAC all mind flayers as per mondata.h have to be here */
	    case PM_HUMAN_WEREMINDFLAYER:
		catch_lycanthropy = TRUE;
		if (!Race_if(PM_HUMAN_WEREWOLF) && !Race_if(PM_AK_THIEF_IS_DEAD_) && !Role_if(PM_LUNATIC)) u.ulycn = PM_WEREMINDFLAYER;
		/* fall through */

	    case PM_INTELLIGENT_TROVE:
	    case PM_MASTER_MIND_FLAYER:
	    case PM_HUGE_MIND_FLAYER:
	    case PM_MINOR_MIND_FLAYER:
	    case PM_LOW_MIND_FLAYER:
	    case PM_LARGE_MIND_FLAYER:
	    case PM_EVIL_MIND_FLAYER:
	    case PM_TELEMINDFLAYER:
	    case PM_GIANT_MIND_FLAYER:
	    case PM_ARCH_MIND_FLAYER:
	    case PM_UNDEAD_MIND_FLAYER:
	    case PM_ELDRITCH_MIND_FLAYER:
	    case PM_GIGANTIC_MIND_FLAYER:
	    case PM_GRANDMASTER_MIND_FLAYER:
	    case PM_NASTY_MASTER_MIND_FLAYER:
	    case PM_ILLITHID:
	    case PM_MIND_BEAMER:
		case PM_COCKATRICE:
		case PM_PETTY_COCKATRICE:
		case PM_BASILISK:
		case PM_PETTY_MIND_FLAYER:
		case PM_ICKY_FACE:
		case PM_BRAIN_EATER:
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
		case PM_PASSIVE_MIND_FLAYER:
	    case PM_MIND_FLAYER: {
		if (u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) break;
#if 0
		int     temp;
		temp = urole.attrmax[A_INT];
#endif
		if (ABASE(A_INT) < ATTRMAX(A_INT)) {
			if (!rn2(2)) {
				pline(Hallucination ? "Hmm, is that what human brain tastes like?" : "Yum! That was real brain food!");
				(void) adjattrib(A_INT, 1, FALSE);
				break;	/* don't give them telepathy, too */
			}
		}
		else {
			pline(Hallucination ? "Eek, that tasted like rotten oversalted seaweed!" : "For some reason, that tasted bland.");
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
		if(is_giant(ptr) && !rn2(4) && !(u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) ) {gainstr((struct obj *)0, 0); pline(Hallucination ? "You feel like ripping out some trees!" : "You feel stronger!"); }

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

	/* Charisma is next to impossible to raise, so eating nymphs will help now. --Amy */

		if (ptr->mlet == S_NYMPH) {
		 if (ABASE(A_CHA) < ATTRMAX(A_CHA)) {
			if (!rn2(10) && !(u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) ) {
				pline("You feel more %s!", flags.female ? "pretty" : "attractive");
				(void) adjattrib(A_CHA, 1, FALSE);
			}
		  }
		}

	/* Or eating item-stealers. --Amy */

		if (dmgtype(ptr, AD_SITM) || dmgtype(ptr, AD_SEDU) || dmgtype(ptr, AD_SSEX) ) {
		 if (ABASE(A_CHA) < ATTRMAX(A_CHA)) {
			if (!rn2(dmgtype(ptr, AD_SSEX) ? 3 : 10)  && !(u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) ) {
				pline("You feel more %s!", flags.female ? "pretty" : "attractive");
				(void) adjattrib(A_CHA, 1, FALSE);
			}
		  }
		}

	/* luck is also harder to get; eating luck-reducing monsters sometimes grants a boost --Amy */
		if (dmgtype(ptr, AD_LUCK) && (ptr->mlevel > rn2(Race_if(PM_ILLITHID) ? 105 : 35) && rn2(4) && !(u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() )  ) ) {
			change_luck(1);
			pline("You feel lucky.");
		}

	/* or rarely, random attack monsters --Amy */
		if (dmgtype(ptr, AD_RBRE) && (ptr->mlevel > rn2(Race_if(PM_ILLITHID) ? 525 : 175) && rn2(4) && !(u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() )  ) ) {
			change_luck(1);
			pline("You feel lucky.");
		}

	/* and since mind flayers are so rare, improve INT gain a bit --Amy */
		if (dmgtype(ptr, AD_DRIN) && (ptr->mlevel > rn2(Race_if(PM_ILLITHID) ? 105 : 35) && rn2(4) && !(u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() )  ) ) {
		 if (ABASE(A_INT) < ATTRMAX(A_INT)) {
			pline("You feel smarter!");
			(void) adjattrib(A_INT, 1, 2);
		  }
		}

	/* psi-based enemies grant INT too --Amy */
		if (dmgtype(ptr, AD_SPC2) && (ptr->mlevel > rn2(Race_if(PM_ILLITHID) ? 105 : 35) && rn2(4) && !(u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() )  ) ) {
		 if (ABASE(A_INT) < ATTRMAX(A_INT)) {
			pline("You feel smarter!");
			(void) adjattrib(A_INT, 1, 2);
		  }
		}

	/* skill cap reducing monsters very rarely grant something good too --Amy */
		if (dmgtype(ptr, AD_SKIL) && (ptr->mlevel > rn2(Race_if(PM_ILLITHID) ? 1050 : 350) && rn2(2) && !(u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() )  ) ) {

			pline("You feel the RNG's touch...");

			int skillimprove = rnd(P_NUM_SKILLS);

			if (P_MAX_SKILL(skillimprove) == P_ISRESTRICTED) {
				unrestrict_weapon_skill(skillimprove);
				pline("You can now learn the %s skill.", P_NAME(skillimprove));
			} else if (P_MAX_SKILL(skillimprove) == P_UNSKILLED) {
				unrestrict_weapon_skill(skillimprove);
				P_MAX_SKILL(skillimprove) = P_BASIC;
				pline("You can now learn the %s skill.", P_NAME(skillimprove));
			} else if (rn2(2) && P_MAX_SKILL(skillimprove) == P_BASIC) {
				P_MAX_SKILL(skillimprove) = P_SKILLED;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(4) && P_MAX_SKILL(skillimprove) == P_SKILLED) {
				P_MAX_SKILL(skillimprove) = P_EXPERT;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(10) && P_MAX_SKILL(skillimprove) == P_EXPERT) {
				P_MAX_SKILL(skillimprove) = P_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(100) && P_MAX_SKILL(skillimprove) == P_MASTER) {
				P_MAX_SKILL(skillimprove) = P_GRAND_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else if (!rn2(200) && P_MAX_SKILL(skillimprove) == P_GRAND_MASTER) {
				P_MAX_SKILL(skillimprove) = P_SUPREME_MASTER;
				pline("Your knowledge of the %s skill increases.", P_NAME(skillimprove));
			} else pline("You feel no different than before.");

		}

	/* eating anything that spouts fake messages will give another one --Amy */
		if (dmgtype(ptr, AD_FAKE)) {
			pline(fauxmessage());
			if (!rn2(3)) pline(fauxmessage());
		}

	/* AD_HEAL monsters are rare; let's make them give one extra max HP --Amy */
		if (dmgtype(ptr, AD_HEAL) && !(u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) ) {
			if (Upolyd) {
				u.mh++;
				u.mhmax++;
			} else {
				u.uhp++;
				u.uhpmax++;
			}
		    You_feel("vitalized.");
		    flags.botl = 1;
		}

	/* And since I'm nice, do a similar thing for mana --Amy */
		if (dmgtype(ptr, AD_MANA) && !(u.uprops[NONINTRINSIC_EFFECT].extrinsic || Nonintrinsics || have_nonintrinsicstone() ) ) {
			u.uenmax++;
			pline("You feel a mild buzz.");
		    flags.botl = 1;
		}

	/* Dream eaters, on the other hand, are bad to eat. --Amy */
		if (dmgtype(ptr, AD_DREA) && !multi) {
		    pline("Suddenly you have a nightmare!");
		    nomul(-5, "scared by a nightmare");
		    nomovemsg = 0;
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
	if ((u.uhs == SATIATED) && ((Role_if(PM_KNIGHT) && u.ualign.type == A_LAWFUL) || Role_if(PM_CHEVALIER) || Race_if(PM_VEELA) || Role_if(PM_PALADIN) || Role_if(PM_TOPMODEL) || Role_if(PM_FAILED_EXISTENCE) || Role_if(PM_GOFF)) ) {
			adjalign(-3);		/* gluttony is unchivalrous */
		You(Hallucination ? "feel that your belly's gonna burst!" : "feel like a glutton!");
	}

	if ((u.uhs == SATIATED) && (Race_if(PM_VEELA) || Role_if(PM_FAILED_EXISTENCE)) ) {	badeffect();
	}

	if (u.uhs == SATIATED && (Role_if(PM_TOPMODEL) || Role_if(PM_GOFF)) ) { /* They aren't used to eat much. --Amy */

	if(!rn2(4)) {
		if (Hallucination) You_feel("rather trippy.");
		else You_feel("rather %s.", body_part(LIGHT_HEADED));
		make_confused(HConfusion + d(2,4),FALSE);
	} else if(!rn2(5)) {
		if (Hallucination) You_feel("uncontrollable."); /* this and some other effects added by Amy */
		else You_feel("stunned.");
		make_stunned(HStun + d(2,4),FALSE);
	} else if(!rn2(5)) {
		if (Hallucination) You_feel("even weirder!");
		else You_feel("weirded out!");
		make_hallucinated(HHallucination + d(2,10),FALSE,0L);
	} else if(!rn2(4)) {
		if (!Blind) pline("Everything suddenly goes dark.");
		make_blinded(Blinded+d(2,10),FALSE);
		if (!Blind) Your(vision_clears);
	} else if(!rn2(8)) {
		if (Hallucination) You_feel("like your body is trying to fall asleep!");
		else You_feel("numb.");
		make_numbed(HNumbed + d(2,15),FALSE);
	} else if(!rn2(10)) {
		if (Hallucination) pline("Crap! A %s suddenly appeared! RUN!",rndmonnam() );
		else You_feel("fear spreading through your body.");
		make_feared(HFeared + d(2,15),FALSE);
	} else if(!rn2(10)) {
		if (Hallucination) You_feel("like your on fire despite there being no heat!");
		else You_feel("like you're on fire!");
		make_burned(HBurned + d(2,15),FALSE);
	} else if(!rn2(20)) {
		if (Hallucination) You_feel("a giant ice cream cone enclosing you!");
		else pline("You're getting the chills.");
		make_frozen(HFrozen + d(2,15),FALSE);
	} else if(!rn2(10)) {
		if (Hallucination) You_feel("like you have dementia tremor!"); /* not a real name --Amy */
		else pline("Your hands start trembling violently!");
		incr_itimeout(&Glib, d(2,4) );
	} else if(!rn2(10)) {
		if (Hallucination) You_feel("totally down! Seems you tried some illegal shit!");
		else pline("You feel like you're going to throw up.");
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
		    what = "you slap against the", where =
#ifdef STEED
			   (u.usteed) ? "saddle" :
#endif
			   surface(u.ux,u.uy);
		pline_The("world spins and %s %s.", what, where);
		flags.soundok = 0;
		nomul(-rnd(10), "unconscious from forgetting your anorexia conduct");
		nomovemsg = "You are conscious again.";
		afternmv = Hear_again;
		return;
	} else if(!rn2(50) && !Sick) { /* The chance of this outcome !MUST! be low. Everything else would be unfair. --Amy */
	    make_sick(rn1(25,25), "rotten food", TRUE, SICK_VOMITABLE);
	} else if(!rn2(200) && !Slimed && !issoviet && !flaming(youmonst.data) && !Unchanging && !slime_on_touch(youmonst.data) ) { /* This chance should be even lower. --Amy */
		    Slimed = 100L;
		    flags.botl = 1;
		    killer_format = KILLED_BY_AN;
		    delayed_killer = "slimy meal";
	}

	}



}

void
violated_vegetarian()
{
    u.uconduct.unvegetarian++;
    if (Role_if(PM_MONK) || Role_if(PM_TOPMODEL) || Role_if(PM_FAILED_EXISTENCE) || Role_if(PM_GOFF) || Race_if(PM_SYLPH) ) {
	You_feel("guilty.");
	adjalign(-5);
    }
	if (Role_if(PM_FAILED_EXISTENCE)) badeffect();

	if (Role_if(PM_TOPMODEL) || Role_if(PM_GOFF)) { /* Their metabolism isn't used to meat. --Amy */

	if(!rn2(4)) {
		if (Hallucination) You_feel("rather trippy.");
		else You_feel("rather %s.", body_part(LIGHT_HEADED));
		make_confused(HConfusion + d(2,4),FALSE);
	} else if(!rn2(5)) {
		if (Hallucination) You_feel("uncontrollable."); /* this and some other effects added by Amy */
		else You_feel("stunned.");
		make_stunned(HStun + d(2,4),FALSE);
	} else if(!rn2(5)) {
		if (Hallucination) You_feel("even weirder!");
		else You_feel("weirded out!");
		make_hallucinated(HHallucination + d(2,10),FALSE,0L);
	} else if(!rn2(4)) {
		if (!Blind) pline("Everything suddenly goes dark.");
		make_blinded(Blinded+d(2,10),FALSE);
		if (!Blind) Your(vision_clears);
	} else if(!rn2(8)) {
		if (Hallucination) You_feel("like your body is trying to fall asleep!");
		else You_feel("numb.");
		make_numbed(HNumbed + d(2,15),FALSE);
	} else if(!rn2(10)) {
		if (Hallucination) pline("Crap! A %s suddenly appeared! RUN!",rndmonnam() );
		else You_feel("fear spreading through your body.");
		make_feared(HFeared + d(2,15),FALSE);
	} else if(!rn2(10)) {
		if (Hallucination) You_feel("like your on fire despite there being no heat!");
		else You_feel("like you're on fire!");
		make_burned(HBurned + d(2,15),FALSE);
	} else if(!rn2(20)) {
		if (Hallucination) You_feel("a giant ice cream cone enclosing you!");
		else pline("You're getting the chills.");
		make_frozen(HFrozen + d(2,15),FALSE);
	} else if(!rn2(10)) {
		if (Hallucination) You_feel("like you have dementia tremor!"); /* not a real name --Amy */
		else pline("Your hands start trembling violently!");
		incr_itimeout(&Glib, d(2,4) );
	} else if(!rn2(10)) {
		if (Hallucination) You_feel("totally down! Seems you tried some illegal shit!");
		else pline("You feel like you're going to throw up.");
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
		    what = "you slap against the", where =
#ifdef STEED
			   (u.usteed) ? "saddle" :
#endif
			   surface(u.ux,u.uy);
		pline_The("world spins and %s %s.", what, where);
		flags.soundok = 0;
		nomul(-rnd(10), "unconscious from eating meat");
		nomovemsg = "You are conscious again.";
		afternmv = Hear_again;
		return;
	} else if(!rn2(50) && !Sick) { /* The chance of this outcome !MUST! be low. Everything else would be unfair. --Amy */
	    make_sick(rn1(25,25), "rotten food", TRUE, SICK_VOMITABLE);
	} else if(!rn2(200) && !Slimed && !issoviet && !flaming(youmonst.data) && !Unchanging && !slime_on_touch(youmonst.data) ) { /* This chance should be even lower. --Amy */
		    Slimed = 100L;
		    flags.botl = 1;
		    killer_format = KILLED_BY_AN;
		    delayed_killer = "slimy meal";
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

	if(!carried(tin.tin) && !obj_here(tin.tin, u.ux, u.uy))
					/* perhaps it was stolen? */
		return(0);		/* %% probably we should use tinoid */
	if(tin.usedtime++ >= 50) {
		You(Hallucination ? "get bored while playing with that dildo-like thing." : "give up your attempt to open the tin.");
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
	You(Hallucination ? "open it! Now let's see the contents, maybe it's some acid?" : "succeed in opening the tin.");
	if(tin.tin->spe != 1) {
	    if (tin.tin->corpsenm == NON_PM) {
		pline(Hallucination ? "Nothing in there, might as well throw it in the trash can." : "It turns out to be empty.");
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
#ifdef EATEN_MEMORY
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
#endif
	    pline("It smells like %s%s.", (which == 2) ? "the " : "", what);

	    if (yn("Eat it?") == 'n') {
#ifdef EATEN_MEMORY
	    	/* ALI - you know the tin iff you recognized the contents */
		if (mvitals[tin.tin->corpsenm].eaten)
#endif
		if (!Hallucination) tin.tin->dknown = tin.tin->known = TRUE;
		if (flags.verbose) You("discard the open tin.");
		costly_tin((const char*)0);
		goto use_me;
	    }
	    /* in case stop_occupation() was called on previous meal */
	    victual.piece = (struct obj *)0;
	    victual.fullwarn = victual.eating = victual.doreset = FALSE;

#ifdef EATEN_MEMORY
	    /* WAC - you only recognize if you've eaten this before */
	    You("consume %s %s.", tintxts[r].txt,
				mvitals[tin.tin->corpsenm].eaten ?
				mons[tin.tin->corpsenm].mname : "food");
#else
	    You("consume %s %s.", tintxts[r].txt,
			mons[tin.tin->corpsenm].mname);
#endif

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

#ifdef EATEN_MEMORY
	    if (mvitals[tin.tin->corpsenm].eaten)
#endif
	    tin.tin->dknown = tin.tin->known = TRUE;
	    cprefx(tin.tin->corpsenm); cpostfx(tin.tin->corpsenm);

	    /* charge for one at pre-eating cost */
	    costly_tin((const char*)0);

	    /* check for vomiting added by GAN 01/16/87 */
	    if(tintxts[r].nut < 0) {make_vomiting((long)rn1(15,10), FALSE);
			if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */
			if (!issoviet) lesshungry(tintxts[r].nut);
		}
	    else lesshungry(tintxts[r].nut);

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
	} else {
	    if (tin.tin->cursed)
		pline("It contains some decaying%s%s substance.",
			Blind ? "" : " ", Blind ? "" : hcolor(NH_GREEN));
	    else
		pline(Hallucination ? "It contains potato tack or something like that." : "It contains spinach.");

	    if (yn("Eat it?") == 'n') {
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
		      Hallucination ? "Swee'pea" : "Popeye");
	    lesshungry(600);
	    gainstr(tin.tin, 0);
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
		return;
	} else if (otmp->blessed) {
		pline_The(Hallucination ? "tin is opened by the little man sitting inside!" : "tin opens like magic!");
		tmp = 1;
	} else if(uwep) {
		switch(uwep->otyp) {
		case TIN_OPENER:
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
		case DROVEN_DAGGER:
			tmp = 3;
			break;
		case PICK_AXE:
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
		pline(Hallucination ? "Using your fingernails, you try to open this tin." : "It is not so easy to open this tin.");
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
	if (!Deafness && !u.uprops[DEAFNESS].extrinsic && !have_deafnessstone() ) flags.soundok = 1;
	return 0;
}

/* called on the "first bite" of rotten food */
STATIC_OVL int
rottenfood(obj)
struct obj *obj;
{
	pline("Blecch!  Rotten %s!", foodword(obj));
	if (issoviet) pline("Tip bloka l'da vinovat v etom!");
	if(!rn2(4)) {
		if (Hallucination) You_feel("rather trippy.");
		else You_feel("rather %s.", body_part(LIGHT_HEADED));
		make_confused(HConfusion + d(2,4),FALSE);
	} else if(!rn2(5)) {
		if (Hallucination) You_feel("uncontrollable."); /* this and some other effects added by Amy */
		else You_feel("stunned.");
		make_stunned(HStun + d(2,4),FALSE);
	} else if(!rn2(5)) {
		if (Hallucination) You_feel("even weirder!");
		else You_feel("weirded out!");
		make_hallucinated(HHallucination + d(2,10),FALSE,0L);
	} else if(!rn2(4)) {
		if (!Blind) pline("Everything suddenly goes dark.");
		make_blinded(Blinded+d(2,10),FALSE);
		if (!Blind) Your(vision_clears);
	} else if(!rn2(8)) {
		if (Hallucination) You_feel("like your body is trying to fall asleep!");
		else You_feel("numb.");
		make_numbed(HNumbed + d(2,15),FALSE);
	} else if(!rn2(10)) {
		if (Hallucination) pline("Crap! A %s suddenly appeared! RUN!",rndmonnam() );
		else You_feel("fear spreading through your body.");
		make_feared(HFeared + d(2,15),FALSE);
	} else if(!rn2(10)) {
		if (Hallucination) You_feel("like your on fire despite there being no heat!");
		else You_feel("like you're on fire!");
		make_burned(HBurned + d(2,15),FALSE);
	} else if(!rn2(20)) {
		if (Hallucination) You_feel("a giant ice cream cone enclosing you!");
		else pline("You're getting the chills.");
		make_frozen(HFrozen + d(2,15),FALSE);
	} else if(!rn2(10)) {
		if (Hallucination) You_feel("like you have dementia tremor!"); /* not a real name --Amy */
		else pline("Your hands start trembling violently!");
		incr_itimeout(&Glib, d(2,4) );
	} else if(!rn2(10)) {
		if (Hallucination) You_feel("totally down! Seems you tried some illegal shit!");
		else pline("You feel like you're going to throw up.");
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
		    what = "you slap against the", where =
#ifdef STEED
			   (u.usteed) ? "saddle" :
#endif
			   surface(u.ux,u.uy);
		pline_The("world spins and %s %s.", what, where);
		flags.soundok = 0;
		nomul(-rnd(10), "unconscious from rotten food");
		nomovemsg = "You are conscious again.";
		afternmv = Hear_again;
		return(1);
	} else if(!rn2(50) && !Sick) { /* The chance of this outcome !MUST! be low. Everything else would be unfair. --Amy */
	    make_sick(rn1(25,25), "rotten food", TRUE, SICK_VOMITABLE);
	} else if(!rn2(200) && !Slimed && !issoviet && !flaming(youmonst.data) && !Unchanging && !slime_on_touch(youmonst.data) ) { /* This chance should be even lower. --Amy */
		    Slimed = 100L;
		    flags.botl = 1;
		    killer_format = KILLED_BY_AN;
		    delayed_killer = "slimy meal";
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
	boolean stoneable = (touch_petrifies(&mons[mnum]) && !Stone_resistance &&
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
	    	pline(Hallucination ? "No juice left - gotta get a new bottle from your cellar!" : "There is no blood left in this corpse!");
	    	return 3;
	    } else if (rotted <= 0 &&
	      (peek_at_iced_corpse_age(otmp) + 5) >= monstermoves) {
		char buf[BUFSZ];

		/* Generate the name for the corpse */
		if (!uniq || Hallucination)
		    Sprintf(buf, "%s", the(corpse_xname(otmp,TRUE)));
		else
		    Sprintf(buf, "%s%s corpse",
			    !type_is_pname(&mons[mnum]) ? "the " : "",
			    s_suffix(mons[mnum].mname));

	    	pline("You drain the blood from %s.", buf);
		otmp->odrained = 1;
	    } else {
	    	pline(Hallucination ? "You try to suck, but it just tastes very disgusting!" : "The blood in this corpse has coagulated!");
	    	return 3;
	    }
	}
	else
	    otmp->odrained = 0;

	/* Very rotten corpse will make you sick unless you are a ghoul or a ghast */
	if (mnum != PM_ACID_BLOB && !stoneable && rotted > 5L) {
	    boolean cannibal = maybe_cannibal(mnum, FALSE);
	    if (u.umonnum == PM_GHOUL || u.umonnum == PM_GHAST || u.umonnum == PM_GASTLY || u.umonnum == PM_PHANTOM_GHOST || u.umonnum == PM_HAUNTER
		|| u.umonnum == PM_GENGAR || (Race_if(PM_GASTLY) && !Upolyd) || (Race_if(PM_PHANTOM_GHOST) && !Upolyd) ) {
	    	pline("Yum - that %s was well aged%s!",
		      mons[mnum].mlet == S_FUNGUS ? "fungoid vegetation" :
		      !vegetarian(&mons[mnum]) ? "meat" : "protoplasm",
		      cannibal ? ", cannibal" : "");
	    } else {	    
		pline("Ulch - that %s was tainted%s!",
		      mons[mnum].mlet == S_FUNGUS ? "fungoid vegetation" :
		      !vegetarian(&mons[mnum]) ? "meat" : "protoplasm",
		      cannibal ? ", cannibal" : "");
		if (Sick_resistance) {
			pline(Hallucination ? "Interesting taste, though..." : "It doesn't seem at all sickening, though...");
		} else {
			char buf[BUFSZ];
			long sick_time;

			sick_time = (long) rn1(20, 20);
			/* make sure new ill doesn't result in improvement */
			if (Sick && (sick_time > Sick))
			    sick_time = (Sick > 1L) ? Sick - 1L : 1L;
			if (!uniq || Hallucination)
			    Sprintf(buf, "rotted %s", corpse_xname(otmp,TRUE));
			else
			    Sprintf(buf, "%s%s rotted corpse",
				    !type_is_pname(&mons[mnum]) ? "the " : "",
				    s_suffix(mons[mnum].mname));
			make_sick(sick_time, buf, TRUE, SICK_VOMITABLE);
		}
		cpostfx(mnum);
		if (carried(otmp)) useup(otmp);
		else useupf(otmp, 1L);
		return(2);
	    }
	} else if (youmonst.data == &mons[PM_GHOUL] || 
	youmonst.data == &mons[PM_GASTLY] || youmonst.data == &mons[PM_HAUNTER] || youmonst.data == &mons[PM_GENGAR] || youmonst.data == &mons[PM_PHANTOM_GHOST] || 
		   youmonst.data == &mons[PM_GHAST] || (Race_if(PM_GASTLY) && !Upolyd) || (Race_if(PM_PHANTOM_GHOST) && !Upolyd) ) {
		pline (Hallucination ? "You can't seem to find any manky bits!" : "This corpse is too fresh!");
		return 3;
	} else if (acidic(&mons[mnum]) && !Acid_resistance) {
		tp++;
		You("have a very bad case of stomach acid."); /* not body_part() */
		losehp(rnd(15 + ptr->mlevel), "acidic corpse", KILLED_BY_AN);
	} else if (poisonous(&mons[mnum]) && rn2(5)) {
		tp++;
		pline(Hallucination ? "Feels like your face is turning green!" : "Ecch - that must have been poisonous!");
		if(!Poison_resistance) {
			if (!rn2(3)) losestr(rnd(2)); /* tone down strength loss, since you have to eat many more poisonous */
			if (!rn2(60)) losestr(rnd(2)); /* corpses in order to get poison resistance --Amy */
			losehp(rnd(15 + ptr->mlevel), "poisonous corpse", KILLED_BY_AN);
		} else	You("seem unaffected by the poison.");
	/* now any corpse left too long will make you mildly ill */
	} else if ((rotted > 5L || (rotted > 3L && rn2(5)))
					&& !Sick_resistance) {
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

#ifdef EATEN_MEMORY
	/* WAC Track food types eaten */
	if (mvitals[mnum].eaten < 255) mvitals[mnum].eaten++;
#endif

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

	Sprintf(msgbuf, "%s %s", otmp->odrained ? "draining" : "eating",
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
		    pline(Hallucination ? "Oh wow, like, superior, man!" :
			  "That food really hit the spot!");
		else if(u.uhunger <= 1200) pline("That satiated your %s!",
						body_part(STOMACH));

		if (otmp && otmp->oartifact == ART_HOE_PA) {
			if (!Cold_resistance) pline("You feel more resistant to cold!");
			incr_itimeout(&HCold_resistance, rnz(10000));
			if (!Fire_resistance) pline("You feel more resistant to fire!");
			incr_itimeout(&HFire_resistance, rnz(10000));
			if (!Shock_resistance) pline("You feel more resistant to shock!");
			incr_itimeout(&HShock_resistance, rnz(10000));
			if (!Poison_resistance) pline("You feel more resistant to poison!");
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
		    pline(Hallucination ? "Tastes great! Less filling!" :
			  "Mmm, tripe... not bad!");
		else {
		    pline("Yak - dog food!");
#ifdef CONVICT
		    if (Role_if(PM_CONVICT))
			pline("At least it's not prison food.");
#endif /* CONVICT */
		    more_experienced(1,0);
		    newexplevel();
		    /* not cannibalism, but we use similar criteria
		       for deciding whether to be sickened by this meal */
		    if (rn2(2) && !CANNIBAL_ALLOWED())
#ifdef CONVICT
		    if (!Role_if(PM_CONVICT))
#endif /* CONVICT */
			{ make_vomiting((long)rn1(victual.reqtime, 14), FALSE);
			if (Sick && Sick < 100) 	set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */
			}
		}
		break;
	    case PILL:            
		You("swallow the little pink pill.");
		switch(rn2(7))
		{
		   case 0:

			if (rn2(100)) { /* make wishes much less common --Amy */

			    make_sick(rn1(15,15), "bad pill", TRUE,
			      SICK_VOMITABLE);
				break;
			}

			else {

			/* [Tom] wishing pills are from the Land of Oz */
			pline ("The pink sugar coating hid a silver wishing pill!");
			makewish();
			break;
			}
		   case 1:
			if(!Poison_resistance) {
				You("feel your stomach twinge.");
				losestr(rnd(4));
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
			if(Sleep_resistance) {
				pline("Hmm. Nothing happens.");
			} else {
				pline("You feel drowsy...");
				nomul(-rn2(50), "sleeping from a pink pill");
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
	       otmp->cursed ? (Hallucination ? "far-out!" : "terrible!") :
		      Hallucination ? "groovy!" : "delicious!");
		switch(rn2(10))
		{
		   case 0:
		   case 1:
			if(!Poison_resistance) {
				You("feel rather ill....");
				losestr(rnd(4));
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

		if (!rn2(3)) pline(Hallucination ? "...It tastes sweet, so, too sweet!" :
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
		if (hates_silver(youmonst.data)) {
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
		if (otmp->otyp == SLIME_MOLD && !otmp->cursed && !(FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone())
			&& otmp->spe == current_fruit)
		    pline("My, that was a %s %s!",
			  Hallucination ? "primo" : "yummy",
			  singular(otmp, xname));
		else
#ifdef UNIX
		if (otmp->otyp == APPLE || otmp->otyp == PEAR) {
		    if (!Hallucination) pline("Core dumped.");
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
		if (otmp->otyp == EGG && stale_egg(otmp)) {
		    pline(Hallucination ? "Uaaah! That tasted like hydrogen sulfide!" : "Ugh.  Rotten egg.");	/* perhaps others like it */
#ifdef CONVICT
		if (Role_if(PM_CONVICT) && (rn2(8) > u.ulevel)) {
		    You_feel("a slight stomach ache.");	/* prisoners are used to bad food */
		} else
#endif /* CONVICT */
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
		      bad_for_you ? (Hallucination ? "grody!" : "terrible!") :
		      (otmp->otyp == CRAM_RATION
		      || otmp->otyp == K_RATION
		      || otmp->otyp == C_RATION)
		      ? "bland." :
		      Hallucination ? "gnarly!" : "delicious!");
		}
		if (otmp->otyp == EGG && otmp->oartifact == ART_EGG_OF_SPLAT) {
		    pline("Ulch - that was a contaminated egg!");
		    make_sick(rn1(25,25), "Team Splat egg (Go Team Hardcore Autism!)", TRUE, SICK_VOMITABLE);
		}

		break;
	}
}

STATIC_OVL void
accessory_has_effect(otmp)
struct obj *otmp;
{
	pline("Magic spreads through your body as you digest the %s.",
	    otmp->oclass == RING_CLASS ? "ring" : "amulet");
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
	if (!rn2(otmp->oclass == RING_CLASS ? 5 : 10)) { /* lower chance, due to existence of sickness resistance etc --Amy */
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
				Hallucination ? "normal" : "strange");
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
		if (adjattrib(A_CHA, otmp->spe, -1))
		    makeknown(typ);
		break;
	    case RIN_GAIN_STRENGTH:
		accessory_has_effect(otmp);
		if (adjattrib(A_STR, otmp->spe, -1))
		    makeknown(typ);
		break;
	    case RIN_GAIN_CONSTITUTION:
		accessory_has_effect(otmp);
		if (adjattrib(A_CON, otmp->spe, -1))
		    makeknown(typ);
		break;
	    case RIN_GAIN_INTELLIGENCE:
		accessory_has_effect(otmp);
		if (adjattrib(A_INT, otmp->spe, -1))
		    makeknown(typ);
		break;
	    case RIN_GAIN_WISDOM:
		accessory_has_effect(otmp);
		if (adjattrib(A_WIS, otmp->spe, -1))
		    makeknown(typ);
		break;
	    case RIN_GAIN_DEXTERITY:
		accessory_has_effect(otmp);
		if (adjattrib(A_DEX, otmp->spe, -1))
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
		You("feel rather strange.");
		polyself(FALSE);
		flags.botl = 1;
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
	if (otmp->oclass == RING_CLASS || otmp->oclass == AMULET_CLASS)
		eataccessory(otmp);
	else if (otmp->otyp == LEASH && otmp->leashmon)
		o_unleash(otmp);

	/* KMH -- idea by "Tommy the Terrorist" */
	if ((otmp->otyp == TRIDENT) && !otmp->cursed && !(FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone()) )
	{
		pline(Hallucination ? "Four out of five dentists agree." :
				"That was pure chewing satisfaction!");
		exercise(A_WIS, TRUE);
	}
	if ((otmp->otyp == FLINT) && !otmp->cursed && !(FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone()) )
	{
		pline(Hallucination ? "Whoops, what's that grating sound? Was that a piece of your tooth?" : "Yabba-dabba delicious!");
		exercise(A_CON, TRUE);
	}

	if (Race_if(PM_WORM_THAT_WALKS) && otmp->otyp == FIGURINE) {/* chance to polymorph into the depicted monster --Amy */
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
static const char *foodwords[] = {
	"meal", "liquid", "wax", "food", "meat",
	"paper", "cloth", "leather", "wood", "bone", "scale",
	"iron", "metal", "copper", "silver", "gold", "platinum", "mithril",
	"plastic", "glass", "rich food", "stone"
};

STATIC_OVL const char *
foodword(otmp)
register struct obj *otmp;
{
	if (otmp->oclass == FOOD_CLASS) return "food";
	if (otmp->oclass == GEM_CLASS &&
	    objects[otmp->otyp].oc_material == GLASS &&
	    otmp->dknown)
		makeknown(otmp->otyp);
	return foodwords[objects[otmp->otyp].oc_material];
}

STATIC_OVL void
fpostfx(otmp)		/* called after consuming (non-corpse) food */
register struct obj *otmp;
{
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
				break;
			case 5:	/* ginger (cure hallucinate) */
				pline("Ginger! Who puts that in a chocolate???");
				(void) make_hallucinated(0L, TRUE, 0L);
				break;
			default: break;

		}

		break;

	    case HOLY_WAFER:            
		if (u.ualign.type == A_LAWFUL) {
			if (u.uhp < u.uhpmax) {
				You("feel warm inside.");
				u.uhp += rn1(20,20);
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			} 
		}
		if (Sick) make_sick(0L, (char *)0, TRUE, SICK_ALL);
		if (u.ulycn != -1) {
		    you_unwere(TRUE);
		}
		if (u.ualign.type == A_CHAOTIC) {
		    You("feel a burning inside!");
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
		break;

	    case PEAR:
		make_stunned(0L, TRUE);
		break;

	    case ASIAN_PEAR:
		make_confused(0L, TRUE);
		make_stunned(0L, TRUE);
		break;

	    case MAGIC_BANANA: /* cure all */
		make_confused(0L, TRUE);
		make_stunned(0L, TRUE);
		make_burned(0L, TRUE);
		make_feared(0L, TRUE);
		make_numbed(0L, TRUE);
		make_frozen(0L, TRUE);
		(void) make_hallucinated(0L, TRUE, 0L);
		make_blinded((long)u.ucreamed,TRUE);

		break;

	    /* body parts -- now checks for artifact and name*/
	    case EYEBALL:
		if (!otmp->oartifact) break;
		You("feel a burning inside!");
		u.uhp -= rn1(50,150);
		if (u.uhp <= 0) {
		  killer_format = KILLED_BY;
		  killer = food_xname(otmp, TRUE);
		  done(CHOKING);
		}
		break;

	    case UGH_MEMORY_TO_CREATE_INVENTORY:

		u.wormpolymorph = (NUMMONS + rnd(MISSINGNORANGE));
		u.ughmemory = 1;
		polyself(FALSE);

		break;

	    case SEVERED_HAND:
		if (!otmp->oartifact) break;
		You("feel the hand scrabbling around inside of you!");
		u.uhp -= rn1(50,150);
		if (u.uhp <= 0) {
		  killer_format = KILLED_BY;
		  killer = food_xname(otmp, TRUE);
		  done(CHOKING);
		}
		break;
	    case FORTUNE_COOKIE:

		if (rn2(2)) {	/* sometimes change the player's luck --Amy */

			pline("You feel your luck is turning...");
	
			if (bcsign(otmp) == -1)	change_luck(-1);
			else if (bcsign(otmp) == 0) change_luck(rn2(2) ? -1 : 1);
			else if (bcsign(otmp) == 1) change_luck(1);

		}

	    	if (yn("Read the fortune?") == 'y') {

			/* reading it might influence your luck --Amy */

			if (bcsign(otmp) == -1)	change_luck(-1);
			else if (rn2(2) && bcsign(otmp) == 0) change_luck(1);
			else if (bcsign(otmp) == 1) change_luck(1);

			outrumor(bcsign(otmp), BY_COOKIE);
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
			killer_format = KILLED_BY_AN;
			killer = "rotten lump of royal jelly";
			done(POISONING);
		    }
		}
		if(!otmp->cursed && !(FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone()) ) heal_legs();
		break;
	    case EGG:

		/* eating your own eggs is bad luck --Amy */
		if (otmp->spe && otmp->corpsenm >= LOW_PM) {
				pline(Hallucination ? "Great taste, like what chewing your own nails is like!" : "How are your children, you sorry excuse for a parent?");
			    change_luck(-2);
		}

		if (touch_petrifies(&mons[otmp->corpsenm])) {
		    if (!Stone_resistance &&
			!(poly_when_stoned(youmonst.data) && polymon(PM_STONE_GOLEM))) {
			if (!Stoned) Stoned = 7;
			killer_format = KILLED_BY_AN;
			Sprintf(killer_buf, "petrifying egg (%s)", mons[otmp->corpsenm].mname);
			delayed_killer = killer_buf;
		    }
		}
		break;
	    case EUCALYPTUS_LEAF:
		if (Sick && !otmp->cursed && !(FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone()) )
		    make_sick(0L, (char *)0, TRUE, SICK_ALL);
		if (Vomiting && !otmp->cursed && !(FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone()) )
		    make_vomiting(0L, TRUE);
		break;
	}

	if (!rn2(25)) { /* more random fortunes --Amy */
		if (yn("Somehow, a strip of paper appeared in your food! Read it?") == 'y') {

		/* reading it will influence the player's luck --Amy */

		if (bcsign(otmp) == -1)	change_luck(-1);
		else if (rn2(2) && bcsign(otmp) == 0) change_luck(1);
		else if (bcsign(otmp) == 1) change_luck(1);

		outrumor(bcsign(otmp), BY_PAPER);
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

	Strcpy(foodsmell, Tobjnam(otmp, "smell"));
	Strcpy(it_or_they, (otmp->quan == 1L) ? "it" : "they");
	Sprintf(eat_it_anyway, "Eat %s anyway?",
		(otmp->quan == 1L) ? "it" : "one");

	if (cadaver || otmp->otyp == EGG || otmp->otyp == TIN) {
		/* These checks must match those in eatcorpse() */
	  	stoneorslime = (touch_petrifies(&mons[mnum]) &&
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

	if (cadaver && mnum != PM_ACID_BLOB && rotted > 5L && !Sick_resistance) {
		/* Tainted meat */
		Sprintf(buf, "%s like %s could be tainted! %s",
			foodsmell, it_or_they, eat_it_anyway);
		if (yn_function(buf,ynchars,'n')=='n') return 1;
		else return 2;
	}
	if (stoneorslime) {
		Sprintf(buf, "%s like %s could be something very dangerous! %s",
			foodsmell, it_or_they, eat_it_anyway);
		if (yn_function(buf,ynchars,'n')=='n') return 1;
		else return 2;
	}
	if (otmp->orotten || (cadaver && rotted > 3L)) {
		/* Rotten */
		Sprintf(buf, "%s like %s could be rotten! %s",
			foodsmell, it_or_they, eat_it_anyway);
		if (yn_function(buf,ynchars,'n')=='n') return 1;
		else return 2;
	}
	if (cadaver && poisonous(&mons[mnum]) && !Poison_resistance) {
		/* poisonous */
		Sprintf(buf, "%s like %s might be poisonous! %s",
			foodsmell, it_or_they, eat_it_anyway);
		if (yn_function(buf,ynchars,'n')=='n') return 1;
		else return 2;
	}
	if (cadaver && !vegetarian(&mons[mnum]) &&
	    !u.uconduct.unvegetarian && (Role_if(PM_MONK) || Role_if(PM_TOPMODEL) || Role_if(PM_FAILED_EXISTENCE) || Role_if(PM_GOFF) || Race_if(PM_SYLPH) ) ) {
		Sprintf(buf, "%s unhealthy. %s",
			foodsmell, eat_it_anyway);
		if (yn_function(buf,ynchars,'n')=='n') return 1;
		else return 2;
	}
	if (cadaver && acidic(&mons[mnum]) && !Acid_resistance) {
		Sprintf(buf, "%s rather acidic. %s",
			foodsmell, eat_it_anyway);
		if (yn_function(buf,ynchars,'n')=='n') return 1;
		else return 2;
	}
	if (Upolyd && u.umonnum == PM_RUST_MONSTER &&
	    is_metallic(otmp) && otmp->oerodeproof) {
		Sprintf(buf, "%s disgusting to you right now. %s",
			foodsmell, eat_it_anyway);
		if (yn_function(buf,ynchars,'n')=='n') return 1;
		else return 2;
	}

	if (dmgtype(&mons[mnum], AD_STUN) || dmgtype(&mons[mnum], AD_HALU) || 
		    mnum == PM_VIOLET_FUNGUS || mnum == PM_VIOLET_STALK || mnum == PM_VIOLET_SPORE || mnum == PM_VIOLET_COLONY) { 
		Sprintf(buf, "%s like %s may be %s! %s", 
			foodsmell, it_or_they, 
			Hallucination ? "some real hard stuff" 
			: "hallucinogenic", eat_it_anyway); 
		if (yn_function(buf,ynchars,'n')=='n') return 1; 
		/*adjalign(-sgn(u.ualign.type));*/ /* you took it knowingly */ /*what the heck??? --Amy */
		return 2; 
	} 
	if (is_were(&mons[mnum]) && u.ulycn != mnum) { 
		Sprintf(buf, "%s like %s might be diseased. %s", 
			foodsmell, it_or_they, eat_it_anyway); 
		if (yn_function(buf,ynchars,'n')=='n') return 1; 
		else return 2; 
	} 

	/*
	 * Breaks conduct, but otherwise safe.
	 */
	 
	if (!u.uconduct.unvegan &&
	    ((material == LEATHER || material == BONE ||
	      material == EYEBALL || material == SEVERED_HAND ||
	      material == DRAGON_HIDE || material == WAX) ||
	     (cadaver && !vegan(&mons[mnum])))) {
		Sprintf(buf, "%s foul and unfamiliar to you. %s",
			foodsmell, eat_it_anyway);
		if (yn_function(buf,ynchars,'n')=='n') return 1;
		else return 2;
	}
	if (!u.uconduct.unvegetarian &&
	    ((material == LEATHER || material == BONE ||
	      material == EYEBALL || material == SEVERED_HAND ||
	      material == DRAGON_HIDE) ||
	     (cadaver && !vegetarian(&mons[mnum])))) {
		Sprintf(buf, "%s unfamiliar to you. %s",
			foodsmell, eat_it_anyway);
		if (yn_function(buf,ynchars,'n')=='n') return 1;
		else return 2;
	}

	if (cadaver && mnum != PM_ACID_BLOB && rotted > 5L && Sick_resistance) {
		/* Tainted meat with Sick_resistance */
		Sprintf(buf, "%s like %s could be tainted! %s",
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
		pline(Hallucination ? "You just can't seem to get it down your throat - is it too big?!" : "If you can't breathe air, how can you consume solids?");
		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	}
#ifdef JEDI
	if (uarmh && (uarmh->otyp == PLASTEEL_HELM || uarmh->otyp == HELM_OF_NO_DIGESTION || uarmh->otyp == HELM_OF_STORMS || uarmh->otyp == HELM_OF_DETECT_MONSTERS) ){
		pline("The %s covers your whole face.", xname(uarmh));
		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	}
#endif
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
	} else if ((otmp->owornmask & (W_ARMOR|W_TOOL|W_AMUL
#ifdef STEED
			|W_SADDLE
#endif
			)) != 0) {
	    /* let them eat rings */
	    You_cant("eat %s you're wearing.", something);
	    return 0;
	}
	if (is_metallic(otmp) &&
	    u.umonnum == PM_RUST_MONSTER && otmp->oerodeproof) {
	    	otmp->rknown = TRUE;
		if (otmp->quan > 1L) {
		    if(!carried(otmp))
			(void) splitobj(otmp, otmp->quan - 1L);
		    else
			otmp = splitobj(otmp, 1L);
		}
		pline("Ulch - That %s was rustproofed!", xname(otmp));
		/* The regurgitated object's rustproofing is gone now */
		otmp->oerodeproof = 0;
		make_stunned(HStun + rn2(10), TRUE);
		You("spit %s out onto the %s.", the(xname(otmp)),
			surface(u.ux, u.uy));
		if (carried(otmp)) {
			freeinv(otmp);
			dropy(otmp);
		}
		stackobj(otmp);
		return 1;
	}
	if (otmp->otyp == EYEBALL || otmp->otyp == SEVERED_HAND) {
	    Strcpy(qbuf,"Are you sure you want to eat that?");
	    if ((c = yn_function(qbuf, ynqchars, 'n')) != 'y') return 0;
	}
	if (otmp->otyp == UGH_MEMORY_TO_CREATE_INVENTORY) {
	    Strcpy(qbuf,"Did you read this item's description? Do you really want to eat it?");
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

	    victual.nmod = basenutrit;
	    victual.eating = TRUE; /* needed for lesshungry() */

	    material = objects[otmp->otyp].oc_material;
	    if (material == LEATHER ||
		material == EYEBALL || material == SEVERED_HAND ||
		material == BONE || material == DRAGON_HIDE) {
	 		u.uconduct.unvegan++;
	    		violated_vegetarian();
	    } else if (material == WAX)
			u.uconduct.unvegan++;
	    u.uconduct.food++;
		gluttonous();
	    
	    if (otmp->cursed || FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone())
		(void) rottenfood(otmp);

	    if (otmp->oclass == WEAPON_CLASS && otmp->opoisoned) {
		pline(Hallucination ? "Urgh - that tastes like cactus juice with full-length thorns in it!" : "Ecch - that must have been poisonous!");
		if(!Poison_resistance) {
		    losestr(rnd(4));
		    losehp(rnd(15), xname(otmp), KILLED_BY_AN);
		} else
		    You("seem unaffected by the poison.");
	    } else if (!otmp->cursed && !(FoodIsAlwaysRotten || u.uprops[FOOD_IS_ROTTEN].extrinsic || have_rottenstone()) )
		pline("This %s is delicious!",
		      otmp->oclass == COIN_CLASS ? foodword(otmp) :
		      singular(otmp, xname));

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
	    case FLESH:
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
	if ( (u.umonnum == PM_GHOUL || u.umonnum == PM_GHAST || u.umonnum == PM_GASTLY || u.umonnum == PM_PHANTOM_GHOST || u.umonnum == PM_HAUNTER || u.umonnum == PM_GENGAR || (Race_if(PM_GASTLY) && !Upolyd) || (Race_if(PM_PHANTOM_GHOST) && !Upolyd) ) && (u.uhunger < 2500) ) vampirenutrition += rn2(3);

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

	if ( (Role_if(PM_TOPMODEL) || Role_if(PM_FAILED_EXISTENCE) || Role_if(PM_GOFF)) && ( (rn2(2) && u.uhs == HUNGRY) || (rn2(4) && u.uhs == WEAK) || (rn2(8) && u.uhs == FAINTING) || (rn2(16) && u.uhs == FAINTED) ) ) return; /* They are used to eating very little. --Amy */

	if ((!u.usleep || !rn2(10))	/* slow metabolic rate while asleep */
		&& (carnivorous(youmonst.data) || herbivorous(youmonst.data) || metallivorous(youmonst.data) || lithivorous(youmonst.data))
#ifdef CONVICT
        /* Convicts can last twice as long at hungry and below */
        && (!Role_if(PM_CONVICT) || (moves % 2) || (u.uhs < HUNGRY))
#endif /* CONVICT */
		&& !Slow_digestion)
	    u.uhunger--;		/* ordinary food consumption */

	if (u.uprops[FAST_METABOLISM].extrinsic) u.uhunger--;
	if (have_metabolicstone()) u.uhunger--;
	if (FastMetabolismEffect) {

		int extrahungerpoints;

		u.uhunger--;

		extrahungerpoints = FastMetabolismEffect / 5000;
		if (extrahungerpoints) u.uhunger -= extrahungerpoints;
	}

	if (moves % 2) {	/* odd turns */
	    /* Regeneration uses up food, unless due to an artifact */
	    /*if (HRegeneration || ((ERegeneration & (~W_ART)) &&
				(ERegeneration != W_WEP || !uwep->oartifact)))
			u.uhunger--;*/

		/* rewrite by Amy */
		if (Regeneration) u.uhunger--;
		if (Energy_regeneration) { u.uhunger--; u.uhunger--;}

	    if (near_capacity() > SLT_ENCUMBER) u.uhunger--;
	} else {		/* even turns */
	    if (Hunger) u.uhunger--;
		if (Race_if(PM_CLOCKWORK_AUTOMATON)) u.uhunger--; /* to prevent =oSD from being overpowered --Amy */
	    /* Conflict uses up food too */
		/* and a lot of it because conflict is so overpowered --Amy */
	    if (HConflict || (EConflict & (~W_ARTI))) { u.uhunger--; u.uhunger--; u.uhunger--; u.uhunger--; u.uhunger--; }
	    if (uwep && uwep->oartifact == ART_TENSA_ZANGETSU) u.uhunger -= 10;
	    if (uwep && uwep->oartifact == ART_GARNET_ROD) u.uhunger -= 3;
	    /* +0 charged rings don't do anything, so don't affect hunger */
	    /* Slow digestion still uses ring hunger */
	    switch ((int)(moves % 20)) {	/* note: use even cases only */
	     case  4: if (uleft &&
			  (uleft->spe || !objects[uleft->otyp].oc_charged))
			    u.uhunger--;
		    break;
	     case  8: if (uamul) u.uhunger--;
		    break;
	     case 12: if (uright &&
			  (uright->spe || !objects[uright->otyp].oc_charged))
			    u.uhunger--;
		    break;
	     case 16: if (u.uhave.amulet) u.uhunger--;
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
		    pline(Hallucination ? "You feel like taking the All-You-Can-Eat challenge." : "You're having a hard time getting all of it down.");
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
	if(is_fainted()) nomul(0, 0);
}

#if 0
void
sync_hunger()
{
	if(is_fainted()) {
		flags.soundok = 0;
		nomul(-10+(u.uhunger/10), "fainted from lack of food");
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
				You(Hallucination ? "pass out due to those damn munchies." : Role_if(PM_TOPMODEL) ? "enter a state of trance." : "faint from lack of food.");

	/* warn player if starvation will happen soon, that is, less than 200 nutrition remaining --Amy */
			if(u.uhunger < -(int)(800 + 50*ACURR(A_CON))) { You(Hallucination ? "sense the Grim Reaper approaching." : Role_if(PM_TOPMODEL) ? "sense that you're getting closer to your deity." : "are close to starvation.");
		if (Role_if(PM_TOPMODEL)) adjalign(5);
		}
				flags.soundok = 0;
				nomul(-3+(u.uhunger/200), "fainted from lack of food");
				nomovemsg = "You regain consciousness.";
				afternmv = unfaint;
				newhs = FAINTED;
				if (Role_if(PM_TOPMODEL)) adjalign(1);
			}
		} else
		if(u.uhunger < -(int)(1000 + 50*ACURR(A_CON))) {
			u.uhs = STARVED;
			flags.botl = 1;
			bot();
			You(Hallucination ? "are taken away by the grim reaper..." : Role_if(PM_TOPMODEL) ? "meet your deity at last." : "die from starvation.");
			killer_format = KILLED_BY;
			killer = "starvation";
			done(STARVING);
			/* if we return, we lifesaved, and that calls newuhs */
			return;
		}
	}

	if(newhs != u.uhs) {
		if(newhs >= WEAK && u.uhs < WEAK) {
			losestr(1);	/* this may kill you -- see below */
			if (Fixed_abil || Race_if(PM_SUSTAINER) || (uarms && uarms->oartifact == ART_SYSTEMATIC_CHAOS) || (uamul && uamul->oartifact == ART_FIX_EVERYTHING) ) u.weakcheat++; /* cheater! */
			}
		else if(newhs < WEAK && u.uhs >= WEAK) {
			if (!u.weakcheat) losestr(-1); /* otherwise this could be exploited until you have 25 str --Amy */
			else u.weakcheat--;
			}
		switch(newhs){
		case HUNGRY:
			if (Hallucination) {
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
			if (Hallucination)
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
			You(Hallucination ? "pass away like a filthy bum." : Role_if(PM_TOPMODEL) ? "are embraced by the shadowy figure of your deity..." : "die from hunger and exhaustion.");
			killer_format = KILLED_BY;
			killer = "exhaustion";
			done(STARVING);
			return;
		}
	}
}

#endif /* OVL0 */
#ifdef OVLB

boolean can_reach_floorobj()
{
    return can_reach_floor() &&
	  !((is_pool(u.ux, u.uy) || is_lava(u.ux, u.uy)) &&
	    (Wwalking || is_clinger(youmonst.data) || (Flying && !Breathless)));
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

#ifdef STEED
	if (u.usteed)	/* can't eat off floor while riding */
	    edibles++;
	else
#endif
	if (metallivorous(youmonst.data)) {
	    struct trap *ttmp = t_at(u.ux, u.uy);

	    if (ttmp && ttmp->tseen && ttmp->ttyp == BEAR_TRAP) {
		/* If not already stuck in the trap, perhaps there should
		   be a chance to becoming trapped?  Probably not, because
		   then the trap would just get eaten on the _next_ turn... */
		Sprintf(qbuf, "There is a bear trap here (%s); eat it?",
			(u.utrap && u.utraptype == TT_BEARTRAP) ?
				"holding you" : "armed");
		if ((c = yn_function(qbuf, ynqchars, 'n')) == 'y') {
		    u.utrap = u.utraptype = 0;
		    deltrap(ttmp);
		    return mksobj(BEARTRAP, TRUE, FALSE);
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
	make_sick(0L, (char *) 0, TRUE, SICK_VOMITABLE);
	nomul(-2, "vomiting");
	nomovemsg = 0;
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

