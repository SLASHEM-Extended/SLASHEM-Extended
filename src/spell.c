/*	SCCS Id: @(#)spell.c	3.4	2003/01/17	*/
/*	Copyright (c) M. Stephenson 1988			  */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "edog.h"

/* Are now ints */
static NEARDATA int delay;            /* moves left for this spell */
static NEARDATA int end_delay;        /* when to stop studying */
static NEARDATA struct obj *book;	/* last/current book being xscribed */

/* spellmenu arguments; 0 thru n-1 used as spl_book[] index when swapping */
#define SPELLMENU_CAST (-2)
#define SPELLMENU_VIEW (-1)

#define KEEN 		10000	/* memory increase reading the book */
#define DUNADAN_KEEN 	5000	/* memory increase reading the book */
#define CAST_BOOST 	  500	/* memory increase for successful casting */
#define DUNADAN_CAST_BOOST 	  250	/* memory increase for successful casting */
#define MAX_KNOW 	70000	/* Absolute Max timeout */
#define MAX_CAN_STUDY 	60000	/* Can study while timeout is less than */

#define MAX_STUDY_TIME 	  300	/* Max time for one study session */
#define MAX_SPELL_STUDY    30	/* Uses before spellbook crumbles */

#define spellknow(spell)	spl_book[spell].sp_know 

static NEARDATA const char revivables[] = { ALLOW_FLOOROBJ, FOOD_CLASS, 0 };

static const char all_count[] = { ALLOW_COUNT, ALL_CLASSES, 0 };

#define incrnknow(spell)        spl_book[spell].sp_know = ((spl_book[spell].sp_know < 1) ? (Race_if(PM_DUNADAN) ? DUNADAN_KEEN : KEEN) \
				 : ((spl_book[spell].sp_know + (Race_if(PM_DUNADAN) ? DUNADAN_KEEN : KEEN) ) > MAX_KNOW) ? MAX_KNOW \
				 : spl_book[spell].sp_know + (Race_if(PM_DUNADAN) ? DUNADAN_KEEN : KEEN) )
#define boostknow(spell,boost)  spl_book[spell].sp_know = ((spl_book[spell].sp_know + boost > MAX_KNOW) ? MAX_KNOW \
				 : spl_book[spell].sp_know + boost)

#define spellev(spell)		spl_book[spell].sp_lev
#define spellid(spell)          spl_book[spell].sp_id
#define spellname(spell)	OBJ_NAME(objects[spellid(spell)])
#define spellet(spell)	\
	((char)((spell < 26) ? ('a' + spell) : \
	        (spell < 52) ? ('A' + spell - 26) : \
		(spell < 78) ? ('0' + spell - 52) : 0 ))

STATIC_DCL int spell_let_to_idx(CHAR_P);
STATIC_DCL boolean cursed_book(struct obj *bp);
STATIC_DCL boolean confused_book(struct obj *);
STATIC_DCL void deadbook(struct obj *);
STATIC_PTR int learn(void);
STATIC_DCL void do_reset_learn(void);
STATIC_DCL boolean getspell(int *);
STATIC_DCL boolean dospellmenu(const char *,int,int *);
STATIC_DCL int percent_success(int);
STATIC_DCL void cast_protection(void);
STATIC_DCL void cast_reflection(void);
STATIC_DCL void spell_backfire(int);
STATIC_DCL const char *spelltypemnemonic(int);
STATIC_DCL int isqrt(int);
static int spell_dash(void);

/* categories whose names don't come from OBJ_NAME(objects[type]) */
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
#define PN_OCCULT_SPELL		(-13)
#define PN_ELEMENTAL_SPELL		(-14)
#define PN_CHAOS_SPELL		(-15)
#define PN_MATTER_SPELL		(-16)
#define PN_BARE_HANDED		(-17)
#define PN_HIGH_HEELS		(-18)
#define PN_GENERAL_COMBAT		(-19)
#define PN_SHIELD		(-20)
#define PN_BODY_ARMOR		(-21)
#define PN_TWO_HANDED_WEAPON		(-22)
#define PN_POLYMORPHING		(-23)
#define PN_DEVICES		(-24)
#define PN_SEARCHING		(-25)
#define PN_SPIRITUALITY		(-26)
#define PN_PETKEEPING		(-27)
#define PN_MISSILE_WEAPONS		(-28)
#define PN_TECHNIQUES		(-29)
#define PN_IMPLANTS		(-30)
#define PN_MARTIAL_ARTS		(-31)
#define PN_RIDING		(-32)
#define PN_TWO_WEAPONS		(-33)
#define PN_LIGHTSABER		(-34)

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
	PN_LIGHTSABER,
	PN_ATTACK_SPELL,     PN_HEALING_SPELL,
	PN_DIVINATION_SPELL, PN_ENCHANTMENT_SPELL,
	PN_PROTECTION_SPELL,            PN_BODY_SPELL,
	PN_OCCULT_SPELL,
	PN_ELEMENTAL_SPELL,
	PN_CHAOS_SPELL,
	PN_MATTER_SPELL,
	PN_BARE_HANDED,	PN_HIGH_HEELS,
	PN_GENERAL_COMBAT,	PN_SHIELD,	PN_BODY_ARMOR,
	PN_TWO_HANDED_WEAPON,	PN_POLYMORPHING,	PN_DEVICES,
	PN_SEARCHING,	PN_SPIRITUALITY,	PN_PETKEEPING,
	PN_MISSILE_WEAPONS,	PN_TECHNIQUES,	PN_IMPLANTS,	PN_MARTIAL_ARTS, 
	PN_TWO_WEAPONS,
	PN_RIDING,
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
    "occult spells",
    "elemental spells",
    "chaos spells",
    "matter spells",
    "bare-handed combat",
    "high heels",
    "general combat",
    "shield",
    "body armor",
    "two-handed weapons",
    "polymorphing",
    "devices",
    "searching",
    "spirituality",
    "petkeeping",
    "missile weapons",
    "techniques",
    "implants",
    "martial arts",
    "riding",
    "two-weapon combat",
    "lightsaber"
};

#endif	/* OVLB */

#define P_NAME(type) (skill_names_indices[type] > 0 ? \
		      OBJ_NAME(objects[skill_names_indices[type]]) : \
			odd_skill_names[-skill_names_indices[type]])

boolean
spell_known(int sbook_id)
{
	int i;

	i = 0;
	while (spl_book[i].sp_id != NO_SPELL && i < MAXSPELL) {
		if (spl_book[i].sp_id == sbook_id) return(TRUE);
		i++;
	}
	return(FALSE);
}



/* The roles[] table lists the role-specific values for tuning
 * percent_success().
 *
 * Reasoning:
 *   splcaster, special:
 *	Arc are aware of magic through historical research
 *	Bar abhor magic (Conan finds it "interferes with his animal instincts")
 *	Cav are ignorant to magic
 *	Hea are very aware of healing magic through medical research
 *	Kni are moderately aware of healing from Paladin training
 *	Mon use magic to attack and defend in lieu of weapons and armor
 *	Pri are very aware of healing magic through theological research
 *	Ran avoid magic, preferring to fight unseen and unheard
 *	Rog are moderately aware of magic through trickery
 *	Sam have limited magical awareness, prefering meditation to conjuring
 *	Tou are aware of magic from all the great films they have seen
 *	Val have limited magical awareness, prefering fighting
 *	Wiz are trained mages
 *
 *	The arms penalty is lessened for trained fighters Bar, Kni, Ran,
 *	Sam, Val -
 *	the penalty is its metal interference, not encumbrance.
 *	The `spelspec' is a single spell which is fundamentally easier
 *	 for that role to cast.
 *
 *  spelspec, spelsbon:
 *	Arc map masters (SPE_MAGIC_MAPPING)
 *	Bar fugue/berserker (SPE_HASTE_SELF)
 *	Cav born to dig (SPE_DIG)
 *	Hea to heal (SPE_CURE_SICKNESS)
 *	Kni to turn back evil (SPE_TURN_UNDEAD)
 *	Mon to preserve their abilities (SPE_RESTORE_ABILITY)
 *	Pri to bless (SPE_REMOVE_CURSE)
 *	Ran to hide (SPE_INVISIBILITY)
 *	Rog to find loot (SPE_DETECT_TREASURE)
 *	Sam to be At One (SPE_CLAIRVOYANCE)
 *	Tou to smile (SPE_CHARM_MONSTER)
 *	Val control lightning (SPE_LIGHTNING)
 *	Wiz all really, but SPE_MAGIC_MISSILE is their party trick
 *	Yeo guard doors (SPE_KNOCK)
 *
 *	See percent_success() below for more comments.
 *
 *  uarmbon, uarmsbon, uarmhbon, uarmgbon, uarmfbon:
 *	Fighters find body armour & shield a little less limiting.
 *	Headgear, Gauntlets and Footwear are not role-specific (but
 *	still have an effect, except helm of brilliance, which is designed
 *	to permit magic-use).
 */

#define uarmhbon 4 /* Metal helmets interfere with the mind */
#define uarmgbon 6 /* Casting channels through the hands */
#define uarmfbon 2 /* All metal interferes to some degree */

/* since the spellbook itself doesn't blow up, don't say just "explodes" */
static const char explodes[] = "radiates explosive energy";

/* convert an alnum into a number in the range 0..61, or -1 if not an alnum */
STATIC_OVL int
spell_let_to_idx(ilet)
char ilet;
{
    int indx;

    indx = ilet - 'a';
    if (indx >= 0 && indx < 26) return indx;
    indx = ilet - 'A';
    if (indx >= 0 && indx < 26) return indx + 26;
    indx = ilet - '0';
    if (indx >= 0 && indx < 10) return indx + 52;
    return -1;
}

STATIC_PTR void
undo_barfloodC(x, y, roomcnt)
int x, y;
void * roomcnt;
{
	if (levl[x][y].typ != IRONBARS)
		return;

	(*(int *)roomcnt)++;

	/* Get rid of bars at x, y */
	levl[x][y].typ = ROOM;
	unblock_point(x,y);
	newsym(x,y);
}

/* TRUE: book should be destroyed by caller */
STATIC_OVL boolean
cursed_book(bp)
	struct obj *bp;
{
	int lev = objects[bp->otyp].oc_level;

	switch(rn2(lev)) {
	case 0:
		You_feel("a wrenching sensation.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Tam net nikakoy zashchity. Tam net nikakoy nadezhdy. Yedinstvennoye, chto yest'? Uverennost' v tom, chto vy, igrok, budet umeret' uzhasnoy i muchitel'noy smert'yu." : "SCHRING!");
		tele();		/* teleport him */
		break;
	case 1:
		You_feel("threatened.");
		aggravate();
		break;
	case 2:
		/* [Tom] lowered this (used to be 100,250) */
		make_blinded(Blinded + rn1(50,25),TRUE);
		break;
	case 3:
		take_gold();
		break;
	case 4:
		pline("These runes were just too much to comprehend.");
		make_confused(HConfusion + rn1(7,16),FALSE);
		break;
	case 5:
		pline_The("book was coated with contact poison!");
		if (uarmg) {
		    if (uarmg->oerodeproof || !is_corrodeable(uarmg)) {
			Your("gloves seem unaffected.");
		    } else if (uarmg->oeroded2 < MAX_ERODE) {
			if (uarmg->greased) {
			    grease_protect(uarmg, "gloves", &youmonst);
			} else if (!rn2(2) || !(uarmf && uarmf->oartifact == ART_LUISA_S_IRRESISTIBLE_CHARM) ) {
			    Your("gloves corrode%s!",
				 uarmg->oeroded2+1 == MAX_ERODE ?
				 " completely" : uarmg->oeroded2 ?
				 " further" : "");
			    uarmg->oeroded2++;
			}
		    } else
			Your("gloves %s completely corroded.",
			     Blind ? "feel" : "look");
		    break;
		}
		/* temp disable in_use; death should not destroy the book */
		bp->in_use = FALSE;
		losestr(Poison_resistance ? rn1(2,1) : rn1(4,3));
		losehp(rnd(Poison_resistance ? 6 : 10),
		       "contact-poisoned spellbook", KILLED_BY_AN);
		bp->in_use = TRUE;
		break;
	case 6:
		if(Antimagic) {
		    shieldeff(u.ux, u.uy);
		    pline_The("book %s, but you are unharmed!", explodes);
		} else {
		    pline("As you read the book, it %s in your %s!",
			  explodes, body_part(FACE));
		    losehp(2*rnd(10)+5, "exploding rune", KILLED_BY_AN);
		}
		return TRUE;
	default:
		rndcurse();
		break;
	}
	return FALSE;
}

/* study while confused: returns TRUE if the book is destroyed */
STATIC_OVL boolean
confused_book(spellbook)
struct obj *spellbook;
{
	boolean gone = FALSE;

	if (!rn2(5) && spellbook->otyp != SPE_BOOK_OF_THE_DEAD) {
	    spellbook->in_use = TRUE;	/* in case called from learn */
	    pline(
	"Being confused you have difficulties in controlling your actions.");
	    display_nhwindow(WIN_MESSAGE, FALSE);
	    You("accidentally tear the spellbook to pieces.");
	    if (!objects[spellbook->otyp].oc_name_known &&
		!objects[spellbook->otyp].oc_uname)
		docall(spellbook);
	    if (carried(spellbook)) useup(spellbook);
	    else useupf(spellbook, 1L);
	    gone = TRUE;
	} else {
	    You("find yourself reading the %s line over and over again.",
		spellbook == book ? "next" : "first");
	}
	return gone;
}

STATIC_PTR void
do_lockfloodg(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}
	if (rn2(5)) randomamount = 0;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && ((levl[randomx][randomy].wall_info & W_NONDIGGABLE) == 0) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR || (levl[randomx][randomy].typ == DOOR && levl[randomx][randomy].doormask == D_NODOOR) ) ) {

			if (rn2(3)) doorlockX(randomx, randomy, TRUE);
			else {
				if (levl[randomx][randomy].typ != DOOR) levl[randomx][randomy].typ = STONE;
				else levl[randomx][randomy].typ = CROSSWALL;
				block_point(randomx,randomy);
				if (!(levl[randomx][randomy].wall_info & W_EASYGROWTH)) levl[randomx][randomy].wall_info |= W_HARDGROWTH;
				del_engr_at(randomx, randomy);

				if ((mtmp = m_at(randomx, randomy)) != 0) {
					(void) minliquid(mtmp);
				} else {
					newsym(randomx,randomy);
				}

			}
		}
	}
	if (rn2(3)) doorlockX(x, y, TRUE);

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].wall_info & W_NONDIGGABLE) != 0 || (levl[x][y].typ != CORR && levl[x][y].typ != ROOM && (levl[x][y].typ != DOOR || levl[x][y].doormask != D_NODOOR) ))
		return;

	if ((ttmp = t_at(x, y)) != 0 && !delfloortrap(ttmp))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a wall at x, y */
		if (levl[x][y].typ != DOOR) levl[x][y].typ = STONE;
		else levl[x][y].typ = CROSSWALL;
		block_point(x,y);
		if (!(levl[x][y].wall_info & W_EASYGROWTH)) levl[x][y].wall_info |= W_HARDGROWTH;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_treefloodg(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}
	if (rn2(5)) randomamount = 0;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = TREE;
			block_point(randomx,randomy);
			if (!(levl[randomx][randomy].wall_info & W_EASYGROWTH)) levl[randomx][randomy].wall_info |= W_HARDGROWTH;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	if ((ttmp = t_at(x, y)) != 0 && !delfloortrap(ttmp))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = TREE;
		block_point(x,y);
		if (!(levl[x][y].wall_info & W_EASYGROWTH)) levl[x][y].wall_info |= W_HARDGROWTH;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_icefloodg(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}
	if (rn2(5)) randomamount = 0;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = ICE;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	if ((ttmp = t_at(x, y)) != 0 && !delfloortrap(ttmp))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = ICE;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_cloudfloodg(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}
	if (rn2(5)) randomamount = 0;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = CLOUD;
			block_point(randomx,randomy);
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	if ((ttmp = t_at(x, y)) != 0 && !delfloortrap(ttmp))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = CLOUD;
		block_point(x,y);
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_terrainfloodg(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}
	if (rn2(7)) randomamount = 0;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = randomwalltype();
			block_point(randomx,randomy);
			if (!(levl[randomx][randomy].wall_info & W_EASYGROWTH)) levl[randomx][randomy].wall_info |= W_HARDGROWTH;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	if ((ttmp = t_at(x, y)) != 0 && !delfloortrap(ttmp))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = randomwalltype();
		block_point(x,y);
		if (!(levl[x][y].wall_info & W_EASYGROWTH)) levl[x][y].wall_info |= W_HARDGROWTH;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_barfloodg(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;
	int randomamount = 0;
	int randomx, randomy;
	if (!rn2(25)) randomamount += rnz(2);
	if (!rn2(125)) randomamount += rnz(5);
	if (!rn2(625)) randomamount += rnz(20);
	if (!rn2(3125)) randomamount += rnz(50);
	if (isaquarian) {
		if (!rn2(25)) randomamount += rnz(2);
		if (!rn2(125)) randomamount += rnz(5);
		if (!rn2(625)) randomamount += rnz(20);
		if (!rn2(3125)) randomamount += rnz(50);
	}
	if (rn2(5)) randomamount = 0;

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {
			levl[randomx][randomy].typ = IRONBARS;
			block_point(randomx,randomy);
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y) )
		return;

	if ((ttmp = t_at(x, y)) != 0 && !delfloortrap(ttmp))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = IRONBARS;
		block_point(x,y);
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_lavafloodg(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;

	if (/*nexttodoor(x, y) || */(rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	if ((ttmp = t_at(x, y)) != 0 && !delfloortrap(ttmp))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = LAVAPOOL;
		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

STATIC_PTR void
do_floodg(x, y, poolcnt)
int x, y;
void * poolcnt;
{
	register struct monst *mtmp;
	register struct trap *ttmp;

	if (/*nexttodoor(x, y) || */(rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	if ((ttmp = t_at(x, y)) != 0 && !delfloortrap(ttmp))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = POOL;
		del_engr_at(x, y);
		water_damage(level.objects[x][y], FALSE, TRUE);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

}

/* special effects for The Book of the Dead */
STATIC_OVL void
deadbook(book2)
struct obj *book2;
{
    struct monst *mtmp, *mtmp2;
    coord mm;

    You("turn the pages of the Book of the Dead...");
    makeknown(SPE_BOOK_OF_THE_DEAD);

    if (!u.bellimbued) {
	pline("But nothing happens. You faintly recall that this book will be an enigma to you until you finally imbue a certain 'silver bell'.");
	return;
    }

    /* KMH -- Need ->known to avoid "_a_ Book of the Dead" */
    book2->known = 1;
    if(invocation_pos(u.ux, u.uy) && !On_stairs(u.ux, u.uy)) {
	register struct obj *otmp;
	register boolean arti1_primed = FALSE, arti2_primed = FALSE,
			 arti_cursed = FALSE;

	/* If your book becomes prime cursed or worse, the game should still be winnable --Amy */
	if(book2->cursed && rn2(2)) {
	    pline_The("runes appear scrambled.  You can't read them!");
	    return;
	}

	if(!u.uhave.bell || !u.uhave.menorah) {
	    pline("A chill runs down your %s.", body_part(SPINE));
	    if(!u.uhave.bell) You_hear("a faint chime...");
	    if(!u.uhave.menorah) pline("Vlad's doppelganger is amused.");
	    return;
	}

	for(otmp = invent; otmp; otmp = otmp->nobj) {
	    if(otmp->otyp == CANDELABRUM_OF_INVOCATION &&
	       otmp->spe == 7 && otmp->lamplit) {
		if(!otmp->cursed || !issoviet) arti1_primed = TRUE;
		else arti_cursed = TRUE;
	    }
	    if(otmp->otyp == BELL_OF_OPENING &&
	       (moves - otmp->age) < 50L) { /* you rang it recently */
		if(!otmp->cursed || !issoviet) arti2_primed = TRUE;
		else arti_cursed = TRUE;
	    }
	}

	if(arti_cursed && issoviet) { /* Nyah-nyah! --Amy */
	    pline_The("invocation fails!");
	    pline("At least one of your artifacts is cursed...");
	} else if(arti1_primed && arti2_primed) {
	    unsigned soon = (unsigned) d(2,6);	/* time til next intervene() */

	    /* successful invocation */
	    mkinvokearea();
	    u.uevent.invoked = 1;
	    /* in case you haven't killed the Wizard yet, behave as if
	       you just did */
	    u.uevent.udemigod = 1;	/* wizdead() */
	    if (!u.udg_cnt || u.udg_cnt > soon) u.udg_cnt = soon;
	} else {	/* at least one artifact not prepared properly */
	    You("have a feeling that %s is amiss...", something);
	    goto raise_dead;
	}
	return;
    }

    /* when not an invocation situation */
    if (book2->cursed) {
raise_dead:

	You("raised the dead!");
	/* first maybe place a dangerous adversary */
	if (!rn2(3) && ((mtmp = makemon(&mons[PM_MASTER_LICH],
					u.ux, u.uy, NO_MINVENT)) != 0 ||
			(mtmp = makemon(&mons[PM_NALFESHNEE],
					u.ux, u.uy, NO_MINVENT)) != 0)) {
	    mtmp->mpeaceful = 0;
	    set_malign(mtmp);
	}
	/* next handle the affect on things you're carrying */
	(void) unturn_dead(&youmonst);
	/* last place some monsters around you */
	mm.x = u.ux;
	mm.y = u.uy;
	mkundead(&mm, TRUE, NO_MINVENT);
	badeffect();
	aggravate();
    } else if(book2->blessed) {
	for(mtmp = fmon; mtmp; mtmp = mtmp2) {
	    mtmp2 = mtmp->nmon;		/* tamedog() changes chain */
	    if (DEADMONSTER(mtmp)) continue;

	    if ((is_undead(mtmp->data) || mtmp->egotype_undead) && cansee(mtmp->mx, mtmp->my)) {
		mtmp->mpeaceful = TRUE;
		if(sgn(mtmp->data->maligntyp) == sgn(u.ualign.type)
		   && distu(mtmp->mx, mtmp->my) < 4)
		    if (mtmp->mtame) {
			if (mtmp->mtame < 20)
			    mtmp->mtame++;
		    } else
			(void) tamedog(mtmp, (struct obj *)0, FALSE);
		else monflee(mtmp, rnd(10), FALSE, TRUE);
	    }
	}
	/* Amy edit: infinite command undead would be way overpowered */
	unbless(book2);
    } else {
	switch(rn2(3)) {
	case 0:
	    Your("ancestors are annoyed with you!");
	    break;
	case 1:
	    pline_The("headstones in the cemetery begin to move!");
	    break;
	default:
	    pline("Oh my!  Your name appears in the book!");
	}
    }
    return;
}

STATIC_PTR int
learn()
{
	int i;
	short booktype;
	char splname[BUFSZ];
	boolean costly = TRUE;

	if (!book || !(carried(book) || 
		(book->where == OBJ_FLOOR && 
			book->ox == u.ux && book->oy == u.uy))) {
	    /* maybe it was stolen or polymorphed? */
	    do_reset_learn();
	    return(0);
	}
	/* JDS: lenses give 50% faster reading; 33% smaller read time */
	if (delay < end_delay && ublindf && ublindf->otyp == LENSES && rn2(2))
	    delay++;
	if (delay < end_delay && ublindf && ublindf->otyp == RADIOGLASSES && rn2(2))
	    delay++;
	if (delay < end_delay && ublindf && ublindf->otyp == BOSS_VISOR && rn2(2))
	    delay++;

	if (Confusion && (book->otyp != SPE_BOOK_OF_THE_DEAD) && !Conf_resist && !rn2(Role_if(PM_LIBRARIAN) ? 100 : 10) ) {		/* became confused while learning */

	    (void) confused_book(book);
	    book = 0;			/* no longer studying */
	    if ((delay - end_delay) < 0) {
			if (!issoviet) nomul((-(rno(-(delay - end_delay)))), "reading a confusing book", TRUE); /* remaining delay is uninterrupted */
			else {
				nomul((delay - end_delay), "reading a confusing book", TRUE);
				pline("Vy tol'ko chto podpisal svoy smertnyy prigovor, potomu chto sovetskaya ne zabotitsya o igrovoy balans. Ne dazhe nebol'shoye nemnogo.");

			}
	    }
	    delay = end_delay;
	    return(0);
	}
	if (delay < end_delay) {    /* not if (delay++), so at end delay == 0 */
	    delay++;
	    if (delay) return(1); /* still busy, Amy edit: to me it looks like the original code is not working correctly,
					   * since "delay" can become 0 here and if you get interrupted, you lose a charge again! */
	}
	exercise(A_WIS, TRUE);		/* you're studying. */
	booktype = book->otyp;
	if(booktype == SPE_BOOK_OF_THE_DEAD) {
	    deadbook(book);
	    return(0);
	}

	sprintf(splname, objects[booktype].oc_name_known ?
			"\"%s\"" : "the \"%s\" spell",
		OBJ_NAME(objects[booktype]) );
	for (i = 0; i < MAXSPELL; i++)  {
		if (spellid(i) == booktype)  {
			if (book->spestudied > MAX_SPELL_STUDY) {
			    pline("This spellbook is too faint to be read anymore.");
			    book->otyp = booktype = SPE_BLANK_PAPER;
			} else if (spellknow(i) <= MAX_CAN_STUDY) {
			    Your("knowledge of that spell is keener.");
			    incrnknow(i);
				if (uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "runic gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "runa rukovitsakh") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "runi qo'lqop") ) && !rn2(2) ) incrnknow(i);
				if (Role_if(PM_MAHOU_SHOUJO)) incrnknow(i);
			    book->spestudied++;
			    if (end_delay) {
			    	boostknow(i,
				  end_delay * (book->spe > 0 ? 20 : 10));
				use_skill(spell_skilltype(book->otyp),
				  end_delay / (book->spe > 0 ? 10 : 20));
			    }
			    exercise(A_WIS, TRUE);      /* extra study */
			} else { /* MAX_CAN_STUDY < spellknow(i) <= MAX_SPELL_STUDY */
			    You("know %s quite well already.", splname);
			    costly = FALSE;
			}
			/* make book become known even when spell is already
			   known, in case amnesia made you forget the book */
			makeknown((int)booktype);
			break;
		} else if (spellid(i) == NO_SPELL)  {
			spl_book[i].sp_id = booktype;
			spl_book[i].sp_lev = objects[booktype].oc_level;
			incrnknow(i);
			if (uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "runic gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "runa rukovitsakh") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "runi qo'lqop") ) && !rn2(2) ) incrnknow(i);
			if (Role_if(PM_MAHOU_SHOUJO)) incrnknow(i);
			book->spestudied++;
			You("have keen knowledge of the spell.");
			You(i > 0 ? "add %s to your repertoire." : "learn %s.",
			    splname);
			if (booktype == SPE_FORBIDDEN_KNOWLEDGE) {
				u.ugangr += 15;
				if (flags.soundok) You_hear("a thunderous growling and rumbling...");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Pozdravlyayu, teper' vashe bozhestvo ochen' zol. Pochemu ty ne molish'sya? Eto mozhet byt' prosto privesti svoyu zhizn' do kontsa, a zatem vy mozhete svernut' luchshe, nesovetskimi kharakter nakonets-to! SDELAY ETO SEYCHAS!" : "Grrrrrrr! Grummel! Wummmmmm!");
			}

			makeknown((int)booktype);
			if (booktype == SPE_AMNESIA) {
				You_feel("dizzy!");
				forget(ALL_MAP);
			}
			break;
		}
	}
	if (i == MAXSPELL) impossible("Too many spells memorized!");

	if ( (book->cursed || book->spe < 1) && !Role_if(PM_LIBRARIAN) && !(booktype == SPE_BOOK_OF_THE_DEAD) ) {	/* maybe a demon cursed it */
	    if (cursed_book(book)) {
		if (carried(book)) useup(book);
		else useupf(book, 1L);
		book = 0;
		return 0;
	    }
	}
	if (costly) check_unpaid(book);
	book = 0;
	return(0);
}

int
study_book(spellbook)
register struct obj *spellbook;
{
	register int	 booktype = spellbook->otyp;
	register boolean confused = ((Confusion != 0) && !Conf_resist);
	boolean too_hard = FALSE;

	if (delay && !confused && spellbook == book &&
		    /* handle the sequence: start reading, get interrupted,
		       have book become erased somehow, resume reading it */
		    booktype != SPE_BLANK_PAPER) {
		You("continue your efforts to memorize the spell.");
		if (AutoDestruct || u.uprops[AUTO_DESTRUCT].extrinsic || (uarmf && uarmf->oartifact == ART_KHOR_S_REQUIRED_IDEA) || have_autodestructstone()) learn();
	} else {
		/* KMH -- Simplified this code */
		if (booktype == SPE_BLANK_PAPER) {
			pline("This spellbook is all blank.");
			makeknown(booktype);
			return(1);
		}
		if (spellbook->spe && confused && rn2(Role_if(PM_LIBRARIAN) ? 2 : 10) ) {
		    check_unpaid_usage(spellbook, TRUE);

			int nochargechange = 10;
			if (!(PlayerCannotUseSkills)) {
				switch (P_SKILL(P_DEVICES)) {
					default: break;
					case P_BASIC: nochargechange = 9; break;
					case P_SKILLED: nochargechange = 8; break;
					case P_EXPERT: nochargechange = 7; break;
					case P_MASTER: nochargechange = 6; break;
					case P_GRAND_MASTER: nochargechange = 5; break;
					case P_SUPREME_MASTER: nochargechange = 4; break;
				}
			}

		    if (nochargechange >= rnd(10)) consume_obj_charge(spellbook, FALSE);

			use_skill(P_DEVICES,1);
			if (Race_if(PM_FAWN)) {
				use_skill(P_DEVICES,1);
			}
			if (Race_if(PM_SATRE)) {
				use_skill(P_DEVICES,1);
				use_skill(P_DEVICES,1);
			}

		    pline_The("words on the page seem to glow faintly purple.");
		    You_cant("quite make them out.");
		    return 1;
		}

		switch (objects[booktype].oc_level) {
		 case 1:
		 case 2:
			delay = -objects[booktype].oc_delay;
			break;
		 case 3:
		 case 4:
			delay = -(objects[booktype].oc_level - 1) *
				objects[booktype].oc_delay;
			break;
		 case 5:
		 case 6:
			delay = -objects[booktype].oc_level *
				objects[booktype].oc_delay;
			break;
		 case 7:
			delay = -8 * objects[booktype].oc_delay;
			break;
		 case 8:
			delay = -12 * objects[booktype].oc_delay;
			break;
		 default:
			impossible("Unknown spellbook level %d, book %d;",
				objects[booktype].oc_level, booktype);
			return 0;
		}

		/* Books are often wiser than their readers (Rus.) */
		spellbook->in_use = TRUE;
		if (!spellbook->blessed &&
		    spellbook->otyp != SPE_BOOK_OF_THE_DEAD) {
		    if ( ((spellbook->cursed && rn2(4)) || (spellbook->spe < 1 && rn2(3)) ) && !Role_if(PM_LIBRARIAN) && booktype != SPE_BOOK_OF_THE_DEAD ) {
			too_hard = TRUE;
		    } else {
			/* uncursed - chance to fail */
			int read_ability = ACURR(A_INT) + 4 + u.ulevel/2
			    - 2*objects[booktype].oc_level
			    + ((ublindf && (ublindf->otyp == LENSES || ublindf->otyp == RADIOGLASSES || ublindf->otyp == BOSS_VISOR)) ? 2 : 0);
			/* only wizards know if a spell is too difficult */
			/* Amy edit: others may randomly know it sometimes */
			if ((Role_if(PM_WIZARD) || !rn2(4)) && read_ability < 20 &&
			    !confused && (!spellbook->spe || spellbook->cursed)) {
			    char qbuf[QBUFSZ];
			    sprintf(qbuf,
		      "This spellbook is %sdifficult to comprehend. Continue?",
				    (read_ability < 5 ? "extremely" : read_ability < 12 ? "very " : ""));
			    if (yn(qbuf) != 'y') {
				spellbook->in_use = FALSE;
				return(1);
			    }
			}
			/* its up to random luck now */
			if (rnd(20) > read_ability) {
			    too_hard = TRUE;
			}
		    }
		}

		if ( (too_hard || rn2(2)) && ( (spellbook->cursed && !Role_if(PM_LIBRARIAN) ) || (!(spellbook->spe) && !(booktype == SPE_BOOK_OF_THE_DEAD) ) )) {
		    boolean gone = cursed_book(spellbook);

		    if (delay < 0) {
				if (!issoviet) nomul(-(rno(-(delay))), "reading a cursed book", TRUE); /* study time */
				else {
					nomul((delay), "reading a cursed book", TRUE);
					pline("Vy tol'ko chto podpisal svoy smertnyy prigovor, potomu chto sovetskaya ne zabotitsya o igrovoy balans. Ne dazhe nebol'shoye nemnogo.");
				}

		    }
		    delay = 0;
		    if(gone || !rn2(3)) {
			if (!gone) pline_The("spellbook crumbles to dust!");
			if (!objects[spellbook->otyp].oc_name_known &&
				!objects[spellbook->otyp].oc_uname)
			    docall(spellbook);
				if (carried(spellbook)) useup(spellbook);
				else useupf(spellbook, 1L);
		    } else
			spellbook->in_use = FALSE;
		    return(1);
		} else if (confused && !Conf_resist && !rn2(Role_if(PM_LIBRARIAN) ? 50 : 5) && spellbook->otyp != SPE_BOOK_OF_THE_DEAD) {
		    if (!confused_book(spellbook)) {
			spellbook->in_use = FALSE;
		    }
		    if (delay < 0) {
				if (!issoviet) nomul(-(rno(-(delay))), "reading a book while confused", TRUE);
				else {
					nomul((delay), "reading a book while confused", TRUE);
					pline("Vy tol'ko chto podpisal svoy smertnyy prigovor, potomu chto sovetskaya ne zabotitsya o igrovoy balans. Ne dazhe nebol'shoye nemnogo.");
				}
		    }
		    delay = 0;
		    return(1);
		}
		spellbook->in_use = FALSE;

		/* The glowing words make studying easier */
		if (spellbook->otyp != SPE_BOOK_OF_THE_DEAD) {
		    delay *= 2;
		    if (spellbook->spe) {
			check_unpaid_usage(spellbook, TRUE);

			int nochargechange = 10;
			if (!(PlayerCannotUseSkills)) {
				switch (P_SKILL(P_DEVICES)) {
					default: break;
					case P_BASIC: nochargechange = 9; break;
					case P_SKILLED: nochargechange = 8; break;
					case P_EXPERT: nochargechange = 7; break;
					case P_MASTER: nochargechange = 6; break;
					case P_GRAND_MASTER: nochargechange = 5; break;
					case P_SUPREME_MASTER: nochargechange = 4; break;
				}
			}

			if (nochargechange >= rnd(10)) consume_obj_charge(spellbook, FALSE);

			use_skill(P_DEVICES,1);
			if (Race_if(PM_FAWN)) {
				use_skill(P_DEVICES,1);
			}
			if (Race_if(PM_SATRE)) {
				use_skill(P_DEVICES,1);
				use_skill(P_DEVICES,1);
			}
			if (Role_if(PM_LIBRARIAN)) {
				use_skill(P_DEVICES,1);
			}

			pline_The("words on the page seem to glow faintly.");
			if (!too_hard)
			    delay /= 3;
		    }
		}
		end_delay = 0;  /* Changed if multi != 0 */

#ifdef DEBUG
		pline("Delay: %i", delay);
#endif
		if (multi) {
			/* Count == practice reading :) */
	        	char qbuf[QBUFSZ];
	        	
	        	if (multi + 1 > MAX_STUDY_TIME) multi = MAX_STUDY_TIME - 1;
	        	sprintf(qbuf, "Study for at least %i turns?", (multi+1));
			if (ynq(qbuf) != 'y') {
				multi = 0;
				return(1);
			}
			if ((--multi) > (-delay)) end_delay = multi + delay;
			multi = 0;
#ifdef DEBUG
			pline("end_delay: %i", end_delay);
#endif
		}

		You("begin to %s the runes.",
		    spellbook->otyp == SPE_BOOK_OF_THE_DEAD ? "recite" :
		    "memorize");
	}

	book = spellbook;
	set_occupation(learn, "studying", 0);
	return(1);
}

/* a spellbook has been destroyed or the character has changed levels;
   the stored address for the current book is no longer valid */
void
book_disappears(obj)
struct obj *obj;
{
	if (obj == book) book = (struct obj *)0;
}

/* renaming an object usually results in it having a different address;
   so the sequence start reading, get interrupted, name the book, resume
   reading would read the "new" book from scratch */
void
book_substitution(old_obj, new_obj)
struct obj *old_obj, *new_obj;
{
	if (old_obj == book) book = new_obj;
}

static void
do_reset_learn()
{
	stop_occupation();
}

/* called from moveloop() */
void
age_spells()
{
	int i;
	if (Keen_memory && moves % 3 == 0)
		return;
	/*
	 * The time relative to the hero (a pass through move
	 * loop) causes all spell knowledge to be decremented.
	 * The hero's speed, rest status, conscious status etc.
	 * does not alter the loss of memory.
	 */
	for (i = 0; i < MAXSPELL && spellid(i) != NO_SPELL; i++)
	    if (spellknow(i) ) {

		if (!(uarmc && OBJ_DESCR(objects[uarmc->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "guild cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "gil'dii plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "birlik plash") ) ) ) {
			decrnknow(i);
		}

		if (SpellForgetting || u.uprops[SPELL_FORGETTING].extrinsic || have_spellforgettingstone()) {
			if (spellknow(i)) decrnknow(i);
			if (spellknow(i)) decrnknow(i);
			if (spellknow(i)) decrnknow(i);
			if (spellknow(i)) decrnknow(i);
			if (spellknow(i)) decrnknow(i);
			if (spellknow(i)) decrnknow(i);
			if (spellknow(i)) decrnknow(i);
			if (spellknow(i)) decrnknow(i);
			if (spellknow(i)) decrnknow(i);
		}

		/* In Soviet Russia, people do not use magic very often, and so they definitely don't need any of those
		 * completely superfluous messages telling them about spell memory. They already trashed the actual spell
		 * memory display in the menu, so why the heck would they suddenly want to get messages about it? Hell no,
		 * they prefer to suddenly discover their cure sickness spell was forgotten just as they're about to
		 * cure that green slime effect on level 66 of Gehennom, after investing 500 hours into their character. --Amy */

		if (!issoviet && !SpellColorCyan && !(SpellForgetting || u.uprops[SPELL_FORGETTING].extrinsic || have_spellforgettingstone()) && !(SpellLoss || u.uprops[SPELLS_LOST].extrinsic || have_spelllossstone()) && spellknow(i) == 1000) pline("Your %s spell is beginning to fade from your memory.", spellname(i));
		if (!issoviet && !SpellColorCyan && !(SpellForgetting || u.uprops[SPELL_FORGETTING].extrinsic || have_spellforgettingstone()) && !(SpellLoss || u.uprops[SPELLS_LOST].extrinsic || have_spelllossstone()) && spellknow(i) == 100) pline("You are about to forget the %s spell.", spellname(i));
		if (!issoviet && !SpellColorCyan && !(SpellForgetting || u.uprops[SPELL_FORGETTING].extrinsic || have_spellforgettingstone()) && !(SpellLoss || u.uprops[SPELLS_LOST].extrinsic || have_spelllossstone()) && spellknow(i) == 0) pline("You no longer know how to cast the %s spell.", spellname(i));

		if (spellknow(i) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "forgetful cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "zabyvchiv plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "unutuvchan plash") ) ) {
			decrnknow(i);
			if (!issoviet && !SpellColorCyan && !(SpellForgetting || u.uprops[SPELL_FORGETTING].extrinsic || have_spellforgettingstone()) && !(SpellLoss || u.uprops[SPELLS_LOST].extrinsic || have_spelllossstone()) && spellknow(i) == 1000) pline("Your %s spell is beginning to fade from your memory.", spellname(i));
			if (!issoviet && !SpellColorCyan && !(SpellForgetting || u.uprops[SPELL_FORGETTING].extrinsic || have_spellforgettingstone()) && !(SpellLoss || u.uprops[SPELLS_LOST].extrinsic || have_spelllossstone()) && spellknow(i) == 100) pline("You are about to forget the %s spell.", spellname(i));
			if (!issoviet && !SpellColorCyan && !(SpellForgetting || u.uprops[SPELL_FORGETTING].extrinsic || have_spellforgettingstone()) && !(SpellLoss || u.uprops[SPELLS_LOST].extrinsic || have_spelllossstone()) && spellknow(i) == 0) pline("You no longer know how to cast the %s spell.", spellname(i));

		}

		if (spellknow(i) && RngeForgetting) {
			decrnknow(i);
			if (!issoviet && !SpellColorCyan && !(SpellForgetting || u.uprops[SPELL_FORGETTING].extrinsic || have_spellforgettingstone()) && !(SpellLoss || u.uprops[SPELLS_LOST].extrinsic || have_spelllossstone()) && spellknow(i) == 1000) pline("Your %s spell is beginning to fade from your memory.", spellname(i));
			if (!issoviet && !SpellColorCyan && !(SpellForgetting || u.uprops[SPELL_FORGETTING].extrinsic || have_spellforgettingstone()) && !(SpellLoss || u.uprops[SPELLS_LOST].extrinsic || have_spelllossstone()) && spellknow(i) == 100) pline("You are about to forget the %s spell.", spellname(i));
			if (!issoviet && !SpellColorCyan && !(SpellForgetting || u.uprops[SPELL_FORGETTING].extrinsic || have_spellforgettingstone()) && !(SpellLoss || u.uprops[SPELLS_LOST].extrinsic || have_spelllossstone()) && spellknow(i) == 0) pline("You no longer know how to cast the %s spell.", spellname(i));

		}

	    }
	return;
}

/*
 * Return TRUE if a spell was picked, with the spell index in the return
 * parameter.  Otherwise return FALSE.
 */
STATIC_OVL boolean
getspell(spell_no)
	int *spell_no;
{
	int nspells, idx;
	char ilet, lets[BUFSZ], qbuf[QBUFSZ];

	if (spellid(0) == NO_SPELL)  {
	    You("don't know any spells right now.");
	    return FALSE;
	}
	if (flags.menu_style == MENU_TRADITIONAL) {
	    /* we know there is at least 1 known spell */
	    for (nspells = 1; nspells < MAXSPELL
			    && spellid(nspells) != NO_SPELL; nspells++)
		continue;

	    if (nspells == 1)  strcpy(lets, "a");
	    else if (nspells < 27)  sprintf(lets, "a-%c", 'a' + nspells - 1);
	    else if (nspells == 27)  sprintf(lets, "a-z A");
	    else if (nspells < 53)
		sprintf(lets, "a-z A-%c", 'A' + nspells - 27);
	    else if (nspells == 53)  sprintf(lets, "a-z A-Z 0");
	    else if (nspells < 78)
		sprintf(lets, "a-z A-Z 0-%c", '0' + nspells - 53);
	    else  sprintf(lets, "a-z A-Z 0-9");

	    for(;;)  {
		sprintf(qbuf, "Cast which spell? [%s ?]", lets);
		if ((ilet = yn_function(qbuf, (char *)0, '\0')) == '?')
		    break;

		if (index(quitchars, ilet))
		    return FALSE;

		idx = spell_let_to_idx(ilet);
		if (idx >= 0 && idx < nspells) {
		    *spell_no = idx;
		    return TRUE;
		} else
		    You("don't know that spell.");
	    }
	}
	return dospellmenu( (SpellColorPink) ? "Your spells are pink." : (SpellColorRed) ? "Your spells are red." : (SpellColorGreen) ? "Your spells are green." : (SpellColorCyan) ? "Your spells are cyan." : (SpellColorBlue) ? "Your spells are blue." : (SpellColorWhite) ? "Your spells are white." : (SpellColorBlack) ? "Your spells are black." : (SpellColorGray) ? "Your spells are completely gray." : (SpellColorYellow) ? "Your spells are yellow." : "Choose which spell to cast",
			   SPELLMENU_CAST, spell_no);
}

/* the 'Z' command -- cast a spell */
int
docast()
{
	int spell_no;

	if (u.antimagicshell || (RngeAntimagicA && (moves % 10 == 0)) || (RngeAntimagicB && (moves % 5 == 0)) || (RngeAntimagicC && (moves % 2 == 0)) || (RngeAntimagicD) || (uarmc && uarmc->oartifact == ART_SHELLY && (moves % 3 == 0)) || (uarmc && uarmc->oartifact == ART_BLACK_VEIL_OF_BLACKNESS) || (uarmc && uarmc->oartifact == ART_ARABELLA_S_WAND_BOOSTER) || (uarmu && uarmu->oartifact == ART_ANTIMAGIC_SHELL) || (uarmu && uarmu->oartifact == ART_ANTIMAGIC_FIELD) || Role_if(PM_UNBELIEVER) ) {

		pline("Your anti-magic shell prevents spellcasting.");
		if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	}

	if (getspell(&spell_no)) {

		/* Spellbinder allows you to cast several spells in one turn, but not the same spell twice --Amy */

		if (u.spellbinder) {
			if ((spell_no == u.spellbinder1) || (spell_no == u.spellbinder2) || (spell_no == u.spellbinder3) || (spell_no == u.spellbinder4) || (spell_no == u.spellbinder5) || (spell_no == u.spellbinder6) || (spell_no == u.spellbinder7)) {
				pline("You already bound the same spell in this turn, and therefore it fails!");
				return 0;
			}
			if (u.spellbinder1 == -1) u.spellbinder1 = spell_no;
			else if (u.spellbinder2 == -1) u.spellbinder2 = spell_no;
			else if (u.spellbinder3 == -1) u.spellbinder3 = spell_no;
			else if (u.spellbinder4 == -1) u.spellbinder4 = spell_no;
			else if (u.spellbinder5 == -1) u.spellbinder5 = spell_no;
			else if (u.spellbinder6 == -1) u.spellbinder6 = spell_no;
			else if (u.spellbinder7 == -1) u.spellbinder7 = spell_no;
		}

		return spelleffects(spell_no, FALSE);
	}
	return 0;
}

void
castinertiaspell()
{

	pline("You control the %s spell flow.", spellname(u.inertiacontrolspellno));
	if (yn("Cast it?") == 'y') {

		if (spellid(u.inertiacontrolspellno) != u.inertiacontrolspell) {
			pline("The inertia controlled spell is no longer in place, and therefore cannot be auto-casted!");
			u.inertiacontrol = 0;
			u.inertiacontrolspell = -1;
			u.inertiacontrolspellno = -1;
			return;
		}
		spelleffects(u.inertiacontrolspellno, FALSE);

	}
}

STATIC_OVL const char*
spelltypemnemonic(int skill)
{
	switch (skill) {
	    case P_ATTACK_SPELL:
	        return "   attack";
	    case P_HEALING_SPELL:
		  return "  healing";
	    case P_DIVINATION_SPELL:
	        return "   divine";
	    case P_ENCHANTMENT_SPELL:
	        return "  enchant";
		case P_PROTECTION_SPELL:
	        return "  protect";
	    case P_BODY_SPELL:
	        return "     body";
	    case P_OCCULT_SPELL:
	        return "   occult";
	    case P_ELEMENTAL_SPELL:
	        return "elemental";
	    case P_CHAOS_SPELL:
	        return   "    chaos";
	    case P_MATTER_SPELL:
	        return "   matter";
	    default:
		impossible("Unknown spell skill, %d;", skill);
		return "";
	}
}

int
spell_skilltype(booktype)
int booktype;
{
	return (objects[booktype].oc_skill);
}

STATIC_OVL void
cast_protection()
{
	int loglev = 0;
	int l = u.ulevel;
	int natac = u.uac - u.uspellprot;
	int gain;

	/* loglev=log2(u.ulevel)+1 (1..5) */
	while (l) {
	    loglev++;
	    l /= 2;
	}

	/* The more u.uspellprot you already have, the less you get,
	 * and the better your natural ac, the less you get.
	 *
	 *	LEVEL AC    SPELLPROT from sucessive SPE_PROTECTION casts
	 *      1     10    0,  1,  2,  3,  4
	 *      1      0    0,  1,  2,  3
	 *      1    -10    0,  1,  2
	 *      2-3   10    0,  2,  4,  5,  6,  7,  8
	 *      2-3    0    0,  2,  4,  5,  6
	 *      2-3  -10    0,  2,  3,  4
	 *      4-7   10    0,  3,  6,  8,  9, 10, 11, 12
	 *      4-7    0    0,  3,  5,  7,  8,  9
	 *      4-7  -10    0,  3,  5,  6
	 *      7-15 -10    0,  3,  5,  6
	 *      8-15  10    0,  4,  7, 10, 12, 13, 14, 15, 16
	 *      8-15   0    0,  4,  7,  9, 10, 11, 12
	 *      8-15 -10    0,  4,  6,  7,  8
	 *     16-30  10    0,  5,  9, 12, 14, 16, 17, 18, 19, 20
	 *     16-30   0    0,  5,  9, 11, 13, 14, 15
	 *     16-30 -10    0,  5,  8,  9, 10
	 */
	gain = loglev - (int)u.uspellprot / (4 - min(3,(10 - natac)/10));

	if (gain > 0) {
	    if (!Blind) {
		const char *hgolden = hcolor(NH_GOLDEN);

		if (u.uspellprot)
		    pline_The("%s haze around you becomes more dense.",
			      hgolden);
		else
		    pline_The("%s around you begins to shimmer with %s haze.",
			/*[ what about being inside solid rock while polyd? ]*/
			(Underwater || Is_waterlevel(&u.uz)) ? "water" : "air",
			      an(hgolden));
	    }
	    u.uspellprot += gain;
	    u.uspmtime =
		(!(PlayerCannotUseSkills) && P_SKILL(spell_skilltype(SPE_PROTECTION)) >= P_EXPERT) ? 20 : 10;
	    if (!u.usptime)
		u.usptime = u.uspmtime;
	    find_ac();
	} else {
	    Your("skin feels warm for a moment.");
	}
}

STATIC_OVL void
cast_reflection()
{
	if (HReflecting) {
		if (!Blind)
			pline("The shimmering globe around you becomes slightly brighter.");
		else
			You_feel("slightly more smooth.");
	} else {
		if (!Blind)
			pline("A shimmering globe appears around you!");
		else
			You_feel("smooth.");
	}
	incr_itimeout(&HReflecting, rn1(10, HReflecting ? 4 : 30));
}


/* attempting to cast a forgotten spell will cause disorientation */
STATIC_OVL void
spell_backfire(spell)
int spell;
{
    long duration = (long)((spellev(spell) + 1) * 3);	 /* 6..24 */

    /* prior to 3.4.1, the only effect was confusion; it still predominates */
    switch (rn2(17)) {
    case 0:
    case 1:
    case 2:
    case 3: make_confused(HConfusion + duration, FALSE);			/* 40% */
	    break;
    case 4:
    case 5:
    case 6: make_confused(HConfusion + (2L * duration / 3L), FALSE);		/* 30% */
	    make_stunned(HStun + (duration / 3L), FALSE);
	    break;
    case 7:
    case 8: make_stunned(HStun + (2L * duration / 3L), FALSE);		/* 20% */
	    make_confused(HConfusion + (duration / 3L), FALSE);
	    break;
    case 9: make_stunned(HStun + duration, FALSE);			/* 10% */
	    break;
    case 10: make_numbed(HNumbed + duration, FALSE);			/* 10% */
	    break;
    case 11: make_frozen(HFrozen + duration, FALSE);			/* 10% */
	    break;
    case 12: make_burned(HBurned + duration, FALSE);			/* 10% */
	    break;
    case 13: make_feared(HFeared + duration, FALSE);			/* 10% */
	    break;
    case 14: make_blinded(Blinded + duration, FALSE);			/* 10% */
	    break;
    case 15: make_hallucinated(HHallucination + duration, FALSE, 0L);			/* 10% */
	    break;
    case 16: make_dimmed(HDimmed + duration, FALSE);			/* 10% */
	    break;
    }
    return;
}

int
spelleffects(spell, atme)
int spell;
boolean atme;
{
	int energy, damage, chance, n, intell;
	int hungr;
	int skill, role_skill;
	boolean confused = ((Confusion != 0) && !Conf_resist);
	struct obj *pseudo;
	struct obj *otmp;
	int confusionchance = 0;

	/*
	 * Find the skill the hero has in a spell type category.
	 * See spell_skilltype for categories.
	 */
	skill = spell_skilltype(spellid(spell));
	role_skill = P_SKILL(skill);
	if (PlayerCannotUseSkills) role_skill = P_ISRESTRICTED;

	/*
	 * Spell casting no longer affects knowledge of the spell. A
	 * decrement of spell knowledge is done every turn.
	 */
	if (spellknow(spell) <= 0) {
	    Your("knowledge of this spell is twisted.");
	    pline("It invokes nightmarish images in your mind...");
	    spell_backfire(spell);
	    if (!rn2(25)) {
		badeffect();
	    }
	    return(1);
	} else if (spellknow(spell) <= 100) {
	    You("strain to recall the spell.");
	} else if (spellknow(spell) <= 1000) {
	    Your("knowledge of this spell is growing faint.");
	}
	energy = (spellev(spell) * 5);    /* 5 <= energy <= 35 */
	if (SpellColorYellow) energy *= 2;
	if (SpellColorWhite) energy *= 4;

	/* only being easier to cast is not good enough for the "special spell", since you can't have a failure rate
	 * lower than 0%. Reduce cost of casting the special spell to 80%! --Amy */
	if (spellid(spell) == urole.spelspec) { energy *= 4; energy /= 5; }

	/* Some spells are just plain too powerful, and need to be nerfed. Sorry. --Amy */
	if (spellid(spell) == SPE_FINGER_OF_DEATH) energy *= 2;
	if (spellid(spell) == SPE_TIME_STOP) energy *= 2;
	if (spellid(spell) == SPE_HELLISH_BOLT) energy *= 2;
	if (spellid(spell) == SPE_PETRIFY) { energy *= 5; energy /= 2;}
	if (spellid(spell) == SPE_GODMODE) { energy *= 5; energy /= 2;}
	if (spellid(spell) == SPE_DISINTEGRATION) energy *= 3;
	if (spellid(spell) == SPE_DISINTEGRATION_BEAM) energy *= 3;
	if (spellid(spell) == SPE_FIXING) energy *= 3;
	if (spellid(spell) == SPE_CHROMATIC_BEAM) { energy *= 10; energy /= 7;}
	if (spellid(spell) == SPE_FORCE_BOLT) { energy *= 3; energy /= 2;}
	if (spellid(spell) == SPE_HEALING) { energy *= 3; energy /= 2;}
	if (spellid(spell) == SPE_FIREBALL) energy *= 2;
	if (spellid(spell) == SPE_FIRE_BOLT) { energy *= 3; energy /= 2;}
	if (spellid(spell) == SPE_CONE_OF_COLD) { energy *= 3; energy /= 2;}
	if (spellid(spell) == SPE_MULTIBEAM) { energy *= 6; energy /= 5;}
	if (spellid(spell) == SPE_CALL_THE_ELEMENTS) { energy *= 7; energy /= 4;}
	if (spellid(spell) == SPE_INFERNO) { energy *= 3; energy /= 2;}
	if (spellid(spell) == SPE_ICE_BEAM) { energy *= 3; energy /= 2;}
	if (spellid(spell) == SPE_HYPER_BEAM) { energy *= 4; energy /= 3;}
	if (spellid(spell) == SPE_ELEMENTAL_BEAM) { energy *= 6; energy /= 5;}
	if (spellid(spell) == SPE_NATURE_BEAM) { energy *= 5; energy /= 4;}

	/* slight mana cost decrease if you're very skilled, to make skill matter more --Amy */
	if (role_skill == P_SKILLED) { energy *= 19; energy /= 20;}
	if (role_skill == P_EXPERT) { energy *= 18; energy /= 20;}
	if (role_skill == P_MASTER) { energy *= 17; energy /= 20;}
	if (role_skill == P_GRAND_MASTER) { energy *= 16; energy /= 20;}
	if (role_skill == P_SUPREME_MASTER) { energy *= 15; energy /= 20;}

	if (Role_if(PM_MAHOU_SHOUJO) && energy > 1) energy /= 2; /* Casting any sort of magic uses half power for them */
	if (uwep && uwep->oartifact == ART_MANA_METER_BOOSTER) {
		energy *= 9;
		energy /= 10;
	}

	if (uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "uncanny gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "sverkh''yestestvennyye perchatki") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "dahshatli qo'lqop") )) {
		energy *= 11;
		energy /= 10;
	}

	if (uleft && uleft->oartifact == ART_HENRIETTA_S_MAGICAL_AID) {
		energy *= 4;
		energy /= 5;
	}

	if (Upolyd && dmgtype(youmonst.data, AD_SPEL) ) {
		energy *= 19;
		energy /= 20;
	}
	if (Upolyd && dmgtype(youmonst.data, AD_CLRC) ) {
		energy *= 19;
		energy /= 20;
	}
	if (Upolyd && dmgtype(youmonst.data, AD_CAST) ) {
		energy *= 9;
		energy /= 10;
	}

	if (uarmc && uarmc->oartifact == ART_GAGARIN_S_TRANSLATOR) {
		energy *= 9;
		energy /= 10;
	}

	if (uright && uright->oartifact == ART_HENRIETTA_S_MAGICAL_AID) {
		energy *= 4;
		energy /= 5;
	}
	if (Role_if(PM_ELEMENTALIST) && skill == P_ELEMENTAL_SPELL) {energy *= 3; energy /= 4;}

	if ((uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "occultism gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "perchatki okkul'tizma") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "folbinlik qo'lqop") )) && skill == P_OCCULT_SPELL) {
		energy *= 4;
		energy /= 5;
	}

	if (Race_if(PM_MANSTER) && energy > 1) {
		energy *= 2;
		energy /= 3;
	}

	if (Role_if(PM_MAHOU_SHOUJO) && (energy > 1) && uarmc && OBJ_DESCR(objects[uarmc->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "weeb cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "zese plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "yaponiya ucube rido") ) ) { energy *= 9; energy /= 10;}

	if (u.uhunger <= 10 && spellid(spell) != SPE_DETECT_FOOD && spellid(spell) != SPE_SATISFY_HUNGER) {
		You("are too hungry to cast that spell.");
		if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return(0);
	} else if (ACURR(A_STR) < 4)  {
		You("lack the strength to cast spells.");
		if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return(0);
	} else if(check_capacity(
		"Your concentration falters while carrying so much stuff.")) {
		if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    return (1);
	} else if (!freehandX()) {
		Your("arms are not free to cast!");
		if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return (0);
	} else if (Muteness || u.uprops[MUTENESS].extrinsic || have_mutenessstone()) {
		pline("You're muted!");
		if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return (0);
	} else if (tech_inuse(T_SILENT_OCEAN)) {
		pline("The silent ocean prevents you from spellcasting.");
		if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return (0);
	} else if (isevilvariant && (Strangled || is_silent(youmonst.data) || !has_head(youmonst.data) || youmonst.data->msound == MS_BUZZ || youmonst.data->msound == MS_BURBLE || youmonst.data->msound == MS_GURGLE)) {
		pline("You're unable to chant any magical formula!");
		if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return (0);
	}


	/* Casting any sort of magic as a mahou shoujo or naga does not cause hunger */
	/* Amy edit: but if you're satiated, you always use the standard amount of nutrition. That way, hungerless casting
	 * does not rob you of the ability to get out of satiated status by repeatedly casting spells. */

		if ((!Role_if(PM_MAHOU_SHOUJO) && !Race_if(PM_HUMANLIKE_NAGA) && (spellid(spell) != SPE_DETECT_FOOD) && (spellid(spell) != SPE_SATISFY_HUNGER) ) || u.uhunger > 2500 ) {
		hungr = energy * 2;

			/* If hero is a wizard, their current intelligence
			 * (bonuses + temporary + current)
			 * affects hunger reduction in casting a spell.
			 * 1. int = 17-18 no reduction
			 * 2. int = 16    1/4 hungr
			 * 3. int = 15    1/2 hungr
			 * 4. int = 1-14  normal reduction
			 * The reason for this is:
			 * a) Intelligence affects the amount of exertion
			 * in thinking.
			 * b) Wizards have spent their life at magic and
			 * understand quite well how to cast spells.
			 */
			intell = acurr(A_INT);
			if (!Role_if(PM_WIZARD)) intell = 10;
			if (u.uhunger > 2500) intell = 10;
			switch (intell) {
				case 25: case 24: case 23: case 22:
				case 21: case 20: case 19: case 18:
				case 17: hungr = 0; break;
				case 16: hungr /= 4; break;
				case 15: hungr /= 2; break;
			}
	}
	else
		hungr = 0;
			/* don't put player (quite) into fainting from
			 * casting a spell, particularly since they might
			 * not even be hungry at the beginning; however,
			 * this is low enough that they must eat before
			 * casting anything else except detect food
			 */

	if (u.uhave.amulet && u.amuletcompletelyimbued) {
		/* casting while you have the fully imbued amulet always causes extra hunger no matter what --Amy
		 * but a non-imbued one doesn't increase hunger cost */
		hungr += rnd(2*energy);
	}

			if (hungr > u.uhunger-3)
				hungr = u.uhunger-3;
	if (energy > u.uen)  {
		if (role_skill >= P_SKILLED) You("don't have enough energy to cast that spell.");
		else You("don't have enough energy to cast that spell. The required amount was %d.",energy);
		if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		/* WAC/ALI Experts can override with HP/hunger loss */
		if ((role_skill >= P_SKILLED) && (u.uhpmax > (energy / 5)) && (yn("Continue? Doing so may damage your maximum health.") == 'y')) {
			energy -= u.uen;
			hungr += energy * 2;
			if (hungr > u.uhunger - 1)
				hungr = u.uhunger - 1;

			if (energy > 4) {
			/* otherwise, skilled godmode at 0% fail equals instawin. --Amy */
				pline("Your maximum health was reduced by %d.", energy / 5);
				u.uhpmax -= (energy / 5);
				u.uhp -= (energy / 5);
				if (u.uhp < 1) {
					u.youaredead = 1;
					done(DIED);
					u.youaredead = 0;
				}
			}

			losehp(energy,"spellcasting exhaustion", KILLED_BY);
			if (role_skill < P_EXPERT) exercise(A_WIS, FALSE);
			energy = u.uen;
		} else {
			if (role_skill >= P_SKILLED) pline("The required amount was %d.",energy);
			return 0;
		}
	}

	/* come on, you should be able to cast using less nutrition if your skill is higher! --Amy */

	if (u.uhunger <= 2500) { /* But only if you're not satiated (see above) */

	if ( role_skill == P_BASIC) {hungr *= 85; hungr /= 100;}
	if ( role_skill == P_SKILLED) {hungr *= 70; hungr /= 100;}
	if ( role_skill == P_EXPERT) {hungr *= 55; hungr /= 100;}
	if ( role_skill == P_MASTER) {hungr *= 40; hungr /= 100;}
	if ( role_skill == P_GRAND_MASTER) {hungr *= 25; hungr /= 100;}
	if ( role_skill == P_SUPREME_MASTER) {hungr *= 10; hungr /= 100;}

	/* casting it often (and thereby keeping it in memory) should also reduce hunger... */
	if ( spellknow(spell) >= 10000) {hungr *= 9; hungr /= 10;}
	if ( spellknow(spell) >= 20000) {hungr *= 9; hungr /= 10;}
	if ( spellknow(spell) >= 30000) {hungr *= 9; hungr /= 10;}
	if ( spellknow(spell) >= 40000) {hungr *= 9; hungr /= 10;}
	if ( spellknow(spell) >= 50000) {hungr *= 9; hungr /= 10;}
	if ( spellknow(spell) >= 60000) {hungr *= 9; hungr /= 10;}
	if ( spellknow(spell) >= 70000) {hungr *= 9; hungr /= 10;}

	}

	if (SpellColorBlue) hungr += 100;

	if (hungr < 0) hungr = 0; /* fail safe */

	morehungry(hungr);

	if (SpellColorGray) {

		You("chant the magical formula...");
		nomul(-2, "casting a completely gray spell", TRUE);

	}

	if (MiscastBug || u.uprops[MISCAST_BUG].extrinsic || have_miscastingstone()) {
		badeffect();
	}

	if (SpellColorRed && !rn2(3)) {

		int lcount = rnd(monster_difficulty() ) + 1;

		switch (rn2(11)) {
		    case 0: make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON),20),
				"red spell sickness", TRUE, SICK_NONVOMITABLE);
			    break;
		    case 1: make_blinded(Blinded + lcount, TRUE);
			    break;
		    case 2: if (!Confusion)
				You("suddenly feel %s.", Hallucination ? "trippy" : "confused");
			    make_confused(HConfusion + lcount, TRUE);
			    break;
		    case 3: make_stunned(HStun + lcount, TRUE);
			    break;
		    case 4: make_numbed(HNumbed + lcount, TRUE);
			    break;
		    case 5: make_frozen(HFrozen + lcount, TRUE);
			    break;
		    case 6: make_burned(HBurned + lcount, TRUE);
			    break;
		    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE);
			    break;
		    case 8: (void) make_hallucinated(HHallucination + lcount, TRUE, 0L);
			    break;
		    case 9: make_feared(HFeared + lcount, TRUE);
			    break;
		    case 10: make_dimmed(HDimmed + lcount, TRUE);
			    break;
		}

	}

	if (SpellColorPink) {
		pline(fauxmessage());
	}

	if (SpellColorGreen) {
		register int zx, zy;
		for(zx = 0; zx < COLNO; zx++) for(zy = 0; zy < ROWNO; zy++) {
			/* Zonk all memory of this location. */
			levl[zx][zy].seenv = 0;
			levl[zx][zy].waslit = 0;
			clear_memory_glyph(zx, zy, S_stone);
		}

		docrt();
		vision_recalc(0);

	}

	chance = percent_success(spell);
	if (confused) {
		pline("You try to cast a spell in confusion.");

		/* Amy addition: very high skill may allow you to get spells off even while confused */
		if (!PlayerCannotUseSkills) {
			confusionchance = 0;
			if ( role_skill == P_SKILLED) confusionchance = 25;
			if ( role_skill == P_EXPERT) confusionchance = 50;
			if ( role_skill == P_MASTER) confusionchance = 75;
			if ( role_skill == P_GRAND_MASTER) confusionchance = 90;
			if ( role_skill == P_SUPREME_MASTER) confusionchance = 100;
		}
	}

	if ( (confused && spellid(spell) != SPE_CURE_CONFUSION && spellid(spell) != SPE_CURE_RANDOM_STATUS && (confusionchance < rnd(100)) && rn2(Conf_resist ? 2 : 10) ) || (rnd(100) > chance)) {
		if (!issoviet) pline("You fail to cast the spell correctly.");
		else pline("HA HA HA HA HA, tip bloka l'da sdelal vy ne zaklinaniye!");
		if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		if (!rn2(100)) { /* evil patch idea by Amy: failure effect */
			pline("In fact, you cast the spell incorrectly in a way that causes bad stuff to happen...");
			badeffect();
		}

#ifdef ALLEG_FX
                if (iflags.usealleg) alleg_aura(u.ux, u.uy, P_ATTACK_SPELL-1);
#endif

		/* Higher spellcasting skills mean failure takes less mana. --Amy */

		u.uen -= ((energy * 50 / ((role_skill == P_SUPREME_MASTER) ? 240 : (role_skill == P_GRAND_MASTER) ? 220 : (role_skill == P_MASTER) ? 200 : (role_skill == P_EXPERT) ? 180 : (role_skill == P_SKILLED) ? 160 : (role_skill == P_BASIC) ? 140 : 120)) + 1) ;
		flags.botl = 1;
		return(1);
	}

	/* Players could cheat if they had just barely enough mana for casting a spell without the increased drain.
	 * They'd just need to keep trying until the extra mana costs are randomly very low.
	 * Prevent players from abusing this by calculating the extra drain _after_ the other checks. --Amy */
	if (u.uhave.amulet && u.amuletcompletelyimbued) {
		You_feel("the amulet draining your energy away.");
		energy += rnd(2*energy);
	}

	u.uen -= energy;
	/* And if the amulet drained it below zero, set it to zero and just make the spell fail now. */
	if (u.uhave.amulet && u.amuletcompletelyimbued && u.uen < 0) {
		pline("You are exhausted, and fail to cast the spell due to the amulet draining all your energy away.");
		if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		u.uen = 0;
		return(1);
	}

	if (Role_if(PM_MAHOU_SHOUJO)) { /* Casting any sort of magic causes all monsters on a level to 
      become alert of your location, due to mahou shoujo always announcing their attacks. */

	wake_nearby();
	verbalize("%s!",spellname(spell) );

	}
	
	flags.botl = 1;
	exercise(A_WIS, TRUE);

	/* pseudo is a temporary "false" object containing the spell stats. */
	pseudo = mksobj(spellid(spell), FALSE, 2);
	if (!pseudo) {
		pline("The spell failed spontaneously!");
		if (flags.moreforced && !(MessageSuppression || u.uprops[MESSAGE_SUPPRESSION_BUG].extrinsic || have_messagesuppressionstone() )) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return(1);
	}
	if (pseudo->otyp == GOLD_PIECE) pseudo->otyp = spellid(spell); /* minimalist fix */
	pseudo->blessed = pseudo->cursed = 0;
	pseudo->quan = 20L;			/* do not let useup get it */

	/* WAC -- If skilled enough,  will act like a blessed version */
	if (role_skill >= P_SKILLED) { /* made it depend on skill level --Amy */
		if (!rn2(10) && role_skill == P_SKILLED) pseudo->blessed = 1;
		if (!rn2(8) && role_skill == P_EXPERT) pseudo->blessed = 1;
		if (!rn2(6) && role_skill == P_MASTER) pseudo->blessed = 1;
		if (!rn2(5) && role_skill == P_GRAND_MASTER) pseudo->blessed = 1;
		if (!rn2(4) && role_skill == P_SUPREME_MASTER) pseudo->blessed = 1;
	}

#ifdef ALLEG_FX
        if (iflags.usealleg) alleg_aura(u.ux, u.uy, skill);
#endif
	switch(pseudo->otyp)  {
	/*
	 * At first spells act as expected.  As the hero increases in skill
	 * with the appropriate spell type, some spells increase in their
	 * effects, e.g. more damage, further distance, and so on, without
	 * additional cost to the spellcaster.
	 */
	case SPE_MAGIC_MISSILE:
	case SPE_FIREBALL:
	case SPE_CONE_OF_COLD:
	case SPE_LIGHTNING:
	case SPE_ACID_STREAM:
	case SPE_SOLAR_BEAM:
	case SPE_PSYBEAM:
	case SPE_POISON_BLAST:
		if (tech_inuse(T_SIGIL_TEMPEST)) {
		    weffects(pseudo);
		    break;
		} /* else fall through... */
	/* these spells are all duplicates of wand effects */
	case SPE_FORCE_BOLT:
	case SPE_WATER_BOLT:
	case SPE_MULTIBEAM:
	case SPE_CALL_THE_ELEMENTS:
	case SPE_MANA_BOLT:
	case SPE_SNIPER_BEAM:
	case SPE_BLINDING_RAY:
	case SPE_ENERGY_BOLT:
	case SPE_GRAVITY_BEAM:
	case SPE_BUBBLEBEAM:
	case SPE_DREAM_EATER:
	case SPE_GOOD_NIGHT:
	case SPE_INFERNO:
	case SPE_ICE_BEAM:
	case SPE_THUNDER:
	case SPE_SLUDGE:
	case SPE_TOXIC:
	case SPE_NETHER_BEAM:
	case SPE_AURORA_BEAM:
	case SPE_CHLOROFORM:
	case SPE_SLEEP:
	case SPE_KNOCK:
	case SPE_LOCK_MANIPULATION:
	case SPE_SLOW_MONSTER:
	case SPE_RANDOM_SPEED:
	case SPE_INERTIA:
	case SPE_CLONE_MONSTER:
	case SPE_WIZARD_LOCK:
	case SPE_DIG:
	case SPE_VOLT_ROCK:
	case SPE_BATTERING_RAM:
	case SPE_WATER_FLAME:
	case SPE_TURN_UNDEAD:
	case SPE_POLYMORPH:
	case SPE_CHAOS_BOLT:
	case SPE_HELLISH_BOLT:
	case SPE_HORRIFY:
	case SPE_MUTATION:
	case SPE_TELEPORT_AWAY:
	case SPE_CANCELLATION:
	case SPE_VANISHING:
	case SPE_FINGER_OF_DEATH:
	case SPE_LIGHT:
	case SPE_DARKNESS:
	case SPE_DETECT_UNSEEN:
	case SPE_HEALING:
	case SPE_EXTRA_HEALING:
	case SPE_FULL_HEALING:
	case SPE_DRAIN_LIFE:
	case SPE_TIME:
	case SPE_STONE_TO_FLESH:
	case SPE_FINGER:
	case SPE_MAKE_VISIBLE:
	case SPE_STUN_MONSTER:
	case SPE_DISINTEGRATION:
	case SPE_DISINTEGRATION_BEAM:
	case SPE_CHROMATIC_BEAM:
	case SPE_ELEMENTAL_BEAM:
	case SPE_NATURE_BEAM:
	case SPE_PETRIFY:
	case SPE_WIND:
	case SPE_FIRE_BOLT:
	case SPE_HYPER_BEAM:
	case SPE_PARALYSIS:
		if (!(objects[pseudo->otyp].oc_dir == NODIR)) {
			if (atme) u.dx = u.dy = u.dz = 0;
			else if (!getdir((char *)0)) {
			    /* getdir cancelled, re-use previous direction */

				/* Amy edit: this is absolute bullshit behavior, because it's very easy to mistype. It should
				 * NEVER direct the zap at yourself unless you specifically told it so, but the vanilla behavior
				 * means that knowing finger of death means death to interface screw can happen at any time.
				 * I've made magic resistance work too, see zap.c, but those that don't have magic resistance
				 * shall not be screwed over either. Screw you, programmers. And while you're at it,
				 * read up on game design and specifically interface design! :P --Amy */

			    pline_The("magical energy is released!");
			    if (u.dx == 0 && u.dy == 0) {
				You("would have hit yourself, but to reduce YASD potential the blast goes in a random direction.");
				u.dz = 0;
				confdir();
			    }
			}
			if(!u.dx && !u.dy && !u.dz) {
			    if ((damage = zapyourself(pseudo, TRUE)) != 0) {
				char buf[BUFSZ];
				sprintf(buf, "zapped %sself with a spell", uhim());
				losehp(damage, buf, NO_KILLER_PREFIX);
			    }
			} else weffects(pseudo);
		} else weffects(pseudo);
		update_inventory();	/* spell may modify inventory */
		if (pseudo->otyp == SPE_TELEPORT_AWAY || pseudo->otyp == SPE_POLYMORPH || pseudo->otyp == SPE_MUTATION || pseudo->otyp == SPE_CANCELLATION) {
			if (!rn2(5)) {
				pline("The magical energy goes out of control!");
				badeffect();
			}
		}
		if (pseudo->otyp == SPE_VANISHING) {
			if (!rn2(50)) {
				pline("The magical energy goes out of control!");
				badeffect();
			}
		}
		if (pseudo->otyp == SPE_WATER_BOLT) {
			make_confused(HConfusion + 5, TRUE);
			make_stunned(HStun + 5, TRUE);
			if (!rn2(20)) {
				pline("The spell backfires!");
				badeffect();
			}
			if (!(InterfaceScrewed || u.uprops[INTERFACE_SCREW].extrinsic || have_interfacescrewstone())) (void) doredraw();
		}
		if (pseudo->otyp == SPE_WIND) {
			pushplayer();
			pline("The winds hurt you!");
			losehp(rnd(10), "winds", KILLED_BY);
		}
		if (pseudo->otyp == SPE_CHAOS_BOLT) {
			if (!rn2(3)) {
				pline("The magical energy goes out of control!");
				make_hallucinated(HHallucination + rnd(20), FALSE, 0L);
				if (!rn2(10)) set_itimeout(&HeavyHallu, HHallucination);
				if (!rn2(2)) u.halresdeactivated = rnz(50);
			}
		}
		if (pseudo->otyp == SPE_HELLISH_BOLT) {
			if (!rn2(2)) {
				pline("The magical energy goes out of control!");
				make_hallucinated(HHallucination + rnd(100), FALSE, 0L);
				if (!rn2(5)) set_itimeout(&HeavyHallu, HHallucination);
				if (rn2(3)) u.halresdeactivated = rnz(200);
			}
		}
		break;
	/* these are all duplicates of scroll effects */
	case SPE_REMOVE_CURSE:
	case SPE_CONFUSE_MONSTER:
	case SPE_DETECT_FOOD:
	case SPE_CAUSE_FEAR:
#if 0
		/* high skill yields effect equivalent to blessed scroll */
		if (role_skill >= P_SKILLED) pseudo->blessed = 1;
#endif
		/* fall through */
	case SPE_CHARM_MONSTER:
	case SPE_MAGIC_MAPPING:
	case SPE_CREATE_MONSTER:
	case SPE_IDENTIFY:
	case SPE_DESTROY_ARMOR:
	case SPE_COMMAND_UNDEAD:                
	case SPE_SUMMON_UNDEAD:
		if (rn2(5)) pseudo->blessed = 0;
		(void) seffects(pseudo);
		break;
	case SPE_CHARGING:
		pseudo->blessed = 0;
		(void) seffects(pseudo);
		break;

	case SPE_ENCHANT_WEAPON:
	case SPE_ENCHANT_ARMOR:
		if (role_skill >= P_SUPREME_MASTER) n = 7;
		else if (role_skill >= P_GRAND_MASTER) n = 8;
		else if (role_skill >= P_MASTER) n = 9;
		else if (role_skill >= P_EXPERT) n = 10;
		else if (role_skill >= P_SKILLED) n = 11;
		else if (role_skill >= P_BASIC) n = 12;
		else n = 14;	/* Unskilled or restricted */
		if (!rn2(n)) {
		    pseudo->blessed = 0;
		    (void) seffects(pseudo);
		    if (u.uenmax > 0) {
			    u.uenmax -= 1;
			    pline("The strain from casting such a powerful spell drains your maximum mana.");
		    } else {
			    pline("Casting such a powerful spell from hitpoints causes strong backlash.");
			    badeffect(); badeffect(); badeffect(); badeffect(); badeffect();
		    }
		} else
		    Your("enchantment failed!");
		break;

	case SPE_ENTRAPPING:

		trap_detectX((struct obj *)0);
		exercise(A_WIS, TRUE);

		break;
	/* these are all duplicates of potion effects */
	case SPE_HASTE_SELF:
	case SPE_DETECT_TREASURE:
	case SPE_DETECT_MONSTERS:
	case SPE_LEVITATION:
	case SPE_RESTORE_ABILITY:
	case SPE_BANISHING_FEAR:
#if 0
		/* high skill yields effect equivalent to blessed potion */
		if (role_skill >= P_SKILLED) pseudo->blessed = 1;
#endif
		/* fall through */
	case SPE_INVISIBILITY:
		if (rn2(5)) pseudo->blessed = 0;
		(void) peffects(pseudo);
		break;
	case SPE_CURE_BLINDNESS:
		healup(0, 0, FALSE, TRUE);
		break;
	case SPE_AMNESIA:
		You_feel("dizzy!");
		forget(1 + rn2(5));
		break;
	case SPE_AGGRAVATE_MONSTER:
		You_feel("that monsters are aware of your presence.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Dazhe sovetskaya Pyat' Lo obostryayetsya v vashem nizkom igrovom masterstve." : "Woaaaaaah!");
		aggravate();
		break;
	case SPE_CURSE_ITEMS:
		You_feel("as if you need some help.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
		rndcurse();
		break;
	case SPE_FUMBLING:
		if (!Fumbling) pline("You start fumbling.");
		HFumbling = FROMOUTSIDE | rnd(5);
		set_itimeout(&HFumbling, 2);
		u.fumbleduration += rnz(1000);
		break;
	case SPE_REMOVE_BLESSING:
		{

		    if (Hallucination)
			You_feel("the power of the Force against you!");
		    else
			You_feel("like you need some help.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");

		    register struct obj *obj, *obj2;
		    for (obj = invent; obj; obj = obj2) {
		      obj2 = obj->nobj;
			if (obj->blessed && !stack_too_big(obj) ) unbless(obj);
		    }
		}

		break;
	case SPE_CORRODE_METAL:
		{

		    pline("You are covered by %s particles!",NH_GREEN);

		    register struct obj *obj, *obj2;
		    for (obj = invent; obj; obj = obj2) {
		      obj2 = obj->nobj;
			rust_dmg(obj, xname(obj), 3, TRUE, &youmonst);
		    }
		}

		break;
	case SPE_DETECT_ARMOR_ENCHANTMENT:
		{

		    pline("You detect the enchantment status of your armor!");

		    register struct obj *obj, *obj2;
		    for (obj = invent; obj; obj = obj2) {
		      obj2 = obj->nobj;

			if (obj->oclass == ARMOR_CLASS) obj->known = TRUE;

		    }
		}

		break;

	case SPE_THRONE_GAMBLE:

	    if (rnd(6) > 4)  {
		switch (rnd(20))  {
		    case 1:
			(void) adjattrib(rn2(A_MAX), -rn1(4,3), FALSE);
			losehp(rnd(10), "cursed throne", KILLED_BY_AN);
			break;
		    case 2:
			(void) adjattrib(rn2(A_MAX), 1, FALSE);
			break;
		    case 3:
			pline("A%s electric shock shoots through your body!",
			      (Shock_resistance) ? "n" : " massive");
			losehp(Shock_resistance ? rnd(6) : rnd(30),
			       "electric chair", KILLED_BY_AN);
			exercise(A_CON, FALSE);
			break;
		    case 4:
			You_feel("much, much better!");
			if (Upolyd) {
			    if (u.mh >= (u.mhmax - 5))  u.mhmax += 4;
			    u.mh = u.mhmax;
			}
			if(u.uhp >= (u.uhpmax - 5))  u.uhpmax += 4;
			u.uhp = u.uhpmax;
			make_blinded(0L,TRUE);
			make_sick(0L, (char *) 0, FALSE, SICK_ALL);
			heal_legs();
			flags.botl = 1;
			break;
		    case 5:
			take_gold();
			break;
		    case 6:

			/* no wishes --Amy */
			You_feel("your luck is changing.");
			change_luck(5);

			break;
		    case 7:
			{
			register int cnt = rnd(10);

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			pline("A voice echoes:");
			verbalize("Thy audience hath been summoned, %s!",
				  flags.female ? "Dame" : "Sire");
			while(cnt--)
			    (void) makemon(courtmon(), u.ux, u.uy, NO_MM_FLAGS);

			u.aggravation = 0;

			break;
			}
		    case 8:
			pline("A voice echoes:");
			verbalize("By thy Imperious order, %s...",
				  flags.female ? "Dame" : "Sire");
			do_genocide(5);	/* REALLY|ONTHRONE, see do_genocide() */
			break;
		    case 9:
			pline("A voice echoes:");
			verbalize("A curse upon thee for sitting upon this most holy throne!");
			if (Luck > 0)  {
			    make_blinded(Blinded + rn1(100,250),TRUE);
			} else	    rndcurse();
			break;
		    case 10:
			if (Luck < 0 || (HSee_invisible & INTRINSIC))  {
				if (level.flags.nommap) {
					pline(
					"A terrible drone fills your head!");
					make_confused(HConfusion + rnd(30),
									FALSE);
				} else {
					pline("An image forms in your mind.");
					do_mapping();
				}
			} else  {
				Your("vision becomes clear.");
				HSee_invisible |= FROMOUTSIDE;
				newsym(u.ux, u.uy);
			}
			break;
		    case 11:
			if (Luck < 0)  {
			    You_feel("threatened.");
			    aggravate();
			} else  {

			    You_feel("a wrenching sensation.");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Tam net nikakoy zashchity. Tam net nikakoy nadezhdy. Yedinstvennoye, chto yest'? Uverennost' v tom, chto vy, igrok, budet umeret' uzhasnoy i muchitel'noy smert'yu." : "SCHRING!");
			    tele();		/* teleport him */
			}
			break;
		    case 12:
			You("are granted an insight!");
			if (invent) {
			    /* rn2(5) agrees w/seffects() */
			    identify_pack(rn2(5), 0);
			}
			break;
		    case 13:
			Your("mind turns into a pretzel!");
			make_confused(HConfusion + rn1(7,16),FALSE);
			break;
		    case 14:
			You("are granted some new skills!"); /* new effect that unrestricts skills --Amy */

			int acquiredskill;
			acquiredskill = 0;

			pline("Pick a skill to unrestrict. The prompt will loop until you actually make a choice.");

			while (acquiredskill == 0) { /* ask the player what they want --Amy */

			if (P_RESTRICTED(P_DAGGER) && yn("Do you want to learn the dagger skill?")=='y') {
				    unrestrict_weapon_skill(P_DAGGER);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_KNIFE) && yn("Do you want to learn the knife skill?")=='y') {
				    unrestrict_weapon_skill(P_KNIFE);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_AXE) && yn("Do you want to learn the axe skill?")=='y') {
				    unrestrict_weapon_skill(P_AXE);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_PICK_AXE) && yn("Do you want to learn the pick-axe skill?")=='y') {
				    unrestrict_weapon_skill(P_PICK_AXE);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_SHORT_SWORD) && yn("Do you want to learn the short sword skill?")=='y') {
				    unrestrict_weapon_skill(P_SHORT_SWORD);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_BROAD_SWORD) && yn("Do you want to learn the broad sword skill?")=='y') {
				    unrestrict_weapon_skill(P_BROAD_SWORD);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_LONG_SWORD) && yn("Do you want to learn the long sword skill?")=='y') {
				    unrestrict_weapon_skill(P_LONG_SWORD);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_TWO_HANDED_SWORD) && yn("Do you want to learn the two-handed sword skill?")=='y') {
				    unrestrict_weapon_skill(P_TWO_HANDED_SWORD);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_SCIMITAR) && yn("Do you want to learn the scimitar skill?")=='y') {
				    unrestrict_weapon_skill(P_SCIMITAR);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_SABER) && yn("Do you want to learn the saber skill?")=='y') {
				    unrestrict_weapon_skill(P_SABER);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_CLUB) && yn("Do you want to learn the club skill?")=='y') {
				    unrestrict_weapon_skill(P_CLUB);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_PADDLE) && yn("Do you want to learn the paddle skill?")=='y') {
				    unrestrict_weapon_skill(P_PADDLE);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_MACE) && yn("Do you want to learn the mace skill?")=='y') {
				    unrestrict_weapon_skill(P_MACE);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_MORNING_STAR) && yn("Do you want to learn the morning star skill?")=='y') {
				    unrestrict_weapon_skill(P_MORNING_STAR);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_FLAIL) && yn("Do you want to learn the flail skill?")=='y') {
				    unrestrict_weapon_skill(P_FLAIL);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_HAMMER) && yn("Do you want to learn the hammer skill?")=='y') {
				    unrestrict_weapon_skill(P_HAMMER);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_QUARTERSTAFF) && yn("Do you want to learn the quarterstaff skill?")=='y') {
				    unrestrict_weapon_skill(P_QUARTERSTAFF);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_POLEARMS) && yn("Do you want to learn the polearms skill?")=='y') {
				    unrestrict_weapon_skill(P_POLEARMS);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_SPEAR) && yn("Do you want to learn the spear skill?")=='y') {
				    unrestrict_weapon_skill(P_SPEAR);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_JAVELIN) && yn("Do you want to learn the javelin skill?")=='y') {
				    unrestrict_weapon_skill(P_JAVELIN);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_TRIDENT) && yn("Do you want to learn the trident skill?")=='y') {
				    unrestrict_weapon_skill(P_TRIDENT);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_LANCE) && yn("Do you want to learn the lance skill?")=='y') {
				    unrestrict_weapon_skill(P_LANCE);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_BOW) && yn("Do you want to learn the bow skill?")=='y') {
				    unrestrict_weapon_skill(P_BOW);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_SLING) && yn("Do you want to learn the sling skill?")=='y') {
				    unrestrict_weapon_skill(P_SLING);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_FIREARM) && yn("Do you want to learn the firearms skill?")=='y') {
				    unrestrict_weapon_skill(P_FIREARM);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_CROSSBOW) && yn("Do you want to learn the crossbow skill?")=='y') {
				    unrestrict_weapon_skill(P_CROSSBOW);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_DART) && yn("Do you want to learn the dart skill?")=='y') {
				    unrestrict_weapon_skill(P_DART);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_SHURIKEN) && yn("Do you want to learn the shuriken skill?")=='y') {
				    unrestrict_weapon_skill(P_SHURIKEN);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_BOOMERANG) && yn("Do you want to learn the boomerang skill?")=='y') {
				    unrestrict_weapon_skill(P_BOOMERANG);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_WHIP) && yn("Do you want to learn the whip skill?")=='y') {
				    unrestrict_weapon_skill(P_WHIP);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_UNICORN_HORN) && yn("Do you want to learn the unicorn horn skill?")=='y') {
				    unrestrict_weapon_skill(P_UNICORN_HORN);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_LIGHTSABER) && yn("Do you want to learn the lightsaber skill?")=='y') {
				    unrestrict_weapon_skill(P_LIGHTSABER);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_ATTACK_SPELL) && yn("Do you want to learn the attack spell skill?")=='y') {
				    unrestrict_weapon_skill(P_ATTACK_SPELL);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_HEALING_SPELL) && yn("Do you want to learn the healing spell skill?")=='y') {
				    unrestrict_weapon_skill(P_HEALING_SPELL);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_DIVINATION_SPELL) && yn("Do you want to learn the divination spell skill?")=='y') {
				    unrestrict_weapon_skill(P_DIVINATION_SPELL);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_ENCHANTMENT_SPELL) && yn("Do you want to learn the enchantment spell skill?")=='y') {
				    unrestrict_weapon_skill(P_ENCHANTMENT_SPELL);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_PROTECTION_SPELL) && yn("Do you want to learn the protection spell skill?")=='y') {
				    unrestrict_weapon_skill(P_PROTECTION_SPELL);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_BODY_SPELL) && yn("Do you want to learn the body spell skill?")=='y') {
				    unrestrict_weapon_skill(P_BODY_SPELL);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_OCCULT_SPELL) && yn("Do you want to learn the occult spell skill?")=='y') {
				    unrestrict_weapon_skill(P_OCCULT_SPELL);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_ELEMENTAL_SPELL) && yn("Do you want to learn the elemental spell skill?")=='y') {
				    unrestrict_weapon_skill(P_ELEMENTAL_SPELL);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_CHAOS_SPELL) && yn("Do you want to learn the chaos spell skill?")=='y') {
				    unrestrict_weapon_skill(P_CHAOS_SPELL);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_MATTER_SPELL) && yn("Do you want to learn the matter spell skill?")=='y') {
				    unrestrict_weapon_skill(P_MATTER_SPELL);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_RIDING) && yn("Do you want to learn the riding skill?")=='y') {
				    unrestrict_weapon_skill(P_RIDING);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_HIGH_HEELS) && yn("Do you want to learn the high heels skill?")=='y') {
				    unrestrict_weapon_skill(P_HIGH_HEELS);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_GENERAL_COMBAT) && yn("Do you want to learn the general combat skill?")=='y') {
				    unrestrict_weapon_skill(P_GENERAL_COMBAT);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_SHIELD) && yn("Do you want to learn the shield skill?")=='y') {
				    unrestrict_weapon_skill(P_SHIELD);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_BODY_ARMOR) && yn("Do you want to learn the body armor skill?")=='y') {
				    unrestrict_weapon_skill(P_BODY_ARMOR);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_TWO_HANDED_WEAPON) && yn("Do you want to learn the two-handed weapon skill?")=='y') {
				    unrestrict_weapon_skill(P_TWO_HANDED_WEAPON);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_TWO_WEAPON_COMBAT) && yn("Do you want to learn the two-weapon combat skill?")=='y') {
				    unrestrict_weapon_skill(P_TWO_WEAPON_COMBAT);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_POLYMORPHING) && yn("Do you want to learn the polymorphing skill?")=='y') {
				    unrestrict_weapon_skill(P_POLYMORPHING);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_DEVICES) && yn("Do you want to learn the devices skill?")=='y') {
				    unrestrict_weapon_skill(P_DEVICES);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_SEARCHING) && yn("Do you want to learn the searching skill?")=='y') {
				    unrestrict_weapon_skill(P_SEARCHING);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_SPIRITUALITY) && yn("Do you want to learn the spirituality skill?")=='y') {
				    unrestrict_weapon_skill(P_SPIRITUALITY);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_PETKEEPING) && yn("Do you want to learn the petkeeping skill?")=='y') {
				    unrestrict_weapon_skill(P_PETKEEPING);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_MISSILE_WEAPONS) && yn("Do you want to learn the missile weapons skill?")=='y') {
				    unrestrict_weapon_skill(P_MISSILE_WEAPONS);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_TECHNIQUES) && yn("Do you want to learn the techniques skill?")=='y') {
				    unrestrict_weapon_skill(P_TECHNIQUES);	acquiredskill = 1; }
			else if (P_RESTRICTED(P_IMPLANTS) && yn("Do you want to learn the implants skill?")=='y') {
				    unrestrict_weapon_skill(P_IMPLANTS);	acquiredskill = 1; }
			else if (yn("Do you want to learn no new skill at all?")=='y') {
				    acquiredskill = 1; }
			}
			pline("Check out what you got!");

			break;
		    case 15:
			pline("A voice echoes:");
			verbalize("Thou be cursed!");
			attrcurse();
			break;
		    case 16:
			pline("A voice echoes:");
			verbalize("Thou shall be punished!");
			punishx();
			break;
		    case 17:
			You_feel("like someone has touched your forehead...");

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
			} else pluslvl(FALSE);

			pluslvl(FALSE);

			break;
		    case 18:
			{register int cnt = rnd(10);
			struct permonst *randmonstforspawn = rndmonst();

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			while(cnt--)
			    (void) makemon(randmonstforspawn, u.ux, u.uy, NO_MM_FLAGS);

			u.aggravation = 0;

			pline("A voice echoes:");
			verbalize("Leave me alone, stupid %s", randmonstforspawn->mname);
			break;
			}
		    case 19:
			{register int cnt = rnd(10);
			int randmonstforspawn = rnd(68);
			if (randmonstforspawn == 35) randmonstforspawn = 53;

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			while(cnt--)
			    (void) makemon(mkclass(randmonstforspawn,0), u.ux, u.uy, NO_MM_FLAGS);

			u.aggravation = 0;

			pline("A voice echoes:");
			verbalize("Oh, please help me! A horrible %s stole my sword! I'm nothing without it.", monexplain[randmonstforspawn]);
			break;
			}
		    case 20:
			{
			pline("You may fully identify an object!");
			register struct obj *idobj;

			idobj = getobj(all_count, "secure identify");

			if (!idobj) {
				pline("A feeling of loss comes over you.");
				break;
			}
			if (idobj) {
				makeknown(idobj->otyp);
				if (idobj->oartifact) discover_artifact((int)idobj->oartifact);
				idobj->known = idobj->dknown = idobj->bknown = idobj->rknown = 1;
				if (idobj->otyp == EGG && idobj->corpsenm != NON_PM)
				learn_egg_type(idobj->corpsenm);
				prinv((char *)0, idobj, 0L);
			}
			}
			break;

		    default:	impossible("throne effect");
				break;
		}
	    } else {
		if (is_prince(youmonst.data))
		    You_feel("very comfortable here.");
		else
		    You_feel("somehow out of place...");
	    }

		if (!rn2(10)) badeffect();

		break;

	case SPE_REDEMPTION:

		{
			register struct obj *redeemobj;
			for (redeemobj = fobj; redeemobj; redeemobj = redeemobj->nobj) {
				if (redeemobj->otyp == CORPSE) {
					if (redeemobj->timed) {
					    (void) stop_timer(MOLDY_CORPSE, (void *)redeemobj);
					    (void) stop_timer(REVIVE_MON, (void *)redeemobj);
					}
					if (!(redeemobj->timed)) {
						(void) start_timer(250, TIMER_OBJECT, ROT_CORPSE, (void *)redeemobj);
						pline("A corpse has been redeemed!");
					}
				}
			}
		}


		break;

	case SPE_MAGIC_SHIELD:

		u.magicshield += (u.magicshield ? rnd(10) : 40 + (spell_damage_bonus(spellid(spell)) * 10) );
		pline("You activate your magic shield!");

		break;

	case SPE_BERSERK:

		if (u.berserktime) {
			pline("Bad idea - you are already berserk! Things turn into an unrecognizable blur.");
			make_hallucinated(HHallucination + rnd(50) + HStun + HConfusion, FALSE, 0L);
			set_itimeout(&HeavyHallu, HHallucination);
			break;
		}

		u.berserktime = 7 + spell_damage_bonus(spellid(spell));
		pline("Raaaaaargh! You fly into a berserk rage!");

		break;

	case SPE_RAIN_CLOUD:
		{
			int i, j;
			struct monst *rainedmon;
		    for (i = -1; i <= 1; i++) for(j = -1; j <= 1; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if (levl[u.ux + i][u.uy + j].typ == LAVAPOOL) {
				levl[u.ux + i][u.uy + j].typ = WATER;
				pline("The lava turns into water!");
			}
			if ( (rainedmon = m_at(u.ux + i, u.uy + j)) != 0) {
				if (dmgtype(rainedmon->data, AD_FIRE) || dmgtype(rainedmon->data, AD_BURN) || dmgtype(rainedmon->data, AD_LAVA)) {

					rainedmon->mhp -= rnd(20 + (spell_damage_bonus(spellid(spell)) * 3) );
					pline("%s is damaged by the raindrops!", Monnam(rainedmon));
					if (rainedmon->mhp <= 0) {
						pline("%s is killed!", Monnam(rainedmon));
						xkilled(rainedmon, 0);
					}

				}
			}
		    }
		}

		u.uprops[DEAC_FAST].intrinsic += (( rnd(10) + rnd(monster_difficulty() + 1) ) * 3);
		pline(u.inertia ? "You slow down even more due to the rain." : "You slow down greatly due to the rain.");
		u.inertia += (rnd(10) + rnd(monster_difficulty() + 1));

		if (!(InterfaceScrewed || u.uprops[INTERFACE_SCREW].extrinsic || have_interfacescrewstone())) (void)doredraw();

		break;

	case SPE_DRY_UP_FOUNTAIN:
		{
			int i, j;
		    for (i = -1; i <= 1; i++) for(j = -1; j <= 1; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if (levl[u.ux + i][u.uy + j].typ == FOUNTAIN) {
				levl[u.ux + i][u.uy + j].typ = CORR;
				levl[u.ux + i][u.uy + j].looted = 0;
				levl[u.ux + i][u.uy + j].blessedftn = 0;
				pline("The fountain dries up!");
				level.flags.nfountains--;
			}
		    }
		}

		if (!(InterfaceScrewed || u.uprops[INTERFACE_SCREW].extrinsic || have_interfacescrewstone())) (void)doredraw();

		break;

	case SPE_TAKE_SELFIE:

		pline("You take a selfie.");
		nomul(-(2 + rn2(4)), "taking a selfie", TRUE);

		if (!rn2(5)) {
			make_blinded(Blinded + rnd(50), FALSE);
			pline("You are blinded by the flash!");
		}

		break;

	case SPE_VAPORIZE:
		{
			int i, j;
			struct monst *rainedmon;

			if (Is_waterlevel(&u.uz)) {
				pline("The water foams violently for a moment.");
				break;
			}

		    for (i = -1; i <= 1; i++) for(j = -1; j <= 1; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if (levl[u.ux + i][u.uy + j].typ == WATER || levl[u.ux + i][u.uy + j].typ == POOL || levl[u.ux + i][u.uy + j].typ == MOAT) {
				levl[u.ux + i][u.uy + j].typ = ROOM;
				pline("The water evaporizes!");
				if ( (rainedmon = m_at(u.ux + i, u.uy + j)) != 0) {
					rainedmon->mhp /= 2;
					pline("%s is damaged by the vapors!", Monnam(rainedmon));
					if (!rn2(3)) {
						rainedmon->mcanmove = 0;
						rainedmon->mfrozen = rnd(20);
						rainedmon->mstrategy &= ~STRAT_WAITFORU;
						pline("%s is paralyzed!", Monnam(rainedmon));
					}
					if (rainedmon->mhp <= 0) {
						pline("%s is killed!", Monnam(rainedmon));
						xkilled(rainedmon, 0);
					}
				}
			}
		    }
		}

		pline("The vapors burn you!");
		make_burned(HBurned + rnd(200), FALSE);
		set_itimeout(&HeavyBurned, HBurned);

		if (!(InterfaceScrewed || u.uprops[INTERFACE_SCREW].extrinsic || have_interfacescrewstone())) (void)doredraw();

		break;

	case SPE_NEXUS_FIELD:
		{
			register struct monst *nexusmon, *nextmon;

			for(nexusmon = fmon; nexusmon; nexusmon = nextmon) {
			    nextmon = nexusmon->nmon; /* trap might kill mon */
			    if (DEADMONSTER(nexusmon)) continue;
			    if (resist(nexusmon, SPBOOK_CLASS, 0, NOTELL)) continue;

			    if (!monnear(nexusmon, u.ux, u.uy)) continue;
				if (nexusmon->mtrapped) {
				    /* no longer in previous trap (affects mintrap) */
				    nexusmon->mtrapped = 0;
				    fill_pit(nexusmon->mx, nexusmon->my);
				}
				pline("%s is beamed away!", Monnam(nexusmon));
				rloc(nexusmon, FALSE);
			}
		}

		if (!rn2(10)) {
			register int statloss = rn2(A_MAX);

			ABASE(statloss) -= rnd(2);
			if (ABASE(statloss) < ATTRMIN(statloss)) ABASE(statloss) = ATTRMIN(statloss);
			AMAX(statloss) = ABASE(statloss);
			pline("You are hit by nexus forces!");
			poisontell(statloss);

		}

		break;

	case SPE_TUNNELIZATION:
		{
			register struct monst *nexusmon, *nextmon;

			for(nexusmon = fmon; nexusmon; nexusmon = nextmon) {
			    nextmon = nexusmon->nmon; /* trap might kill mon */
			    if (DEADMONSTER(nexusmon)) continue;
				if (levl[nexusmon->mx][nexusmon->my].typ >= STONE && levl[nexusmon->mx][nexusmon->my].typ <= DBWALL) {
					if ((levl[nexusmon->mx][nexusmon->my].wall_info & W_NONDIGGABLE) == 0 && !(*in_rooms(nexusmon->mx,nexusmon->my,SHOPBASE))) {
						levl[nexusmon->mx][nexusmon->my].typ = CORR;
						unblock_point(nexusmon->mx,nexusmon->my);
						if (!(levl[nexusmon->mx][nexusmon->my].wall_info & W_HARDGROWTH)) levl[nexusmon->mx][nexusmon->my].wall_info |= W_EASYGROWTH;
						newsym(nexusmon->mx,nexusmon->my);

					}
					pline("%s wails out in pain!", Monnam(nexusmon));
					nexusmon->mhp -= rnz(nexusmon->mhp / 2);
					if (nexusmon->mhp <= 0) {
						xkilled(nexusmon, 0);
						pline("%s is killed!", Monnam(nexusmon));
					}
					adjalign(-10);
				}
			}
		}

		pline("You feel bad for tunneling, and are also blinded by heaps of earth flying around.");
		adjalign(-2);
		if (rn2(5)) u.tunnelized += rnd(20);
		else u.tunnelized += rnd(100);
		make_blinded(Blinded + u.tunnelized, FALSE);
		set_itimeout(&HeavyBlind, Blinded);
		if (!(InterfaceScrewed || u.uprops[INTERFACE_SCREW].extrinsic || have_interfacescrewstone())) (void)doredraw();

		break;

	case SPE_APPLY_NAIL_POLISH:

		if (u.nailpolish >= 10) {
			pline("All of your nails are polished already! Nothing happens.");
			break;
		}
		pline("You begin applying nail polish.");
		u.nailpolish++;
		nomul(-(rnd(4)), "applying nail polish", TRUE);
		nomovemsg = "You finish polishing your nails.";

		break;

	case SPE_ENCHANT:

		if (!uwep) {
			pline("You are not holding a weapon!");
			break;
		}

		/* enchanting again while it's already active sets a new timeout and does not add to the old one --Amy */
		u.enchantspell = (5 + spell_damage_bonus(spellid(spell)));
		pline("You enchant your weapon.");

		break;

	case SPE_FROST:
		{
			register struct monst *frostmon, *nxtmon;

			for(frostmon = fmon; frostmon; frostmon = nxtmon) {
			    nxtmon = frostmon->nmon; /* trap might kill mon */
			    if (DEADMONSTER(frostmon)) continue;
			    if (resist(frostmon, SPBOOK_CLASS, 0, NOTELL)) continue;

			    if (!monnear(frostmon, u.ux, u.uy)) continue;
				mon_adjust_speed(frostmon, -1, (struct obj *)0);
				m_dowear(frostmon, FALSE); /* might want speed boots */

			}
		}

		break;

	case SPE_THUNDER_WAVE:
		{
			register struct monst *frostmon, *nxtmon;

			for(frostmon = fmon; frostmon; frostmon = nxtmon) {
			    nxtmon = frostmon->nmon; /* trap might kill mon */
			    if (DEADMONSTER(frostmon)) continue;
			    if (!monnear(frostmon, u.ux, u.uy)) continue;

			    if (!resists_elec(frostmon) && !(dmgtype(frostmon->data, AD_PLYS)) && !resist(frostmon, SPBOOK_CLASS, 0, NOTELL) && !rn2(10)) {

				if (canseemon(frostmon) ) {
					pline("%s is paralyzed! It might be unable to move!", Monnam(frostmon) );
				}
				frostmon->mcanmove = 0;
				frostmon->mfrozen = rnd(5);
				frostmon->mstrategy &= ~STRAT_WAITFORU;

			    }

			    if (!resists_elec(frostmon)) {

				frostmon->mhp -= rnd(10 + (spell_damage_bonus(spellid(spell)) * 2) );
				pline("%s is shocked!", Monnam(frostmon));
				if (frostmon->mhp <= 0) {
					pline("%s is killed!", Monnam(frostmon));
					xkilled(frostmon, 0);
				}

			    }

			}
		}

		break;

	case SPE_POWER_FAILURE:
		{

		    int i, j, bd = 2;
		    struct monst *mtmp;

		    for(i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if ((mtmp = m_at(u.ux + i, u.uy + j)) != 0) {
				if (dmgtype(mtmp->data, AD_ELEC) || dmgtype(mtmp->data, AD_MALK)) {
					mtmp->mhp -= rnd(40 + (spell_damage_bonus(spellid(spell)) * 10) );
					pline("%s's power is down!", Monnam(mtmp));
					if (rn2(3) && !mtmp->mstun) {
						mtmp->mstun = TRUE;
						pline("%s is numbed!", Monnam(mtmp));
					}
					if (rn2(3) && !mtmp->mblinded) {
						mtmp->mblinded = rnd(10);
						pline("%s is blinded!", Monnam(mtmp));
					}
					if (mtmp->mhp <= 0) {
						pline("%s is killed!", Monnam(mtmp));
						xkilled(mtmp, 0);
					}
				}
			}

		    }

		    for (i = -2; i <= 2; i++) for(j = -2; j <= 2; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if (levl[u.ux + i][u.uy + j].typ == IRONBARS && !rn2(1000) ) {
				levl[u.ux + i][u.uy + j].typ = CLOUD;
				pline("The iron bars break apart and leave behind a cloud!");
			}
		    }

		}

		u.powerfailure += rnd(100);
		if (!(InterfaceScrewed || u.uprops[INTERFACE_SCREW].extrinsic || have_interfacescrewstone())) (void)doredraw();

		break;

	case SPE_ANGER_PEACEFUL_MONSTER:
		{
			register struct monst *frostmon, *nxtmon;

			for(frostmon = fmon; frostmon; frostmon = nxtmon) {
			    nxtmon = frostmon->nmon; /* trap might kill mon */
			    if (DEADMONSTER(frostmon)) continue;
			    if (!monnear(frostmon, u.ux, u.uy)) continue;

			    wakeup(frostmon);
			}
		}

		break;

	case SPE_CURE_MONSTER:
		{
			register struct monst *frostmon, *nxtmon;

			for(frostmon = fmon; frostmon; frostmon = nxtmon) {
			    nxtmon = frostmon->nmon; /* trap might kill mon */
			    if (DEADMONSTER(frostmon)) continue;
			    if (!monnear(frostmon, u.ux, u.uy)) continue;
				frostmon->mfrozen = 0;
				frostmon->msleeping = 0;
				frostmon->mcanmove = 1;
				frostmon->mflee = 0;
				frostmon->mcansee = 1;
				frostmon->mblinded = 0;
				frostmon->mstun = 0;
				frostmon->mconf = 0;
				pline("%s is cured.", Monnam(frostmon));
			}
		}

		break;

	case SPE_THORNS:

		pline("You throw up a thorny skin!");
		u.thornspell = 10 + (spell_damage_bonus(spellid(spell)) * 2);
		/* casting it repeatedly will not give you a longer duration --Amy */

		break;

	case SPE_MANA_BATTERY:
		{

		    register struct obj *mbobj, *mbobj2;
		    for (mbobj = invent; mbobj; mbobj = mbobj2) {
		      mbobj2 = mbobj->nobj;

			if (mbobj->oclass == WAND_CLASS) {
				if (mbobj->spe > 1) u.uen += (mbobj->spe * 10);
				if (u.uen > u.uenmax) u.uen = u.uenmax;
				delobj(mbobj);
			}
		    }
		}

		pline("Your wands are consumed to restore your mana.");

		break;

	case SPE_UNTAME_MONSTER:
		{
			register struct monst *frostmon, *nxtmon;

			for(frostmon = fmon; frostmon; frostmon = nxtmon) {
			    nxtmon = frostmon->nmon; /* trap might kill mon */
			    if (DEADMONSTER(frostmon)) continue;
			    if (!monnear(frostmon, u.ux, u.uy)) continue;

				if (frostmon->mtame) frostmon->mtame = 0;
			}
		}

		break;

	case SPE_CODE_EDITING:

		{
			register struct obj *codeobj, *nextobj;
			for (codeobj = fobj; codeobj; codeobj = nextobj) {
				nextobj = codeobj->nobj;
				if (codeobj->otyp != CORPSE) continue;
				if (codeobj->where != OBJ_FLOOR) continue;
				if (obj_resists(codeobj, 0, 50)) continue; /* no easy removal of rider corpses! --Amy */
				explode(codeobj->ox, codeobj->oy, ZT_SPELL(ZT_FIRE), mons[codeobj->corpsenm].mlevel * rnz(5), SCROLL_CLASS, EXPL_FIERY);
				pline("Boom!");
				if (codeobj) delobj(codeobj);
			}
		}

		break;

	case SPE_ELEMENTAL_MINION:
		{

			pline("You sacrifice some of your %s to create an elemental being.", body_part(BLOOD));
			u.uhpmax -= rnd(5);
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			if (u.uhp < 1) {
				u.youaredead = 1;
				killer = "summoning an elemental with too little health";
				killer_format = KILLED_BY;
				done(DIED);
				u.youaredead = 0;
			}
			if (Upolyd) {
				u.mhmax -= rnd(5);
				if (u.mh > u.mhmax) u.mh = u.mhmax;
			}

			register struct monst *elemental;
			elemental = makemon(mkclass(S_ELEMENTAL,0), u.ux, u.uy, MM_NOSPECIALS);
			if (elemental) {
			    if (!resist(elemental, SPBOOK_CLASS, 0, NOTELL)) {
				elemental = tamedog(elemental, (struct obj *) 0, FALSE);
				if (elemental) You("dominate %s!", mon_nam(elemental));
			    } else if (!resist(elemental, SPBOOK_CLASS, 0, NOTELL)) {
				elemental = tamedog(elemental, (struct obj *) 0, FALSE);
				if (elemental) You("dominate %s!", mon_nam(elemental));
			    } else if (!resist(elemental, SPBOOK_CLASS, 0, TELL)) {
				elemental = tamedog(elemental, (struct obj *) 0, FALSE);
				if (elemental) You("dominate %s!", mon_nam(elemental));
			    }

			}
		}

		break;

	case SPE_HYPERSPACE_SUMMON:
		{

			pline("You sacrifice some of your %s and magical energy to create vortices.", body_part(BLOOD));
			u.uhpmax -= rno(8);
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			if (u.uhp < 1) {
				u.youaredead = 1;
				killer = "summoning vortices with too little health";
				killer_format = KILLED_BY;
				done(DIED);
				u.youaredead = 0;
			}
			if (Upolyd) {
				u.mhmax -= rno(8);
				if (u.mh > u.mhmax) u.mh = u.mhmax;
			}
			if (!rn2(2)) {
				u.uenmax -= rno(8);
				if (u.uenmax < 0) u.uenmax = 0;
				if (u.uen > u.uenmax) u.uen = u.uenmax;
			}

			int ammount = 1;
			if (spell_damage_bonus(spellid(spell)) > 0) ammount += spell_damage_bonus(spellid(spell));

			register struct monst *vortex;
			while (ammount > 0) {
				vortex = makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_NOSPECIALS);
				if (vortex) {
				    if (!resist(vortex, SPBOOK_CLASS, 0, NOTELL)) {
					vortex = tamedog(vortex, (struct obj *) 0, FALSE);
					if (vortex) You("dominate %s!", mon_nam(vortex));
				    } else if (!resist(vortex, SPBOOK_CLASS, 0, NOTELL)) {
					vortex = tamedog(vortex, (struct obj *) 0, FALSE);
					if (vortex) You("dominate %s!", mon_nam(vortex));
				    } else if (!resist(vortex, SPBOOK_CLASS, 0, NOTELL)) {
					vortex = tamedog(vortex, (struct obj *) 0, FALSE);
					if (vortex) You("dominate %s!", mon_nam(vortex));
				    } else if (!resist(vortex, SPBOOK_CLASS, 0, NOTELL)) {
					vortex = tamedog(vortex, (struct obj *) 0, FALSE);
					if (vortex) You("dominate %s!", mon_nam(vortex));
				    } else if (!resist(vortex, SPBOOK_CLASS, 0, TELL)) {
					vortex = tamedog(vortex, (struct obj *) 0, FALSE);
					if (vortex) You("dominate %s!", mon_nam(vortex));
				    }
	
				}
				ammount--;
			}
		}

		break;

	case SPE_KNOW_ENCHANTMENT:
		{

		    pline("You detect the enchantment status of your inventory!");

		    register struct obj *obj, *obj2;
		    for (obj = invent; obj; obj = obj2) {
		      obj2 = obj->nobj;

			obj->known = TRUE;

		    }
		}

		break;

	case SPE_DISSOLVE_FOOD:
		{

		    pline("Your food dissolves!");

		    register struct obj *obj, *obj2;
		    for (obj = invent; obj; obj = obj2) {
		      obj2 = obj->nobj;

			if (obj->oclass == FOOD_CLASS) useupall(obj);

		    }
		}

		break;

	case SPE_MIMICRY:
		pline("You start to disguise.");

		youmonst.m_ap_type = M_AP_OBJECT;
		youmonst.mappearance = Hallucination ? ORANGE : GOLD_PIECE;
		newsym(u.ux,u.uy);

		break;

	case SPE_WHISPERS_FROM_BEYOND:

		identify_pack(0, 0);
		identify_pack(0, 0);
		if (rn2(2)) identify_pack(0, 0);
		if (!rn2(5)) identify_pack(0, 0);

		ABASE(A_INT) -= rnd(2);
		if (ABASE(A_INT) < ATTRMIN(A_INT)) {
			u.youaredead = 1;
			Your("last thought fades away.");
			killer = "brainlessness";
			killer_format = KILLED_BY;
			done(DIED);

			/* player still alive somehow? kill them again :P */

			pline("Unfortunately your brain is still gone.");
			killer = "brainlessness";
			killer_format = KILLED_BY;
			done(DIED);
			u.youaredead = 0;

			/* if you're still alive then you're either in wizard mode, or you deserve to be able to go on playing. */

			ABASE(A_INT) = ATTRMIN(A_INT);
			You_feel("like a scarecrow.");
		}
		AMAX(A_INT) = ABASE(A_INT);

		ABASE(A_WIS) -= rnd(2);
		if (ABASE(A_WIS) < ATTRMIN(A_WIS)) {

			u.youaredead = 1;
			You("turn into an unthinkable vegetable and die.");
			killer = "being turned into a vegetable";
			killer_format = KILLED_BY;
			done(DIED);
			u.youaredead = 0;

			/* player still alive somehow? well then, you can go on playing */

			ABASE(A_WIS) = ATTRMIN(A_WIS);
		}
		AMAX(A_WIS) = ABASE(A_WIS);

		/* It is not a mistake that INT loss kills you twice while WIS loss only does so once. --Amy */

		break;

	case SPE_STASIS:
		u.stasistime = rnd(100);
		pline("You freeze completely.");
		nomul(-(u.stasistime), "frozen in stasis", FALSE);

		break;

	case SPE_CRYOGENICS:

		cryogenics();

		break;

	case SPE_TERROR:

		if (!rn2(20)) {

			pline("The spell effect backlashes!");

		    switch (rn2(17)) {
		    case 0:
		    case 1:
		    case 2:
		    case 3: make_confused(HConfusion + 12, FALSE);			/* 40% */
			    break;
		    case 4:
		    case 5:
		    case 6: make_confused(HConfusion + (2L * 12 / 3L), FALSE);		/* 30% */
			    make_stunned(HStun + (12 / 3L), FALSE);
			    break;
		    case 7:
		    case 8: make_stunned(HStun + (2L * 12 / 3L), FALSE);		/* 20% */
			    make_confused(HConfusion + (12 / 3L), FALSE);
			    break;
		    case 9: make_stunned(HStun + 12, FALSE);			/* 10% */
			    break;
		    case 10: make_numbed(HNumbed + 12, FALSE);			/* 10% */
			    break;
		    case 11: make_frozen(HFrozen + 12, FALSE);			/* 10% */
			    break;
		    case 12: make_burned(HBurned + 12, FALSE);			/* 10% */
			    break;
		    case 13: make_feared(HFeared + 12, FALSE);			/* 10% */
			    break;
		    case 14: make_blinded(Blinded + 12, FALSE);			/* 10% */
			    break;
		    case 15: make_hallucinated(HHallucination + 12, FALSE, 0L);			/* 10% */
			    break;
		    case 16: make_dimmed(HDimmed + 12, FALSE);			/* 10% */
			    break;
		    }

		}

	    {	
		register struct monst *fleemon;

		for(fleemon = fmon; fleemon; fleemon = fleemon->nmon) {
		    if (DEADMONSTER(fleemon)) continue;
			if (!resist(fleemon, SPBOOK_CLASS, 0, NOTELL))
				monflee(fleemon, rnd(50), FALSE, FALSE);
		}
		You_hear("horrified screaming close by.");
	    }

		break;

	case SPE_PHASE_DOOR:
		phase_door(0);
		if (!rn2(20)) {
			pline("The spell backlashes!");
			badeffect();
		}

		break;

	case SPE_BURROW:
		u.burrowed = 100;
		u.utrap = 100;
		u.utraptype = TT_INFLOOR;
		pline("You are burrowed into the floor, and gain armor class.");

		break;

	case SPE_SWITCHEROO:

		pline("Click! The red status light goes out while the green light starts shining brightly!");

		RMBLoss = 0L;
		DisplayLoss = 0L;
		SpellLoss = 0L;
		YellowSpells = 0L;
		AutoDestruct = 0L;
		MemoryLoss = 0L;
		InventoryLoss = 0L;
		BlackNgWalls = 0L;
		MenuBug = 0L;
		SpeedBug = 0L;
		Superscroller = 0L;
		FreeHandLoss = 0L;
		Unidentify = 0L;
		Thirst = 0L;
		LuckLoss = 0L;
		ShadesOfGrey = 0L;
		FaintActive = 0L;
		Itemcursing = 0L;
		DifficultyIncreased = 0L;
		Deafness = 0L;
		CasterProblem = 0L;
		WeaknessProblem = 0L;
		RotThirteen = 0L;
		BishopGridbug = 0L;
		ConfusionProblem = 0L;
		NoDropProblem = 0L;
		DSTWProblem = 0L;
		StatusTrapProblem = 0L;
		AlignmentProblem = 0L;
		StairsProblem = 0L;
		UninformationProblem = 0L;
		IntrinsicLossProblem = 0L;
		BloodLossProblem = 0L;
		BadEffectProblem = 0L;
		TrapCreationProblem = 0L;
		AutomaticVulnerabilitiy = 0L;
		TeleportingItems = 0L;
		/*NastinessProblem = 0L;*/ /* commented out on purpose --Amy */
		CaptchaProblem = 0L;
		FarlookProblem = 0L;
		RespawnProblem = 0L;
		RecurringAmnesia = 0L;
		BigscriptEffect = 0L;
		BankTrapEffect = 0L;
		MapTrapEffect = 0L;
		TechTrapEffect = 0L;
		RecurringDisenchant = 0L;
		verisiertEffect = 0L;
		ChaosTerrain = 0L;
		Muteness = 0L;
		EngravingDoesntWork = 0L;
		MagicDeviceEffect = 0L;
		BookTrapEffect = 0L;
		LevelTrapEffect = 0L;
		QuizTrapEffect = 0L;

		LootcutBug = 0L;
		MonsterSpeedBug = 0L;
		ScalingBug = 0L;
		EnmityBug = 0L;
		WhiteSpells = 0L;
		CompleteGraySpells = 0L;
		QuasarVision = 0L;
		MommaBugEffect = 0L;
		HorrorBugEffect = 0L;
		ArtificerBug = 0L;
		WereformBug = 0L;
		NonprayerBug = 0L;
		EvilPatchEffect = 0L;
		HardModeEffect = 0L;
		SecretAttackBug = 0L;
		EaterBugEffect = 0L;
		CovetousnessBug = 0L;
		NotSeenBug = 0L;
		DarkModeBug = 0L;
		AntisearchEffect = 0L;
		HomicideEffect = 0L;
		NastynationBug = 0L;
		WakeupCallBug = 0L;
		GrayoutBug = 0L;
		GrayCenterBug = 0L;
		CheckerboardBug = 0L;
		ClockwiseSpinBug = 0L;
		CounterclockwiseSpin = 0L;
		LagBugEffect = 0L;
		BlesscurseEffect = 0L;
		DeLightBug = 0L;
		DischargeBug = 0L;
		TrashingBugEffect = 0L;
		FilteringBug = 0L;
		DeformattingBug = 0L;
		FlickerStripBug = 0L;
		UndressingEffect = 0L;
		Hyperbluewalls = 0L;
		NoliteBug = 0L;
		ParanoiaBugEffect = 0L;
		FleecescriptBug = 0L;
		InterruptEffect = 0L;
		DustbinBug = 0L;
		ManaBatteryBug = 0L;
		Monsterfingers = 0L;
		MiscastBug = 0L;
		MessageSuppression = 0L;
		StuckAnnouncement = 0L;
		BloodthirstyEffect = 0L;
		MaximumDamageBug = 0L;
		LatencyBugEffect = 0L;
		StarlitBug = 0L;
		KnowledgeBug = 0L;
		HighscoreBug = 0L;
		PinkSpells = 0L;
		GreenSpells = 0L;
		EvencoreEffect = 0L;
		UnderlayerBug = 0L;
		DamageMeterBug = 0L;
		ArbitraryWeightBug = 0L;
		FuckedInfoBug = 0L;
		BlackSpells = 0L;
		CyanSpells = 0L;
		HeapEffectBug = 0L;
		BlueSpells = 0L;
		TronEffect = 0L;
		RedSpells = 0L;
		TooHeavyEffect = 0L;
		ElongationBug = 0L;
		WrapoverEffect = 0L;
		DestructionEffect = 0L;
		MeleePrefixBug = 0L;
		AutomoreBug = 0L;
		UnfairAttackBug = 0L;

		FastMetabolismEffect = 0L;
		NoReturnEffect = 0L;
		AlwaysEgotypeMonsters = 0L;
		TimeGoesByFaster = 0L;
		FoodIsAlwaysRotten = 0L;
		AllSkillsUnskilled = 0L;
		AllStatsAreLower = 0L;
		PlayerCannotTrainSkills = 0L;
		PlayerCannotExerciseStats = 0L;

		TurnLimitation = 0L;
		WeakSight = 0L;
		RandomMessages = 0L;

		Desecration = 0L;
		StarvationEffect = 0L;
		NoDropsEffect = 0L;
		LowEffects = 0L;
		InvisibleTrapsEffect = 0L;
		GhostWorld = 0L;
		Dehydration = 0L;
		HateTrapEffect = 0L;
		TotterTrapEffect = 0L;
		Nonintrinsics = 0L;
		Dropcurses = 0L;
		Nakedness = 0L;
		Antileveling = 0L;
		ItemStealingEffect = 0L;
		Rebellions = 0L;
		CrapEffect = 0L;
		ProjectilesMisfire = 0L;
		WallTrapping = 0L;
		DisconnectedStairs = 0L;
		InterfaceScrewed = 0L;
		Bossfights = 0L;
		EntireLevelMode = 0L;
		BonesLevelChange = 0L;
		AutocursingEquipment = 0L;
		HighlevelStatus = 0L;
		SpellForgetting = 0L;
		SoundEffectBug = 0L;
		TimerunBug = 0L;

		pline("But then the green light goes out again and the red one lights up...");

		NastinessProblem += 10000;
		badeffect();
		badeffect();
		badeffect();
		badeffect();
		badeffect();
		badeffect();
		badeffect();

		break;

	case SPE_GAIN_CORRUPTION:

		pline("Okay, if that's really what you want... you feel corrupted.");
		if (Hallucination) pline("At least this isn't ADOM, where having too many corruptions would instakill you!");

		switch (rnd(169)) {

			case 1: 
			    SpeedBug |= FROMOUTSIDE; break;
			case 2: 
			    MenuBug |= FROMOUTSIDE; break;
			case 3: 
			    RMBLoss |= FROMOUTSIDE; break;
			case 4: 
			    DisplayLoss |= FROMOUTSIDE; break;
			case 5: 
			    SpellLoss |= FROMOUTSIDE; break;
			case 6: 
			    YellowSpells |= FROMOUTSIDE; break;
			case 7: 
			    AutoDestruct |= FROMOUTSIDE; break;
			case 8: 
			    MemoryLoss |= FROMOUTSIDE; break;
			case 9: 
			    DisplayLoss |= FROMOUTSIDE; break;
			case 10: 
			    BlackNgWalls |= FROMOUTSIDE; break;
			case 11: 
			    Superscroller |= FROMOUTSIDE; break;
			case 12: 
			    FreeHandLoss |= FROMOUTSIDE; break;
			case 13: 
			    Unidentify |= FROMOUTSIDE; break;
			case 14: 
			    Thirst |= FROMOUTSIDE; break;
			case 15: 
			    LuckLoss |= FROMOUTSIDE; break;
			case 16: 
			    ShadesOfGrey |= FROMOUTSIDE; break;
			case 17: 
			    FaintActive |= FROMOUTSIDE; break;
			case 18: 
			    Itemcursing |= FROMOUTSIDE; break;
			case 19: 
			    DifficultyIncreased |= FROMOUTSIDE; break;
			case 20: 
			    Deafness |= FROMOUTSIDE; break;
			case 21: 
			    CasterProblem |= FROMOUTSIDE; break;
			case 22: 
			    WeaknessProblem |= FROMOUTSIDE; break;
			case 23: 
			    RotThirteen |= FROMOUTSIDE; break;
			case 24: 
			    BishopGridbug |= FROMOUTSIDE; break;
			case 25: 
			    ConfusionProblem |= FROMOUTSIDE; break;
			case 26: 
			    NoDropProblem |= FROMOUTSIDE; break;
			case 27: 
			    DSTWProblem |= FROMOUTSIDE; break;
			case 28: 
			    StatusTrapProblem |= FROMOUTSIDE; break;
			case 29: 
			    AlignmentProblem |= FROMOUTSIDE; break;
			case 30: 
			    StairsProblem |= FROMOUTSIDE; break;
			case 31: 
			    UninformationProblem |= FROMOUTSIDE; break;
			case 32: 
			    IntrinsicLossProblem |= FROMOUTSIDE; break;
			case 33: 
			    BloodLossProblem |= FROMOUTSIDE; break;
			case 34: 
			    BadEffectProblem |= FROMOUTSIDE; break;
			case 35: 
			    TrapCreationProblem |= FROMOUTSIDE; break;
			case 36: 
			    AutomaticVulnerabilitiy |= FROMOUTSIDE; break;
			case 37: 
			    TeleportingItems |= FROMOUTSIDE; break;
			case 38: 
			    NastinessProblem |= FROMOUTSIDE; break;
			case 39: 
			    RecurringAmnesia |= FROMOUTSIDE; break;
			case 40: 
			    BigscriptEffect |= FROMOUTSIDE; break;
			case 41: 
			    BankTrapEffect |= FROMOUTSIDE; break;
			case 42: 
			    MapTrapEffect |= FROMOUTSIDE; break;
			case 43: 
			    TechTrapEffect |= FROMOUTSIDE; break;
			case 44: 
			    RecurringDisenchant |= FROMOUTSIDE; break;
			case 45: 
			    verisiertEffect |= FROMOUTSIDE; break;
			case 46: 
			    ChaosTerrain |= FROMOUTSIDE; break;
			case 47: 
			    Muteness |= FROMOUTSIDE; break;
			case 48: 
			    EngravingDoesntWork |= FROMOUTSIDE; break;
			case 49: 
			    MagicDeviceEffect |= FROMOUTSIDE; break;
			case 50: 
			    BookTrapEffect |= FROMOUTSIDE; break;
			case 51: 
			    LevelTrapEffect |= FROMOUTSIDE; break;
			case 52: 
			    QuizTrapEffect |= FROMOUTSIDE; break;
			case 53: 
			    CaptchaProblem |= FROMOUTSIDE; break;
			case 54: 
			    FarlookProblem |= FROMOUTSIDE; break;
			case 55: 
			    RespawnProblem |= FROMOUTSIDE; break;
			case 56: 
			    FastMetabolismEffect |= FROMOUTSIDE; break;
			case 57: 
			    NoReturnEffect |= FROMOUTSIDE; break;
			case 58: 
			    AlwaysEgotypeMonsters |= FROMOUTSIDE; break;
			case 59: 
			    TimeGoesByFaster |= FROMOUTSIDE; break;
			case 60: 
			    FoodIsAlwaysRotten |= FROMOUTSIDE; break;
			case 61: 
			    AllSkillsUnskilled |= FROMOUTSIDE; break;
			case 62: 
			    AllStatsAreLower |= FROMOUTSIDE; break;
			case 63: 
			    PlayerCannotTrainSkills |= FROMOUTSIDE; break;
			case 64: 
			    PlayerCannotExerciseStats |= FROMOUTSIDE; break;
			case 65: 
			    TurnLimitation |= FROMOUTSIDE; break;
			case 66: 
			    WeakSight |= FROMOUTSIDE; break;
			case 67: 
			    RandomMessages |= FROMOUTSIDE; break;
			case 68: 
			    Desecration |= FROMOUTSIDE; break;
			case 69: 
			    StarvationEffect |= FROMOUTSIDE; break;
			case 70: 
			    NoDropsEffect |= FROMOUTSIDE; break;
			case 71: 
			    LowEffects |= FROMOUTSIDE; break;
			case 72: 
			    InvisibleTrapsEffect |= FROMOUTSIDE; break;
			case 73: 
			    GhostWorld |= FROMOUTSIDE; break;
			case 74: 
			    Dehydration |= FROMOUTSIDE; break;
			case 75: 
			    HateTrapEffect |= FROMOUTSIDE; break;
			case 76: 
			    TotterTrapEffect |= FROMOUTSIDE; break;
			case 77: 
			    Nonintrinsics |= FROMOUTSIDE; break;
			case 78: 
			    Dropcurses |= FROMOUTSIDE; break;
			case 79: 
			    Nakedness |= FROMOUTSIDE; break;
			case 80: 
			    Antileveling |= FROMOUTSIDE; break;
			case 81: 
			    ItemStealingEffect |= FROMOUTSIDE; break;
			case 82: 
			    Rebellions |= FROMOUTSIDE; break;
			case 83: 
			    CrapEffect |= FROMOUTSIDE; break;
			case 84: 
			    ProjectilesMisfire |= FROMOUTSIDE; break;
			case 85: 
			    WallTrapping |= FROMOUTSIDE; break;
			case 86: 
			    DisconnectedStairs |= FROMOUTSIDE; break;
			case 87: 
			    InterfaceScrewed |= FROMOUTSIDE; break;
			case 88: 
			    Bossfights |= FROMOUTSIDE; break;
			case 89: 
			    EntireLevelMode |= FROMOUTSIDE; break;
			case 90: 
			    BonesLevelChange |= FROMOUTSIDE; break;
			case 91: 
			    AutocursingEquipment |= FROMOUTSIDE; break;
			case 92: 
			    HighlevelStatus |= FROMOUTSIDE; break;
			case 93: 
			    SpellForgetting |= FROMOUTSIDE; break;
			case 94: 
			    SoundEffectBug |= FROMOUTSIDE; break;
			case 95: 
			    TimerunBug |= FROMOUTSIDE; break;
				case 96:
				    LootcutBug |= FROMOUTSIDE; break;
				case 97:
				    MonsterSpeedBug |= FROMOUTSIDE; break;
				case 98:
				    ScalingBug |= FROMOUTSIDE; break;
				case 99:
				    EnmityBug |= FROMOUTSIDE; break;
				case 100:
				    WhiteSpells |= FROMOUTSIDE; break;
				case 101:
				    CompleteGraySpells |= FROMOUTSIDE; break;
				case 102:
				    QuasarVision |= FROMOUTSIDE; break;
				case 103:
				    MommaBugEffect |= FROMOUTSIDE; break;
				case 104:
				    HorrorBugEffect |= FROMOUTSIDE; break;
				case 105:
				    ArtificerBug |= FROMOUTSIDE; break;
				case 106:
				    WereformBug |= FROMOUTSIDE; break;
				case 107:
				    NonprayerBug |= FROMOUTSIDE; break;
				case 108:
				    EvilPatchEffect |= FROMOUTSIDE; break;
				case 109:
				    HardModeEffect |= FROMOUTSIDE; break;
				case 110:
				    SecretAttackBug |= FROMOUTSIDE; break;
				case 111:
				    EaterBugEffect |= FROMOUTSIDE; break;
				case 112:
				    CovetousnessBug |= FROMOUTSIDE; break;
				case 113:
				    NotSeenBug |= FROMOUTSIDE; break;
				case 114:
				    DarkModeBug |= FROMOUTSIDE; break;
				case 115:
				    AntisearchEffect |= FROMOUTSIDE; break;
				case 116:
				    HomicideEffect |= FROMOUTSIDE; break;
				case 117:
				    NastynationBug |= FROMOUTSIDE; break;
				case 118:
				    WakeupCallBug |= FROMOUTSIDE; break;
				case 119:
				    GrayoutBug |= FROMOUTSIDE; break;
				case 120:
				    GrayCenterBug |= FROMOUTSIDE; break;
				case 121:
				    CheckerboardBug |= FROMOUTSIDE; break;
				case 122:
				    ClockwiseSpinBug |= FROMOUTSIDE; break;
				case 123:
				    CounterclockwiseSpin |= FROMOUTSIDE; break;
				case 124:
				    LagBugEffect |= FROMOUTSIDE; break;
				case 125:
				    BlesscurseEffect |= FROMOUTSIDE; break;
				case 126:
				    DeLightBug |= FROMOUTSIDE; break;
				case 127:
				    DischargeBug |= FROMOUTSIDE; break;
				case 128:
				    TrashingBugEffect |= FROMOUTSIDE; break;
				case 129:
				    FilteringBug |= FROMOUTSIDE; break;
				case 130:
				    DeformattingBug |= FROMOUTSIDE; break;
				case 131:
				    FlickerStripBug |= FROMOUTSIDE; break;
				case 132:
				    UndressingEffect |= FROMOUTSIDE; break;
				case 133:
				    Hyperbluewalls |= FROMOUTSIDE; break;
				case 134:
				    NoliteBug |= FROMOUTSIDE; break;
				case 135:
				    ParanoiaBugEffect |= FROMOUTSIDE; break;
				case 136:
				    FleecescriptBug |= FROMOUTSIDE; break;
				case 137:
				    InterruptEffect |= FROMOUTSIDE; break;
				case 138:
				    DustbinBug |= FROMOUTSIDE; break;
				case 139:
				    ManaBatteryBug |= FROMOUTSIDE; break;
				case 140:
				    Monsterfingers |= FROMOUTSIDE; break;
				case 141:
				    MiscastBug |= FROMOUTSIDE; break;
				case 142:
				    MessageSuppression |= FROMOUTSIDE; break;
				case 143:
				    StuckAnnouncement |= FROMOUTSIDE; break;
				case 144:
				    BloodthirstyEffect |= FROMOUTSIDE; break;
				case 145:
				    MaximumDamageBug |= FROMOUTSIDE; break;
				case 146:
				    LatencyBugEffect |= FROMOUTSIDE; break;
				case 147:
				    StarlitBug |= FROMOUTSIDE; break;
				case 148:
				    KnowledgeBug |= FROMOUTSIDE; break;
				case 149:
				    HighscoreBug |= FROMOUTSIDE; break;
				case 150:
				    PinkSpells |= FROMOUTSIDE; break;
				case 151:
				    GreenSpells |= FROMOUTSIDE; break;
				case 152:
				    EvencoreEffect |= FROMOUTSIDE; break;
				case 153:
				    UnderlayerBug |= FROMOUTSIDE; break;
				case 154:
				    DamageMeterBug |= FROMOUTSIDE; break;
				case 155:
				    ArbitraryWeightBug |= FROMOUTSIDE; break;
				case 156:
				    FuckedInfoBug |= FROMOUTSIDE; break;
				case 157:
				    BlackSpells |= FROMOUTSIDE; break;
				case 158:
				    CyanSpells |= FROMOUTSIDE; break;
				case 159:
				    HeapEffectBug |= FROMOUTSIDE; break;
				case 160:
				    BlueSpells |= FROMOUTSIDE; break;
				case 161:
				    TronEffect |= FROMOUTSIDE; break;
				case 162:
				    RedSpells |= FROMOUTSIDE; break;
				case 163:
				    TooHeavyEffect |= FROMOUTSIDE; break;
				case 164:
				    ElongationBug |= FROMOUTSIDE; break;
				case 165:
				    WrapoverEffect |= FROMOUTSIDE; break;
				case 166:
				    DestructionEffect |= FROMOUTSIDE; break;
				case 167:
				    MeleePrefixBug |= FROMOUTSIDE; break;
				case 168:
				    AutomoreBug |= FROMOUTSIDE; break;
				case 169:
				    UnfairAttackBug |= FROMOUTSIDE; break;
		}

		break;

	case SPE_COMMAND_DEMON:

		{
		    int i, j, bd = 1;
		    struct monst *mtmp;

		    for(i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if ((mtmp = m_at(u.ux + i, u.uy + j)) != 0 && (is_demon(mtmp->data)))
				(void) tamedog(mtmp, (struct obj *) 0, FALSE);
		    }
		}

		break;

	case SPE_SELFDESTRUCT:

		{
		    int i, j, bd = 3;
		    struct monst *mtmp;

			u.youaredead = 1;

			pline("%s used SELFDESTRUCT!", plname);

		    for(i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if ((mtmp = m_at(u.ux + i, u.uy + j)) != 0) {
				mtmp->mhp -= rnd(u.uhpmax * 5);
				pline("%s is hit by the explosion!", Monnam(mtmp));
				if (mtmp->mhp <= 0) {
					pline("%s is killed!", Monnam(mtmp));
					xkilled(mtmp, 0);
				}
			}

		    }

			killer = "selfdestructing";
			killer_format = KILLED_BY;
			done(DIED);
			u.youaredead = 0;
			/* No, being polymorphed does not save you. If it did, this spell would be rendered overpowered. --Amy */

		}

		break;

	case SPE_FINAL_EXPLOSION:

		{
		    int i, j, bd = 5;
		    struct monst *mtmp;

			u.youaredead = 1;
			pline("%s used EXPLOSION!", plname);

		    for(i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if ((mtmp = m_at(u.ux + i, u.uy + j)) != 0) {
				mtmp->mhp -= rnd(u.uhpmax * 10);
				pline("%s is hit by the explosion!", Monnam(mtmp));
				if (mtmp->mhp <= 0) {
					pline("%s is killed!", Monnam(mtmp));
					xkilled(mtmp, 0);
				}
			}

		    }

			killer = "exploding";
			killer_format = KILLED_BY;
			done(DIED);
			u.youaredead = 0;
			/* No, being polymorphed does not save you. If it did, this spell would be rendered overpowered. --Amy */

		}

		break;

	case SPE_EARTHQUAKE:
		pline_The("entire dungeon is shaking around you!");
		do_earthquake((u.ulevel - 1) / 3 + 1);
		if (!rn2(3)) {
			int disableamount = rnd(3);
			while (disableamount) {
				deacrandomintrinsic(rnz( (monster_difficulty() * 10) + 1));
				disableamount--;
				if (disableamount < 0) disableamount = 0;
			} 
		}

		break;

	case SPE_LYCANTHROPY:

		if (u.ulycn != NON_PM) {
			pline("You are already lycanthropic!");
			break;
		}

		{
			int attempts = 0;
			int monstZ;
			do {

				monstZ = rn2(NUMMONS);
				attempts++;

			} while (!(is_were(&mons[monstZ])) || (mons[monstZ].mlet == S_HUMAN) && attempts < 50000);

			if (is_were(&mons[monstZ]) && !(mons[monstZ].mlet == S_HUMAN) ) {
				u.ulycn = monstZ;
				pline("You feel feverish.");
			}

		}

		break;

	case SPE_BUC_RANDOMIZATION:

	    {	register struct obj *objC;
		    if (Hallucination)
			You_feel("the power of the Force against you!");
		    else
			You_feel("like you need some help.");
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");

		    for (objC = invent; objC; objC = objC->nobj) {
			long wornmask;
#ifdef GOLDOBJ
			/* gold isn't subject to cursing and blessing */
			if (objC->oclass == COIN_CLASS) continue;
#endif
			wornmask = (objC->owornmask & ~(W_BALL|W_ART|W_ARTI));
			if (wornmask) {
			    /* handle a couple of special cases; we don't
			       allow auxiliary weapon slots to be used to
			       artificially increase number of worn items */
			    if (objC == uswapwep) {
				if (!u.twoweap) wornmask = 0L;
			    } else if (objC == uquiver) {
				if (objC->oclass == WEAPON_CLASS) {
				    /* mergeable weapon test covers ammo,
				       missiles, spears, daggers & knives */
				    if (!objects[objC->otyp].oc_merge) 
					wornmask = 0L;
				} else if (objC->oclass == GEM_CLASS) {
				    /* possibly ought to check whether
				       alternate weapon is a sling... */
				    if (!uslinging()) wornmask = 0L;
				} else {
				    /* weptools don't merge and aren't
				       reasonable quivered weapons */
				    wornmask = 0L;
				}
			    }
			}
			if ((wornmask ||
			     objC->otyp == LOADSTONE ||
			     objC->otyp == LOADBOULDER ||
			     objC->otyp == STARLIGHTSTONE ||
			     objC->otyp == LUCKSTONE ||
			     objC->otyp == HEALTHSTONE ||
			     objC->otyp == MANASTONE ||
			     objC->otyp == SLEEPSTONE ||
			     objC->otyp == STONE_OF_MAGIC_RESISTANCE ||
			     is_nastygraystone(objC) ||
			     (objC->otyp == LEATHER_LEASH && objC->leashmon) || (objC->otyp == INKA_LEASH && objC->leashmon) ) && !stack_too_big(objC) ) {
			    	blessorcurse(objC, 2);
			}
		}

		update_inventory();

		break;
	    }

	case SPE_MESSAGE:

		pline(fauxmessage());
		if (!rn2(3)) pline(fauxmessage());

		break;

	case SPE_RUMOR:

		{
			const char *line;
			char buflin[BUFSZ];
			if (rn2(2)) line = getrumor(-1, buflin, TRUE);
			else line = getrumor(0, buflin, TRUE);
			if (!*line) line = "Slash'EM rumors file closed for renovation.";
			pline("%s", line);
		}

		break;

	case SPE_TRAP_DISARMING:

		You_feel("out of the danger zone.");
		{
			int rtrap;
			struct trap *ttmp;

		    int i, j;

		      for (i = -1; i <= 1; i++) for(j = -1; j <= 1; j++) {

				if ((ttmp = t_at(u.ux + i, u.uy + j)) != 0) {
				    if (ttmp->ttyp == MAGIC_PORTAL) continue;
					deltrap(ttmp);
				}

			}
		}

		pline("You are hit by the magical reaction from casting this very powerful spell.");
		u.uenmax -= rnd(5);
		badeffect();

		break;

	case SPE_WISHING:

		if (u.uhpmax < 501) {
			pline("You don't have enough health to control the powers of this spell!");
			break;
		}
		if (Upolyd && u.mhmax < 501) {
			pline("You don't have enough health to control the powers of this spell!");
			break;
		}
		if (u.uenmax < 501) {
			pline("You don't have enough mana to control the powers of this spell!");
			break;
		}

		u.uhpmax -= rnd(500);
		if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		if (Upolyd) {
			u.mhmax -= rnd(500);
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		}
		u.uenmax -= rnd(500);
		if (u.uen > u.uenmax) u.uen = u.uenmax;

		ABASE(A_STR) -= rnd(5);
		if (ABASE(A_STR) < ATTRMIN(A_STR)) ABASE(A_STR) = ATTRMIN(A_STR);
		AMAX(A_STR) = ABASE(A_STR);

		ABASE(A_DEX) -= rnd(5);
		if (ABASE(A_DEX) < ATTRMIN(A_DEX)) ABASE(A_DEX) = ATTRMIN(A_DEX);
		AMAX(A_DEX) = ABASE(A_DEX);

		ABASE(A_INT) -= rnd(5);
		if (ABASE(A_INT) < ATTRMIN(A_INT)) ABASE(A_INT) = ATTRMIN(A_INT);
		AMAX(A_INT) = ABASE(A_INT);

		ABASE(A_WIS) -= rnd(5);
		if (ABASE(A_WIS) < ATTRMIN(A_WIS)) ABASE(A_WIS) = ATTRMIN(A_WIS);
		AMAX(A_WIS) = ABASE(A_WIS);

		ABASE(A_CON) -= rnd(5);
		if (ABASE(A_CON) < ATTRMIN(A_CON)) ABASE(A_CON) = ATTRMIN(A_CON);
		AMAX(A_CON) = ABASE(A_CON);

		ABASE(A_CHA) -= rnd(5);
		if (ABASE(A_CHA) < ATTRMIN(A_CHA)) ABASE(A_CHA) = ATTRMIN(A_CHA);
		AMAX(A_CHA) = ABASE(A_CHA);

		makewish();

		break;

	case SPE_ACQUIREMENT:

		if (u.uhpmax < 101) {
			pline("You don't have enough health to control the powers of this spell!");
			break;
		}
		if (Upolyd && u.mhmax < 101) {
			pline("You don't have enough health to control the powers of this spell!");
			break;
		}
		if (u.uenmax < 101) {
			pline("You don't have enough mana to control the powers of this spell!");
			break;
		}

		u.uhpmax -= rnd(33);
		if (rn2(2)) u.uhpmax -= rnd(33);
		if (!rn2(4)) u.uhpmax -= rnd(33);
		if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		if (Upolyd) {
			u.mhmax -= rnd(33);
			if (rn2(2)) u.mhmax -= rnd(33);
			if (!rn2(4)) u.mhmax -= rnd(33);
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		}
		u.uenmax -= rnd(33);
		if (rn2(2)) u.uenmax -= rnd(33);
		if (!rn2(4)) u.uenmax -= rnd(33);
		if (u.uen > u.uenmax) u.uen = u.uenmax;

		if (!rn2(2)) {
		ABASE(A_STR) -= 1;
		if (ABASE(A_STR) < ATTRMIN(A_STR)) ABASE(A_STR) = ATTRMIN(A_STR);
		AMAX(A_STR) = ABASE(A_STR);
		}

		if (!rn2(2)) {
		ABASE(A_DEX) -= 1;
		if (ABASE(A_DEX) < ATTRMIN(A_DEX)) ABASE(A_DEX) = ATTRMIN(A_DEX);
		AMAX(A_DEX) = ABASE(A_DEX);
		}

		if (!rn2(2)) {
		ABASE(A_INT) -= 1;
		if (ABASE(A_INT) < ATTRMIN(A_INT)) ABASE(A_INT) = ATTRMIN(A_INT);
		AMAX(A_INT) = ABASE(A_INT);
		}

		if (!rn2(2)) {
		ABASE(A_WIS) -= 1;
		if (ABASE(A_WIS) < ATTRMIN(A_WIS)) ABASE(A_WIS) = ATTRMIN(A_WIS);
		AMAX(A_WIS) = ABASE(A_WIS);
		}

		if (!rn2(2)) {
		ABASE(A_CON) -= 1;
		if (ABASE(A_CON) < ATTRMIN(A_CON)) ABASE(A_CON) = ATTRMIN(A_CON);
		AMAX(A_CON) = ABASE(A_CON);
		}

		if (!rn2(2)) {
		ABASE(A_CHA) -= 1;
		if (ABASE(A_CHA) < ATTRMIN(A_CHA)) ABASE(A_CHA) = ATTRMIN(A_CHA);
		AMAX(A_CHA) = ABASE(A_CHA);
		}

		register struct obj *acqo;
		int acquireditem;
		acquireditem = 0;

		pline("Pick an item type that you want to acquire. The prompt will loop until you actually make a choice.");

		while (acquireditem == 0) { /* ask the player what they want --Amy */

			if (yn("Do you want to acquire a random item?")=='y') {
				    acqo = mkobj_at(RANDOM_CLASS, u.ux, u.uy, FALSE);	acquireditem = 1; }
			else if (yn("Do you want to acquire a weapon?")=='y') {
				    acqo = mkobj_at(WEAPON_CLASS, u.ux, u.uy, FALSE);	acquireditem = 1; }
			else if (yn("Do you want to acquire an armor?")=='y') {
				    acqo = mkobj_at(ARMOR_CLASS, u.ux, u.uy, FALSE);	acquireditem = 1; }
			else if (yn("Do you want to acquire a ring?")=='y') {
				    acqo = mkobj_at(RING_CLASS, u.ux, u.uy, FALSE);	acquireditem = 1; }
			else if (yn("Do you want to acquire an amulet?")=='y') {
				    acqo = mkobj_at(AMULET_CLASS, u.ux, u.uy, FALSE);	acquireditem = 1; }
			else if (yn("Do you want to acquire an implant?")=='y') {
				    acqo = mkobj_at(IMPLANT_CLASS, u.ux, u.uy, FALSE);	acquireditem = 1; }
			else if (yn("Do you want to acquire a tool?")=='y') {
				    acqo = mkobj_at(TOOL_CLASS, u.ux, u.uy, FALSE);	acquireditem = 1; }
			else if (yn("Do you want to acquire some food?")=='y') {
				    acqo = mkobj_at(FOOD_CLASS, u.ux, u.uy, FALSE);	acquireditem = 1; }
			else if (yn("Do you want to acquire a potion?")=='y') {
				    acqo = mkobj_at(POTION_CLASS, u.ux, u.uy, FALSE);	acquireditem = 1; }
			else if (yn("Do you want to acquire a scroll?")=='y') {
				    acqo = mkobj_at(SCROLL_CLASS, u.ux, u.uy, FALSE);	acquireditem = 1; }
			else if (yn("Do you want to acquire a spellbook?")=='y') {
				    acqo = mkobj_at(SPBOOK_CLASS, u.ux, u.uy, FALSE);	acquireditem = 1; }
			else if (yn("Do you want to acquire a wand?")=='y') {
				    acqo = mkobj_at(WAND_CLASS, u.ux, u.uy, FALSE);	acquireditem = 1; }
			else if (yn("Do you want to acquire some coins?")=='y') {
				    acqo = mkobj_at(COIN_CLASS, u.ux, u.uy, FALSE);	acquireditem = 1; }
			else if (yn("Do you want to acquire a gem?")=='y') {
				    acqo = mkobj_at(GEM_CLASS, u.ux, u.uy, FALSE);	acquireditem = 1; }
			else if (yn("Do you want to acquire a boulder or statue?")=='y') {
				    acqo = mkobj_at(ROCK_CLASS, u.ux, u.uy, FALSE);	acquireditem = 1; }
			else if (yn("Do you want to acquire a heavy iron ball?")=='y') {
				    acqo = mkobj_at(BALL_CLASS, u.ux, u.uy, FALSE);	acquireditem = 1; }
			else if (yn("Do you want to acquire an iron chain?")=='y') {
				    acqo = mkobj_at(CHAIN_CLASS, u.ux, u.uy, FALSE);	acquireditem = 1; }
			else if (yn("Do you want to acquire a splash of venom?")=='y') {
				    acqo = mkobj_at(VENOM_CLASS, u.ux, u.uy, FALSE);	acquireditem = 1; }
	
		}
		if (!acqo) {
			pline("Unfortunately it failed.");
			return 0;
		}

		/* special handling to prevent wands of wishing or similarly overpowered items --Amy */

		if (acqo->otyp == GOLD_PIECE) acqo->quan = rnd(1000);
		if (acqo->otyp == MAGIC_LAMP || acqo->otyp == TREASURE_CHEST) { acqo->otyp = OIL_LAMP; acqo->age = 1500L; }
		if (acqo->otyp == MAGIC_MARKER) acqo->recharged = 1;
	    while(acqo->otyp == WAN_WISHING || acqo->otyp == WAN_POLYMORPH || acqo->otyp == WAN_MUTATION || acqo->otyp == WAN_ACQUIREMENT)
		acqo->otyp = rnd_class(WAN_LIGHT, WAN_PSYBEAM);
	    while (acqo->otyp == SCR_WISHING || acqo->otyp == SCR_RESURRECTION || acqo->otyp == SCR_ACQUIREMENT || acqo->otyp == SCR_ENTHRONIZATION || acqo->otyp == SCR_FOUNTAIN_BUILDING || acqo->otyp == SCR_SINKING || acqo->otyp == SCR_WC)
		acqo->otyp = rnd_class(SCR_CREATE_MONSTER, SCR_BLANK_PAPER);

		pline("Something appeared on the ground just beneath you!");

		break;

	case SPE_MASS_HEALING:
		{
		    register struct monst *mtmp, *mtmp2;

		    for (mtmp = fmon; mtmp; mtmp = mtmp2) {
			mtmp2 = mtmp->nmon;
			if (mtmp == &youmonst) continue;
			if (DEADMONSTER(mtmp)) continue;
			if (!mtmp->mtame) {
			    if (!Hallucination && (!mtmp->mpeaceful || !rn2(2)))
				continue;
			}
			if (!canspotmon(mtmp)) continue;	/*you can't see it and can't sense it*/

			mtmp->mhp += rnd(200);
			if (mtmp->mhp > mtmp->mhpmax) {
			    mtmp->mhp = mtmp->mhpmax;
			}

		    }

		    healup(d(6,8) + rnz(u.ulevel),0,0,0);
		}
		break;

	case SPE_TIME_SHIFT:
		pline("Time goes forward...");
		monstermoves += 5;
		moves += 5;

		break;

	case SPE_CHEMISTRY:
		You("call upon your chemical knowledge. Nothing happens.");
		break;
	case SPE_CURE_SICKNESS:
		if (Sick) You("are no longer ill.");
		if (Slimed) {
		    pline_The("slime disappears!");
		    Slimed = 0;
		 /* flags.botl = 1; -- healup() handles this */
		}
		healup(0, 0, TRUE, FALSE);
		break;
	case SPE_CURE_HALLUCINATION:
		make_hallucinated(0L,TRUE,0L);
		break;
	case SPE_CURE_CONFUSION:
		make_confused(0L,TRUE);
		break;
	case SPE_CURE_STUN:
		make_stunned(0L,TRUE);
		break;
	case SPE_CURE_DIM:
		make_dimmed(0L,TRUE);
		break;
	case SPE_CURE_BURN:
		make_burned(0L,TRUE);
		break;
	case SPE_CURE_FREEZE:
		make_frozen(0L,TRUE);
		break;
	case SPE_CURE_NUMBNESS:
		make_numbed(0L,TRUE);
		break;
	case SPE_CURE_RANDOM_STATUS:
		switch (rnd(10)) {
			case 1:
				if (Sick) You("are no longer ill.");
				if (Slimed) {
				    pline_The("slime disappears!");
				    Slimed = 0;
				}
				healup(0, 0, TRUE, FALSE);
				break;
			case 2:
				make_hallucinated(0L,TRUE,0L);
				break;
			case 3:
				make_confused(0L,TRUE);
				break;
			case 4:
				make_stunned(0L,TRUE);
				break;
			case 5:
				make_burned(0L,TRUE);
				break;
			case 6:
				make_frozen(0L,TRUE);
				break;
			case 7:
				make_numbed(0L,TRUE);
				break;
			case 8:
				make_blinded(0L,FALSE);
				break;
			case 9:
				make_feared(0L,FALSE);
				break;
			case 10:
				make_dimmed(0L,FALSE);
				break;
		}
		break;

	case SPE_STINKING_CLOUD:
	      {  coord cc;
		pline("Where do you want to center the cloud?");
		cc.x = u.ux;
		cc.y = u.uy;
		if (getpos(&cc, TRUE, "the desired position") < 0) {
		    pline(Never_mind);
		    obfree(pseudo, (struct obj *)0);
		    return 0;
		}
		if (!cansee(cc.x, cc.y) || distu(cc.x, cc.y) >= 32) {
		    You("smell rotten eggs.");
		    obfree(pseudo, (struct obj *)0);
		    return 0;
		}
		(void) create_gas_cloud(cc.x, cc.y, 2, 5);
		break;
		}
		break;
	case SPE_FIXING:
		You_feel("revitalized.");
		if (Stoned) fix_petrification();
		    if (Slimed) {
			pline("The slime disappears.");
			Slimed =0;
		    }
		    make_sick(0L, (char *) 0, FALSE, SICK_ALL);
			make_blinded(0L,FALSE);
		    make_stunned(0L,TRUE);
		    make_confused(0L,TRUE);
		    (void) make_hallucinated(0L,FALSE,0L);
		    make_numbed(0L,TRUE);
		    make_feared(0L,TRUE);
		    make_frozen(0L,TRUE);
		    make_burned(0L,TRUE);
		    make_dimmed(0L,TRUE);
		break;

	case SPE_FIRE:

		if (Underwater)
			pline_The("water around you vaporizes violently!");
		else {
		    pline_The("air around you erupts in a tower of flame!");
		    burn_away_slime();
		}
		explode(u.ux, u.uy, ZT_SPELL(ZT_FIRE), 3 + (spell_damage_bonus(spellid(spell)) / 2), SCROLL_CLASS, EXPL_FIERY);

		break;

	case SPE_RUSSIAN_ROULETTE:

		if (rn2(10)) {
			pline("Click!");
			badeffect();

		} else if (rn2(10)) {
			pline("Click! You feel vitalized.");
			u.uhpmax++;
			if (Upolyd) u.mhmax++;

		} else {
			pline("BANG! You suffer from extreme blood loss!");
			u.uhp -= rnd(50);
			u.uhpmax -= rnd(50);
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			if (u.uhp < 1) {
				u.youaredead = 1;
				killer = "playing russian roulette";
				killer_format = KILLED_BY;
				done(DIED);
				u.youaredead = 0;
			}
			if (Upolyd) {
				u.mh -= rnd(50);
				u.mhmax -= rnd(50);
				if (u.mh > u.mhmax) u.mh = u.mhmax;
				if (u.mh < 1) {
					u.youaredead = 1;
					killer = "playing russian roulette";
					killer_format = KILLED_BY;
					done(DIED);
					u.youaredead = 0;
				}
			}
			u.uenmax -= rnd(30);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;

		}

		break;

	case SPE_ACID_INGESTION:
		pline("Sulfuric acid forms in your mouth...");
		if (Acid_resistance) {
			pline("This tastes %s.", Hallucination ? "tangy" : "sour");
		} else {
			pline("This burns a lot!");
			losehp(d(2, 8), "ingesting acid", KILLED_BY);
		}
		if (Stoned) fix_petrification();

		break;

	case SPE_INDUCE_VOMITING:
		pline("You put your %s into your mouth...", body_part(FINGER));

		You_feel("like you're going to throw up.");
	      make_vomiting(Vomiting+20, TRUE);
		if (Sick && Sick < 100)
			set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */

		break;

	case SPE_REBOOT:
		You("decide to reboot.");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Eto poshel na khuy vverkh. No chto zhe vy ozhidali? Igra, v kotoruyu vy mozhete legko vyigrat'? Durak!" : "DUEUEDUET!");
		if (!Race_if(PM_UNGENOMOLD)) newman();
		else polyself(FALSE);

		break;

	case SPE_FORGOTTEN_SPELL:
		Your("knowledge of this spell is twisted.");
		pline("It invokes nightmarish images in your mind...");

		int duration = rn1(25,25);

		switch (rn2(17)) {
		case 0:
		case 1:
		case 2:
		case 3: make_confused(HConfusion + duration, FALSE);			/* 40% */
			break;
		case 4:
		case 5:
		case 6: make_confused(HConfusion + (2L * duration / 3L), FALSE);		/* 30% */
			make_stunned(HStun + (duration / 3L), FALSE);
			break;
		case 7:
		case 8: make_stunned(HStun + (2L * duration / 3L), FALSE);		/* 20% */
			make_confused(HConfusion + (duration / 3L), FALSE);
			break;
		case 9: make_stunned(HStun + duration, FALSE);			/* 10% */
			break;
		case 10: make_numbed(HNumbed + duration, FALSE);			/* 10% */
			break;
		case 11: make_frozen(HFrozen + duration, FALSE);			/* 10% */
			break;
		case 12: make_burned(HBurned + duration, FALSE);			/* 10% */
			break;
		case 13: make_feared(HFeared + duration, FALSE);			/* 10% */
			break;
		case 14: make_blinded(Blinded + duration, FALSE);			/* 10% */
			break;
		case 15: make_hallucinated(HHallucination + duration, FALSE, 0L);			/* 10% */
			break;
		case 16: make_dimmed(HDimmed + duration, FALSE);			/* 10% */
			break;
		}

		if (!rn2(25)) {
			badeffect();
		}
		break;

	case SPE_TIME_STOP:
		pline((Role_if(PM_SAMURAI) || Role_if(PM_NINJA)) ? "Jikan ga teishi shimashita." : "Time has stopped.");
		if (rn2(3)) TimeStopped += (rnd(3) + 1);
		else TimeStopped += rnd(3 + spell_damage_bonus(spellid(spell)) );
		break;
	case SPE_LEVELPORT:
	      if (!flags.lostsoul && !flags.uberlostsoul && !(flags.wonderland && !(u.wonderlandescape)) && !(u.uprops[STORM_HELM].extrinsic) && !(In_bellcaves(&u.uz)) && !(In_subquest(&u.uz)) && !(In_voiddungeon(&u.uz)) && !(In_netherrealm(&u.uz))) {
			level_tele();
			pline("From your strain of casting such a powerful spell, the magical energy backlashes on you.");
			badeffect();
		}
		else pline("Hmm... that level teleport spell didn't do anything.");

		break;
	case SPE_WARPING:
		if (u.uevent.udemigod || u.uhave.amulet || (uarm && uarm->oartifact == ART_CHECK_YOUR_ESCAPES) || NoReturnEffect || u.uprops[NORETURN].extrinsic || have_noreturnstone() || (u.usteed && mon_has_amulet(u.usteed))) { pline("You shudder for a moment."); break;}

		if (flags.lostsoul || flags.uberlostsoul || (flags.wonderland && !(u.wonderlandescape)) || u.uprops[STORM_HELM].extrinsic || In_bellcaves(&u.uz) || In_subquest(&u.uz) || In_voiddungeon(&u.uz) || In_netherrealm(&u.uz)) { 
			pline("You're unable to warp!"); break;}

		make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

		if (rn2(2)) {(void) safe_teleds(FALSE); goto_level(&medusa_level, TRUE, FALSE, FALSE); }
		else {(void) safe_teleds(FALSE); goto_level(&portal_level, TRUE, FALSE, FALSE); }

		register int newlev = rnd(99);
		d_level newlevel;
		get_level(&newlevel, newlev);
		goto_level(&newlevel, TRUE, FALSE, FALSE);

		if (rn2(2)) {
			pline("From your strain of casting such a powerful spell, the magical energy backlashes on you.");
			badeffect();
		}

		break;
	case SPE_TRAP_CREATION:

		You_feel("endangered!!");
		{
			int rtrap;
		      int i, j, bd = 1;

		      for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
				if (!isok(u.ux + i, u.uy + j)) continue;
				if (levl[u.ux + i][u.uy + j].typ <= DBWALL) continue;
				if (t_at(u.ux + i, u.uy + j)) continue;

			      rtrap = randomtrap();

				(void) maketrap(u.ux + i, u.uy + j, rtrap, 100);
			}
		}

		makerandomtrap();
		if (!rn2(2)) makerandomtrap();
		if (!rn2(4)) makerandomtrap();
		if (!rn2(8)) makerandomtrap();
		if (!rn2(16)) makerandomtrap();
		if (!rn2(32)) makerandomtrap();
		if (!rn2(64)) makerandomtrap();
		if (!rn2(128)) makerandomtrap();
		if (!rn2(256)) makerandomtrap();

		break;

	case SPE_STUN_SELF:
		if(!Stunned)
			pline("You stagger a bit...");
		make_stunned(HStun + rnd(25), FALSE);
		break;
	case SPE_CONFUSE_SELF:
		if(!Confusion)
			You_feel("somewhat dizzy.");
		make_confused(HConfusion + rnd(25), FALSE);
		break;
	case SPE_BLIND_SELF:
		if (!Blind && !u.usleep) pline("It suddenly gets dark.");
		make_blinded(Blinded + rnd(25), FALSE);
		if (!Blind && !u.usleep) Your(vision_clears);
		break;

	case SPE_AIR_CURRENT:
		if (Hallucination)
			You_hear("air current noises, and a remark by Amy about how sexy they are.");
		else
			You_hear("air current noises.");

		pushplayer();
		if (In_sokoban(&u.uz)) {
			change_luck(-1);
			pline("You cheater!");
		}

		break;

	case SPE_DASHING:

		if (!getdir((char *)0)) break;
		if (!u.dx && !u.dy) {
			You("stretch.");
			break;
		}

		spell_dash();

		break;

	case SPE_MELTDOWN:
		You("melt!");
		u.uhpmax -= rnd(10);
		if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		if (u.uhp < 1) {
			u.youaredead = 1;
			killer = "melting down";
			killer_format = KILLED_BY;
			done(DIED);
			u.youaredead = 0;
		}
		if (Upolyd) {
			u.mhmax -= rnd(10);
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		}

		u.uenmax -= rnd(10);
		if (u.uen > u.uenmax) u.uen = u.uenmax;

		int maderoom = 0;
		do_clear_areaX(u.ux, u.uy, 1, undo_barfloodC, (void *)&maderoom);
		if (maderoom) {
			You("have a sense of freedom.");
		}

		break;

	case SPE_FLOOD:
		{
		int madepoolX = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_floodg, (void *)&madepoolX);
		if (madepoolX) pline("Watery pits appear in the dungeon!");
		u.uenmax -= rnd(10);
		if (u.uenmax < 0) u.uenmax = 0;
		if (u.uen > u.uenmax) u.uen = u.uenmax;
		pline("Casting this spell is straining for your maximum mana supply.");

		}
		break;

	case SPE_LAVA:
		{
		int madepool = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_lavafloodg, (void *)&madepool);
		if (madepool) pline("Lava pools are created!");
		u.uenmax -= rnd(10);
		if (u.uenmax < 0) u.uenmax = 0;
		if (u.uen > u.uenmax) u.uen = u.uenmax;
		pline("Casting this spell is straining for your maximum mana supply.");
		}

		break;

	case SPE_IRON_PRISON:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_barfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("Iron bars appear from thin air!");
		u.uenmax -= rnd(24);
		if (u.uenmax < 0) u.uenmax = 0;
		if (u.uen > u.uenmax) u.uen = u.uenmax;
		pline("Casting this spell is straining for your maximum mana supply.");
		}

		break;

	case SPE_CLOUDS:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_cloudfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("Clouds everywhere!");
		u.uenmax -= rnd(4);
		if (u.uenmax < 0) u.uenmax = 0;
		if (u.uen > u.uenmax) u.uen = u.uenmax;
		pline("Casting this spell is straining for your maximum mana supply.");
		}

		break;

	case SPE_ICE:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_icefloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("The landscape is winterized!");
		u.uenmax -= rnd(7);
		if (u.uenmax < 0) u.uenmax = 0;
		if (u.uen > u.uenmax) u.uen = u.uenmax;
		pline("Casting this spell is straining for your maximum mana supply.");
		}

		break;

	case SPE_LOCKOUT:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_lockfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("The area is walled off!");
		u.uenmax -= rnd(30);
		if (u.uenmax < 0) u.uenmax = 0;
		if (u.uen > u.uenmax) u.uen = u.uenmax;
		pline("Casting this spell is straining for your maximum mana supply.");
		}

		break;

	case SPE_GROW_TREES:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_treefloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("Trees start to grow rapidly!");
		u.uenmax -= rnd(25);
		if (u.uenmax < 0) u.uenmax = 0;
		if (u.uen > u.uenmax) u.uen = u.uenmax;
		pline("Casting this spell is straining for your maximum mana supply.");
		}

		break;

	case SPE_CHAOS_TERRAIN:

		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_terrainfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("Chaotic terrain is generated!");
		u.uenmax -= rnd(20);
		if (u.uenmax < 0) u.uenmax = 0;
		if (u.uen > u.uenmax) u.uen = u.uenmax;
		pline("Casting this spell is straining for your maximum mana supply.");
		}

		break;

	case SPE_BOMBING:

		{
		int attempts = 0;
		register struct permonst *ptrZ;

			do {
				ptrZ = rndmonst();
				attempts++;

			} while ( (!ptrZ || (ptrZ && !(attacktype(ptrZ, AT_EXPL)))) && attempts < 50000);

			if (ptrZ && attacktype(ptrZ, AT_EXPL)) {

				register struct monst *bomber;

	/* note by Amy: I can't, for the life of me, remember that converting a permonst to number is done via monsndx.
	 * Therefore I'm leaving a note here that I can grep for the next 200 times I forget that permonst monster number
	 * is done via monsndx! And since I STILL keep forgetting it and NOT find it, I'll leave some more breadcrumbs:
	 * monster index to number is done via the monsndx function
	 * monster number is done via that monsndx function */
				int monnumber = monsndx(ptrZ);

				bomber = make_helper(monnumber, u.ux, u.uy);
				if (!bomber) break;
				bomber->mtame = 10;
				bomber->isspell = bomber->uexp = TRUE;

			}

		}

		break;

	case SPE_ANTI_MAGIC_SHELL:
			pline("You produce an anti-magic shell.");
			u.antimagicshell += rnd(100 + (spell_damage_bonus(spellid(spell)) * 5) );
			/* can't cast it with the shell active, so no need to check for that :D --Amy */

		break;

	case SPE_POSSESSION:
		{
			register struct obj *poss;
            	poss = getobj((const char *)revivables, "possess");
            	if (!poss) {
				pline("Your possession attempt fails.");
				break;
			}
            	if (poss->otyp != CORPSE) {
				pline("That cannot be possessed.");
				break;
			}
			if (obj_resists(poss, 0, 50)) {
				pline("Your attempt to possess such a powerful monster fails.");
				break;
			}
			u.wormpolymorph = poss->corpsenm;
			if (!rn2(5)) {
				u.wormpolymorph = rn2(NUMMONS);
				pline("Your possession attempt goes out of control!");
			}
			if (poss) delobj(poss);
			polyself(FALSE);
			if (!rn2(3)) {
				pline("The raw power goes out of control!");
				badeffect();
				u.uenmax -= rnd(5);
				if (u.uen > u.uenmax) u.uen = u.uenmax;

				if (!rn2(10)) NastinessProblem += rnd(1000);
			}
		}

		break;

	case SPE_POLYFORM:

		do {
			u.wormpolymorph = rn2(NUMMONS);
		} while(( (notake(&mons[u.wormpolymorph]) && rn2(4) ) || ((mons[u.wormpolymorph].mlet == S_BAT) && rn2(2)) || ((mons[u.wormpolymorph].mlet == S_EYE) && rn2(2) ) || ((mons[u.wormpolymorph].mmove == 1) && rn2(4) ) || ((mons[u.wormpolymorph].mmove == 2) && rn2(3) ) || ((mons[u.wormpolymorph].mmove == 3) && rn2(2) ) || ((mons[u.wormpolymorph].mmove == 4) && !rn2(3) ) || ( (mons[u.wormpolymorph].mlevel < 10) && ((mons[u.wormpolymorph].mlevel + 1) < rnd(u.ulevel)) ) || (!haseyes(&mons[u.wormpolymorph]) && rn2(2) ) || ( is_nonmoving(&mons[u.wormpolymorph]) && rn2(5) ) || ( is_eel(&mons[u.wormpolymorph]) && rn2(5) ) || ( is_nonmoving(&mons[u.wormpolymorph]) && rn2(20) ) || ( uncommon2(&mons[u.wormpolymorph]) && !rn2(4) ) || ( uncommon3(&mons[u.wormpolymorph]) && !rn2(3) ) || ( uncommon5(&mons[u.wormpolymorph]) && !rn2(2) ) || ( uncommon7(&mons[u.wormpolymorph]) && rn2(3) ) || ( uncommon10(&mons[u.wormpolymorph]) && rn2(5) ) || ( is_eel(&mons[u.wormpolymorph]) && rn2(20) ) ) );

		pline("You feel polyform.");
		polyself(FALSE);
		if (!rn2(10)) {
			pline("The magical energy goes out of control!");
			badeffect();
			if (!rn2(3)) {
				badeffect();
				if (!rn2(3)) {
					badeffect();
					if (!rn2(3)) {
						badeffect();
						if (!rn2(3)) {
							badeffect();
						}
					}
				}
			}
		}
		if (u.mtimedone && !rn2(2) && Upolyd && mons[u.umonnum].mlevel) {
			u.mtimedone /= mons[u.umonnum].mlevel;
			if (u.mtimedone < 5) u.mtimedone = 5;
		}

		break;

	case SPE_FIRE_GOLEM:
		{
			register struct obj *golemfuel;
			register struct monst *firegolem;

			golemfuel = carrying(TORCH);

			if (!golemfuel) {
				pline("You need a torch to create the golem from!");
				break;
			}

			if (golemfuel) useup(golemfuel);

			firegolem = make_helper(PM_SUMMONED_FIRE_GOLEM, u.ux, u.uy);
			if (!firegolem) break;
			firegolem->mtame = 5;
			firegolem->isspell = firegolem->uexp = TRUE;

		}

		break;

	case SPE_TOTEM_SUMMONING:
		{
			register struct obj *poss;
			register struct monst *posmon;

			if (!Upolyd && u.uhp < 101) {
				pline("Totem summoning requires you to have at least 101 HP.");
				break;
			}

			if (Upolyd && u.mh < 101) {
				pline("Totem summoning requires you to have at least 101 HP.");
				break;
			}

            	poss = getobj((const char *)revivables, "revive");
            	if (!poss) {
				pline("Your totem summoning attempt fails.");
				break;
			}
            	if (poss->otyp != CORPSE) {
				pline("That cannot be revived.");
				break;
			}
			if (obj_resists(poss, 0, 50)) {
				pline("Your attempt to summon such a powerful monster fails.");
				break;
			}
            	posmon = revive(poss);

            	if (posmon) {
			    if (Is_blackmarket(&u.uz))
				setmangry(posmon);
			    else
			    if (posmon->isshk)
				make_happy_shk(posmon, FALSE);
			    else if (!resist(posmon, SPBOOK_CLASS, 0, NOTELL))
				(void) tamedog(posmon, (struct obj *) 0, FALSE);
			}
            	if (Upolyd) u.mh -= rnd(100);
            	else u.uhp -= rnd(100);

			if (!rn2(3)) {
				pline("The summoning power goes out of control!");
				badeffect();
				create_critters(rnz(10), (struct permonst *)0);
				u.uenmax -= rnd(10);
				if (u.uen > u.uenmax) u.uen = u.uenmax;

			}
		}

		break;

	case SPE_STERILIZE:

		You_feel("an anti-sexual aura.");

		u.sterilized = 10 + (spell_damage_bonus(spellid(spell)) * 4);

		break;

	case SPE_DISRUPTION_SHIELD:

		You("activate your mana shield.");

		u.disruptionshield = 30 + (spell_damage_bonus(spellid(spell)) * 7);

		break;

	case SPE_HOLY_SHIELD:

		You("activate your holy shield.");

		u.holyshield = 20 + (spell_damage_bonus(spellid(spell)) * 3);

		break;

	case SPE_AVALANCHE:
		{
			register struct trap *avaltrap;
			avaltrap = maketrap(u.ux, u.uy, COLLAPSE_TRAP, 0);
			if (!avaltrap) pline("For some reason, the avalanche does not go off!");

			if (avaltrap && avaltrap->ttyp == COLLAPSE_TRAP) {
				dotrap(avaltrap, 0);

				pline("You are severely hurt and unable to move due to being buried!");

				u.uhpmax -= rnd(10);
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				if (u.uhp < 1) {
					u.youaredead = 1;
					killer = "an avalanche";
					killer_format = KILLED_BY;
					done(DIED);
					u.youaredead = 0;
				}
				if (Upolyd) {
					u.mhmax -= rnd(10);
					if (u.mh > u.mhmax) u.mh = u.mhmax;
					if (u.mh < 1) {
						u.youaredead = 1;
						killer = "an avalanche";
						killer_format = KILLED_BY;
						done(DIED);
						u.youaredead = 0;
					}
				}
				u.uenmax -= rnd(10);
				if (u.uenmax < 0) u.uenmax = 0;
				if (u.uen > u.uenmax) u.uen = u.uenmax;
				if (Free_action) {
				    nomul(-(rnd(5)), "buried by an avalanche", TRUE);
					nomovemsg = "You finally dig yourself out of the debris.";
				} else {
				    nomul(-(rnd(10)), "buried by an avalanche", TRUE);
					nomovemsg = "You finally dig yourself out of the debris.";
				}
			}

		}

		break;

	case SPE_DEMEMORIZE:

		if (spellid(0) == NO_SPELL)  { /* should never happen, but I put it here just in case --Amy */
			You("don't know any spells, and therefore you cannot dememorize them either.");
			break;
		}

		{
			int numspells;

			for (numspells = 0; numspells < MAXSPELL && spellid(numspells) != NO_SPELL; numspells++) {

				pline("You know the %s spell.", spellname(numspells));
				if (yn("Dememorize it?") == 'y') {

					spl_book[numspells].sp_know = 0;
					pline("Alright, the %s spell is a forgotten spell now.", spellname(numspells));

					break;
				}
			}
		}

		break;

	case SPE_INERTIA_CONTROL:

		if (spellid(0) == NO_SPELL)  {
			You("don't know any spells, and therefore inertia control fails.");
			break;
		}

		{
			int numspells;

			for (numspells = 0; numspells < MAXSPELL && spellid(numspells) != NO_SPELL; numspells++) {
				if (spellid(numspells) == SPE_INERTIA_CONTROL) continue;

				pline("You know the %s spell.", spellname(numspells));
				if (yn("Control the flow of this spell?") == 'y') {
					u.inertiacontrolspell = spellid(numspells);
					u.inertiacontrolspellno = numspells;

					u.inertiacontrol = 20;

					if (!(PlayerCannotUseSkills) && P_SKILL(P_OCCULT_SPELL) >= P_BASIC) {

						switch (P_SKILL(P_OCCULT_SPELL)) {
							case P_BASIC: u.inertiacontrol = 23; break;
							case P_SKILLED: u.inertiacontrol = 26; break;
							case P_EXPERT: u.inertiacontrol = 30; break;
							case P_MASTER: u.inertiacontrol = 33; break;
							case P_GRAND_MASTER: u.inertiacontrol = 36; break;
							case P_SUPREME_MASTER: u.inertiacontrol = 40; break;
							default: break;
						}
					}

					break;
				}
			}
		}

		break;

	case SPE_SPELLBINDER:

		if (u.spellbinder) {
			pline("Spellbinder can't be used to bind itself.");
			break;
		}

		{
			register int spellbindings = 2;

			if (!(PlayerCannotUseSkills) && P_SKILL(P_OCCULT_SPELL) >= P_SKILLED) {

				switch (P_SKILL(P_OCCULT_SPELL)) {
					case P_SKILLED: spellbindings = 3; break;
					case P_EXPERT: spellbindings = 4; break;
					case P_MASTER: spellbindings = 5; break;
					case P_GRAND_MASTER: spellbindings = 6; break;
					case P_SUPREME_MASTER: spellbindings = 7; break;
					default: break;
				}

			}

			u.spellbinder = spellbindings;
			u.spellbinder1 = -1;
			u.spellbinder2 = -1;
			u.spellbinder3 = -1;
			u.spellbinder4 = -1;
			u.spellbinder5 = -1;
			u.spellbinder6 = -1;
			u.spellbinder7 = -1;

			pline("You may cast %d more spells.", u.spellbinder);

			while (u.spellbinder) {
				docast();
				u.spellbinder--;
			}

		}

		break;

	case SPE_TRACKER:

		{
			register struct monst *nexusmon;
			boolean teleportdone = FALSE;

			if (level.flags.noteleport && !Race_if(PM_RODNEYAN) ) {
				pline("A mysterious force prevents you from teleporting!");
				break;
			}

			if ( ( (u.uhave.amulet && (u.amuletcompletelyimbued || !rn2(3))) || (uarm && uarm->oartifact == ART_CHECK_YOUR_ESCAPES) || NoReturnEffect || u.uprops[NORETURN].extrinsic || have_noreturnstone() || On_W_tower_level(&u.uz) || (u.usteed && mon_has_amulet(u.usteed)) ) ) {
				You_feel("disoriented for a moment.");
				break;
			}

			for(nexusmon = fmon; nexusmon; nexusmon->nmon) {
				if (nexusmon) break;
			}

			if (!nexusmon) {
				pline("No valid target for tracking. Sorry.");
				break;
			}

			int i, j, bd = 1;

			for(i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
				if (!isok(nexusmon->mx + i, nexusmon->my + j)) continue;

				if (teleok(nexusmon->mx + i, nexusmon->my + j, FALSE)) {
					teleportdone = TRUE;
					teleds(nexusmon->mx + i, nexusmon->my + j, FALSE);
					if (!rn2(10)) {
						pline("The spell backlashes!");
						badeffect();
					}
					break;
				}
			}
			if (!teleportdone) pline("Tracking target is unreachable. Sorry.");

		}

		break;

	case SPE_GEOLYSIS:

		pline("You sacrifice some of your %s and become able to eat through solid rock.", body_part(BLOOD));
		u.uhpmax -= rnd(4);
		if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		if (u.uhp < 1) {
			u.youaredead = 1;
			killer = "invoking geolysis with too little health";
			killer_format = KILLED_BY;
			done(DIED);
			u.youaredead = 0;
		}
		if (Upolyd) {
			u.mhmax -= rnd(4);
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		}

		/* re-casting it will restart the countdown rather than add to the duration --Amy */
		u.geolysis = 15 + (spell_damage_bonus(spellid(spell)) * 5);

		break;

	case SPE_DRIPPING_TREAD:

		pline("You sacrifice some of your %s and start dripping elements.", body_part(BLOOD));
		u.uhpmax -= rnd(15);
		if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		if (u.uhp < 1) {
			u.youaredead = 1;
			killer = "dripping the elements with too little health";
			killer_format = KILLED_BY;
			done(DIED);
			u.youaredead = 0;
		}
		if (Upolyd) {
			u.mhmax -= rnd(15);
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		}
		u.drippingtreadtype = rnd(4);
		/* 1 = water, 2 = lava, 3 = ice, 4 = clouds */

		if (!(PlayerCannotUseSkills) && P_SKILL(P_ELEMENTAL_SPELL) >= P_SKILLED) {
			pline("Choose a terrain type to generate. You can create water, lava, ice or clouds.");
			if (yn("Generate water?") == 'y') {
				u.drippingtreadtype = 1;
			}
			else if (yn("Generate lava?") == 'y') {
				u.drippingtreadtype = 2;
			}
			else if (yn("Generate ice?") == 'y') {
				u.drippingtreadtype = 3;
			}
			else {
				u.drippingtreadtype = 4;
			}

		}

		if (u.drippingtreadtype == 1) pline("Generating water.");
		if (u.drippingtreadtype == 2) pline("Generating lava.");
		if (u.drippingtreadtype == 3) pline("Generating ice.");
		if (u.drippingtreadtype == 4) pline("Generating clouds.");

		/* re-casting it will restart the countdown rather than add to the duration --Amy */
		u.drippingtread = 20 + (spell_damage_bonus(spellid(spell)) * 3);

		break;

	case SPE_POISON_BRAND:

		if (!uwep) {
			pline("You are not holding a weapon!");
			break;
		}
		if (uwep && !is_poisonable(uwep)) {
			pline("Your weapon cannot be poisoned!");
			break;
		}
		if (uwep) {
			if (!stack_too_big(uwep)) {
				uwep->opoisoned = TRUE;
				pline("Your weapon was poisoned.");
			} else pline("Unfortunately your wielded stack of weapons was too big, and so the poisoning failed.");

			pline("The poison spills over you!");

			if (!rn2(Poison_resistance ? 5 : 3)) {
				int typ = rn2(A_MAX);
				poisontell(typ);
				(void) adjattrib(typ, Poison_resistance ? -1 : -rn1(4,3), TRUE);
			}
			if (!Poison_resistance) {
				losehp(rnd(10), "poisoning a weapon", KILLED_BY);
			}

		}

		break;

	case SPE_STEAM_VENOM:

		if (Underwater)
			pline_The("water around you vaporizes violently!");
		else {
		    pline_The("air around you explodes in a cloud of noxious gas!");
		}
		explode(u.ux, u.uy, ZT_SPELL(ZT_POISON_GAS), 24 + (spell_damage_bonus(spellid(spell)) * 2), SCROLL_CLASS, EXPL_NOXIOUS);

		break;

	case SPE_CREATE_FAMILIAR:
		if (!rn2(5)) (void) make_familiar((struct obj *)0, u.ux, u.uy, FALSE);
		else if (!rn2(2)) {
			pline("The summoned monster does not seem to be friendly!");
			(void) makemon((struct permonst *)0, u.ux, u.uy, MM_NOSPECIALS);
		} else if (rn2(4)) {
			pline("The spell fizzled out!");
		} else {
			pline("The spell backfired!");
			badeffect();
		}
		break;
	case SPE_CLAIRVOYANCE:
		if (!BClairvoyant)
		    do_vicinity_mapX();
		/* at present, only one thing blocks clairvoyance */
		else if (uarmh && uarmh->otyp == CORNUTHAUM)
		    You("sense a pointy hat on top of your %s.",
			body_part(HEAD));
		break;
	case SPE_PROTECTION:
		cast_protection();
		break;
	case SPE_JUMPING:
		if (!jump(max(role_skill,1)))
			pline(nothing_happens);
		break;
	case SPE_RESIST_POISON:
		if(!(HPoison_resistance & INTRINSIC)) {
			You_feel("healthy ..... for the moment at least.");
			incr_itimeout(&HPoison_resistance, Poison_resistance ? (rnd(10) + spell_damage_bonus(spellid(spell))) : (rn1(100, 50) + spell_damage_bonus(spellid(spell))*10));
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_ANTI_DISINTEGRATION:
		if(!(HDisint_resistance & INTRINSIC)) {
			You_feel("quite firm for a while.");
			incr_itimeout(&HDisint_resistance, Disint_resistance ? (rnd(10) + spell_damage_bonus(spellid(spell))) : (rn1(100, 50) + spell_damage_bonus(spellid(spell))*10));
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_MAGICTORCH:
		if(!(HSight_bonus & INTRINSIC)) {
			You("can see in the dark!");
			incr_itimeout(&HSight_bonus, rn1(20, 10) +
				spell_damage_bonus(spellid(spell))*20);
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_DISPLACEMENT:
		if(!(HDisplaced & INTRINSIC)) {
			pline("Your image is displaced!");
			incr_itimeout(&HDisplaced, Displaced ? (rnd(5) + spell_damage_bonus(spellid(spell))) : (rn1(200, 100) + spell_damage_bonus(spellid(spell))*20));
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_TRUE_SIGHT:
		if(!(HSee_invisible & INTRINSIC)) {
			pline("You can see invisible things!");
			incr_itimeout(&HSee_invisible, See_invisible ? (rnd(5) + spell_damage_bonus(spellid(spell))) : (rn1(50, 25) + spell_damage_bonus(spellid(spell))*5));
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_CONFLICT:
		if(!(HConflict & INTRINSIC)) {
			pline("You start generating conflict!");
			incr_itimeout(&HConflict, Conflict ? (rnd(5) + spell_damage_bonus(spellid(spell))) : (rn1(20, 10) + spell_damage_bonus(spellid(spell))*3));
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_ESP:
		if(!(HTelepat & INTRINSIC)) {
			You_feel("a strange mental acuity.");
			incr_itimeout(&HTelepat, Blind_telepat ? (rnd(5) + spell_damage_bonus(spellid(spell))) : (rn1(100, 50) + spell_damage_bonus(spellid(spell))*10));
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_RADAR:
		if(!(HWarning & INTRINSIC)) {
			pline("You turn on your radar.");
			incr_itimeout(&HWarning, Warning ? (rnd(5) + spell_damage_bonus(spellid(spell))) : (rn1(100, 50) + spell_damage_bonus(spellid(spell))*10));
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_REGENERATION:
		if(!(HRegeneration & INTRINSIC)) {
			pline("You direct your internal energy to closing your wounds.");
			incr_itimeout(&HRegeneration, Regeneration ? (rnd(5) + spell_damage_bonus(spellid(spell))) : (rn1(100, 50) + spell_damage_bonus(spellid(spell))*10));
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_SEARCHING:
		if(!(HSearching & INTRINSIC)) {
			pline("You start searching.");
			incr_itimeout(&HSearching, Searching ? (rnd(5) + spell_damage_bonus(spellid(spell))) : (rn1(100, 50) + spell_damage_bonus(spellid(spell))*10));
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_FREE_ACTION:
		if(!(HFree_action & INTRINSIC)) {
			pline("You are resistant to paralysis.");
			incr_itimeout(&HFree_action, Free_action ? (rnd(5) + spell_damage_bonus(spellid(spell))) : (rn1(100, 50) + spell_damage_bonus(spellid(spell))*10));
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_STEALTH:
		if(!(HStealth & INTRINSIC)) {
			pline("You start moving silently.");
			incr_itimeout(&HStealth, Stealth ? (rnd(5) + spell_damage_bonus(spellid(spell))) : (rn1(100, 50) + spell_damage_bonus(spellid(spell))*10));
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_INFRAVISION:
		if(!(HInfravision & INTRINSIC)) {
			pline("Your %s are suddenly very sensitive!", makeplural(body_part(EYE)));
			incr_itimeout(&HInfravision, Infravision ? (rnd(5) + spell_damage_bonus(spellid(spell))) : (rn1(100, 50) + spell_damage_bonus(spellid(spell))*10));
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_BOTOX_RESIST:
		if(!(HSick_resistance & INTRINSIC)) {
			You_feel("resistant to sickness.");
			incr_itimeout(&HSick_resistance, Sick_resistance ? (rnd(10) + spell_damage_bonus(spellid(spell))) : (rn1(100, 50) + spell_damage_bonus(spellid(spell))*10));
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_DRAGON_BLOOD:
		if(!(HDrain_resistance & INTRINSIC)) {
			You_feel("resistant to level drainage.");
			incr_itimeout(&HDrain_resistance, Drain_resistance ? (rnd(10) + spell_damage_bonus(spellid(spell))) : (rn1(100, 50) + spell_damage_bonus(spellid(spell))*10));
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_ANTI_MAGIC_FIELD:
		if(!(HAntimagic & INTRINSIC)) {
			You_feel("resistant to magic.");
			incr_itimeout(&HAntimagic, Antimagic ? (rnd(10) + spell_damage_bonus(spellid(spell))) : (rn1(100, 50) + spell_damage_bonus(spellid(spell))*10));
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;

	case SPE_NO_EFFECT:

		pline(nothing_happens);

		break;

	case SPE_CURE_WOUNDED_LEGS:

		if (Wounded_legs) pline("A warm glow spreads through your %s!", makeplural(body_part(LEG)));
		HWounded_legs = EWounded_legs = 0;

		break;

	case SPE_CURE_GLIB:

		if (Glib) pline("You clean your %s.", makeplural(body_part(HAND)));
		Glib = 0;

		break;

	case SPE_CUTTING:

		switch (rnd(7)) {

			case 1: pline("You put the knife to your lower %s and cut...", body_part(ARM)); break;
			case 2: pline("You use a sharp object to cut open your belly..."); break;
			case 3: pline("You slide your body along a rough surface and sustain terrible skin rashes."); break;
			case 4: pline("You rip your butt open with a metallic edge."); break;
			case 5: pline("You scratch up and down your %s with a sexy leather pump until it starts bleeding.", body_part(LEG)); break;
			case 6: pline("You slit your %s full length with a sharp-edged zipper.", body_part(LEG)); break;
			case 7: pline("You prick yourself with a needle."); break;
		}

		if (!rn2(20)) losehp(d(10,8), "cutting", KILLED_BY);
		else if (!rn2(5)) losehp(d(6,8), "cutting", KILLED_BY);
		else losehp(d(4,6), "cutting", KILLED_BY);

		break;

	case SPE_UNLEVITATE:

		if (Levitation) pline("You try to unlevitate.");
		ELevitation &= ~W_ARTI;
		HLevitation &= ~(I_SPECIAL|TIMEOUT);

		break;

	case SPE_WORLD_FALL:

		You("scream \"EYGOORTS-TOGAAL, JEZEHH!\"");
		{
		int num;
	      register struct monst *wfm, *wfm2;
		num = 0;
		for (wfm = fmon; wfm; wfm = wfm2) {
			wfm2 = wfm->nmon;
			if ( ((wfm->m_lev < u.ulevel) || (!rn2(4) && wfm->m_lev < (2 * u.ulevel))) && wfm->mnum != quest_info(MS_NEMESIS) && !(wfm->data->geno & G_UNIQ) ) {
				mondead(wfm);
				num++;
			}
	      }
		pline("Eliminated %d monster%s.", num, plur(num));
		}

		pline("Casting such a powerful spell taxes your maximum health, and also causes massive backlash...");

		u.uhpmax -= rnd(25);
		if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		if (u.uhp < 1) {
			u.youaredead = 1;
			killer = "invoking world fall with too little health";
			killer_format = KILLED_BY;
			done(DIED);
			u.youaredead = 0;
		}
		if (Upolyd) {
			u.mhmax -= rnd(25);
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		}

		badeffect();
		badeffect();
		badeffect();
		badeffect();
		badeffect();
		badeffect();
		badeffect();
		NastinessProblem += rnz(1000 * (monster_difficulty() + 1));

		break;

	case SPE_GENOCIDE:

		if (role_skill >= P_SUPREME_MASTER) n = 13;
		else if (role_skill >= P_GRAND_MASTER) n = 15;
		else if (role_skill >= P_MASTER) n = 16;
		else if (role_skill >= P_EXPERT) n = 18;
		else if (role_skill >= P_SKILLED) n = 20;
		else if (role_skill >= P_BASIC) n = 22;
		else n = 25;	/* Unskilled or restricted */
		if (!rn2(n)) {
			do_genocide(1);	/* REALLY, see do_genocide() */
		} else
		    Your("genocide failed!");
		break;

	case SPE_GAIN_LEVEL:

		if (role_skill >= P_SUPREME_MASTER) n = 38;
		else if (role_skill >= P_GRAND_MASTER) n = 40;
		else if (role_skill >= P_MASTER) n = 42;
		else if (role_skill >= P_EXPERT) n = 44;
		else if (role_skill >= P_SKILLED) n = 46;
		else if (role_skill >= P_BASIC) n = 48;
		else n = 50;	/* Unskilled or restricted */
		if (!rn2(n)) {
			pluslvl(FALSE);
		} else
		    pline("Too bad - it didn't work!");
		break;

	case SPE_DETECT_WATER:

		water_detectX();

		break;

	case SPE_SATISFY_HUNGER:

		pline("Your stomach is filled a bit.");
		lesshungry(100);

		break;

	case SPE_BACKFIRE:

		badeffect();

		break;

	case SPE_MAP_LEVEL:

		if (role_skill >= P_SUPREME_MASTER) n = 4;
		else if (role_skill >= P_GRAND_MASTER) n = 5;
		else if (role_skill >= P_MASTER) n = 6;
		else if (role_skill >= P_EXPERT) n = 7;
		else if (role_skill >= P_SKILLED) n = 8;
		else if (role_skill >= P_BASIC) n = 9;
		else n = 10;	/* Unskilled or restricted */
		if (!rn2(n)) {
		    struct trap *t;
		    long save_Hconf = HConfusion,
			 save_Hhallu = HHallucination;
	
		    HConfusion = HHallucination = 0L;
		    for (t = ftrap; t != 0; t = t->ntrap) {
			if (!rn2(15)) continue;
			if (!t->hiddentrap) t->tseen = 1;
			map_trap(t, TRUE);
		    }
		    do_mappingY();
		    HConfusion = save_Hconf;
		    HHallucination = save_Hhallu;
		    You_feel("knowledgable!");
		    object_detect(pseudo, 0);
		} else
		    pline("The map refuses to reveal its secrets.");
		break;

	case SPE_GODMODE:
		if (rn2(3)) incr_itimeout(&Invulnerable, rnd(5) );
		else incr_itimeout(&Invulnerable, rnd(5 + spell_damage_bonus(spellid(spell)) ) );
		You_feel("invincible!");
		break;
	case SPE_ACIDSHIELD:
		if(!(HAcid_resistance & INTRINSIC)) {
			You("are resistant to acid now. Your items, however, are not.");
			incr_itimeout(&HAcid_resistance, Acid_resistance ? (rnd(10) + spell_damage_bonus(spellid(spell))) : (rn1(100, 50) + spell_damage_bonus(spellid(spell))*10));
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_RESIST_PETRIFICATION:
		if(!(HStone_resistance & INTRINSIC)) {
			You_feel("more limber. Let's eat some cockatrice meat!");
			incr_itimeout(&HStone_resistance, Stone_resistance ? (rnd(5) + spell_damage_bonus(spellid(spell))) : (rn1(40, 20) + spell_damage_bonus(spellid(spell))*4));
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_RESIST_SLEEP:
		if(!(HSleep_resistance & INTRINSIC)) {
			if (Hallucination)
				pline("Too much coffee!");
			else
				You("no longer feel tired.");
			incr_itimeout(&HSleep_resistance, Sleep_resistance ? (rnd(10) + spell_damage_bonus(spellid(spell))) : (rn1(100, 50) + spell_damage_bonus(spellid(spell))*10));
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_FLYING:
		if(!(HFlying & INTRINSIC)) {
			You("start flying!");
			incr_itimeout(&HFlying, Flying ? (rnd(4) + spell_damage_bonus(spellid(spell))) : (rn1(20, 25) + spell_damage_bonus(spellid(spell))*20));
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_ENDURE_COLD:
		if(!(HCold_resistance & INTRINSIC)) {
			You_feel("warmer.");
			incr_itimeout(&HCold_resistance, Cold_resistance ? (rnd(10) + spell_damage_bonus(spellid(spell))) : (rn1(100, 50) + spell_damage_bonus(spellid(spell))*10));
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_ENDURE_HEAT:
		if(!(HFire_resistance & INTRINSIC)) {
			if (Hallucination)
				pline("Excellent! You feel, like, totally cool!");
			else
				You_feel("colder.");
			incr_itimeout(&HFire_resistance, Fire_resistance ? (rnd(10) + spell_damage_bonus(spellid(spell))) : (rn1(100, 50) + spell_damage_bonus(spellid(spell))*10));
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_INSULATE:
		if(!(HShock_resistance & INTRINSIC)) {
			if (Hallucination)
				pline("Bummer! You've been grounded!");
			else
				You("are not at all shocked by this feeling.");
			incr_itimeout(&HShock_resistance, Shock_resistance ? (rnd(10) + spell_damage_bonus(spellid(spell))) : (rn1(100, 50) + spell_damage_bonus(spellid(spell))*10));
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;

	case SPE_HOLD_AIR:
		if(!(HMagical_breathing & INTRINSIC)) {
			You("hold your breath.");
			incr_itimeout(&HMagical_breathing, Amphibious ? (rnd(5) + spell_damage_bonus(spellid(spell))) : (rn1(40, 20) + spell_damage_bonus(spellid(spell))*10));
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;

	case SPE_SWIMMING:
		if(!(HSwimming & INTRINSIC)) {
			You("grow water wings.");
			incr_itimeout(&HSwimming, Amphibious ? (rnd(10) + spell_damage_bonus(spellid(spell))) : (rn1(100, 50) + spell_damage_bonus(spellid(spell))*10));
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;

	case SPE_RESIST_RANDOM_ELEMENT:
		switch (rnd(9)) {
			case 1:
				if(!(HShock_resistance & INTRINSIC)) {
					if (Hallucination)
						pline("Bummer! You've been grounded!");
					else
						You("are not at all shocked by this feeling.");
					incr_itimeout(&HShock_resistance, Shock_resistance ? (rnd(10) + spell_damage_bonus(spellid(spell))) : (rn1(100, 50) + spell_damage_bonus(spellid(spell))*10));
				} else pline(nothing_happens);	/* Already have as intrinsic */
				break;
			case 2:
				if(!(HFire_resistance & INTRINSIC)) {
					if (Hallucination)
						pline("Excellent! You feel, like, totally cool!");
					else
						You_feel("colder.");
					incr_itimeout(&HFire_resistance, Fire_resistance ? (rnd(10) + spell_damage_bonus(spellid(spell))) : (rn1(100, 50) + spell_damage_bonus(spellid(spell))*10));
				} else pline(nothing_happens);	/* Already have as intrinsic */
				break;
			case 3:
				if(!(HCold_resistance & INTRINSIC)) {
					You_feel("warmer.");
					incr_itimeout(&HCold_resistance, Cold_resistance ? (rnd(10) + spell_damage_bonus(spellid(spell))) : (rn1(100, 50) + spell_damage_bonus(spellid(spell))*10));
				} else pline(nothing_happens);	/* Already have as intrinsic */
				break;
			case 4:
				if(!(HSleep_resistance & INTRINSIC)) {
					if (Hallucination)
						pline("Too much coffee!");
					else
						You("no longer feel tired.");
					incr_itimeout(&HSleep_resistance, Sleep_resistance ? (rnd(10) + spell_damage_bonus(spellid(spell))) : (rn1(100, 50) + spell_damage_bonus(spellid(spell))*10));
				} else pline(nothing_happens);	/* Already have as intrinsic */
				break;
			case 5:
				if(!(HStone_resistance & INTRINSIC)) {
					You_feel("more limber. Let's eat some cockatrice meat!");
					incr_itimeout(&HStone_resistance, Stone_resistance ? (rnd(5) + spell_damage_bonus(spellid(spell))) : (rn1(40, 20) + spell_damage_bonus(spellid(spell))*4));
				} else pline(nothing_happens);	/* Already have as intrinsic */
				break;
			case 6:
				if(!(HAcid_resistance & INTRINSIC)) {
					You("are resistant to acid now. Your items, however, are not.");
					incr_itimeout(&HAcid_resistance, Acid_resistance ? (rnd(10) + spell_damage_bonus(spellid(spell))) : (rn1(100, 50) + spell_damage_bonus(spellid(spell))*10));
				} else pline(nothing_happens);	/* Already have as intrinsic */
				break;
			case 7:
				if(!(HSick_resistance & INTRINSIC)) {
					You_feel("resistant to sickness.");
					incr_itimeout(&HSick_resistance, Sick_resistance ? (rnd(10) + spell_damage_bonus(spellid(spell))) : (rn1(100, 50) + spell_damage_bonus(spellid(spell))*10));
				} else pline(nothing_happens);	/* Already have as intrinsic */
				break;
			case 8:
				if(!(HPoison_resistance & INTRINSIC)) {
					You_feel("healthy ..... for the moment at least.");
					incr_itimeout(&HPoison_resistance, Poison_resistance ? (rnd(10) + spell_damage_bonus(spellid(spell))) : (rn1(100, 50) + spell_damage_bonus(spellid(spell))*10));
				} else pline(nothing_happens);	/* Already have as intrinsic */
				break;
			case 9:
				if(!(HDisint_resistance & INTRINSIC)) {
					You_feel("quite firm for a while.");
					incr_itimeout(&HDisint_resistance, Disint_resistance ? (rnd(10) + spell_damage_bonus(spellid(spell))) : (rn1(100, 50) + spell_damage_bonus(spellid(spell))*10));
				} else pline(nothing_happens);	/* Already have as intrinsic */
				break;
		}

		break;

	case SPE_FORBIDDEN_KNOWLEDGE:
		if(!(HHalf_spell_damage & INTRINSIC)) {
			if (Hallucination)
				pline("Let the casting commence!");
			else
				You_feel("a sense of spell knowledge.");
			incr_itimeout(&HHalf_spell_damage, rn1(100, 50) +
				spell_damage_bonus(spellid(spell))*10);
		}
		if(!(HHalf_physical_damage & INTRINSIC)) {
			if (Hallucination)
				You_feel("like a tough motherfucker!");
			else
				You("are resistant to normal damage.");
			incr_itimeout(&HHalf_physical_damage, rn1(100, 50) +
				spell_damage_bonus(spellid(spell))*10);
		}
		u.ugangr++;

		break;

	case SPE_ALTER_REALITY:

		alter_reality();

		break;

	case SPE_ENLIGHTEN: 
		You_feel("self-knowledgeable...");
		display_nhwindow(WIN_MESSAGE, FALSE);
		enlightenment(FALSE, FALSE);
		pline("The feeling subsides.");
		exercise(A_WIS, TRUE);
		break;

	/* WAC -- new spells */
	case SPE_FLAME_SPHERE:
	case SPE_FREEZE_SPHERE:
	{	register int cnt = 1;
		struct monst *mtmp;


		if (rn2(3) && role_skill >= P_SKILLED) cnt += rnd(role_skill - P_BASIC);
		while(cnt--) {
			mtmp = make_helper((pseudo->otyp == SPE_FLAME_SPHERE) ?
					PM_FLAMING_SPHERE : PM_FREEZING_SPHERE, u.ux, u.uy);
			if (!mtmp) continue;
			mtmp->mtame = 10;
			mtmp->mhpmax = mtmp->mhp = 1;
			mtmp->isspell = mtmp->uexp = TRUE;
		} /* end while... */
		break;
	}

	case SPE_SHOCKING_SPHERE:
	{	register int cnt = 1;
		struct monst *mtmp;


		if (rn2(3) && role_skill >= P_SKILLED) cnt += rnd(role_skill - P_BASIC);
		while(cnt--) {
			mtmp = make_helper(PM_SHOCKING_SPHERE, u.ux, u.uy);
			if (!mtmp) continue;
			mtmp->mtame = 10;
			mtmp->mhpmax = mtmp->mhp = 1;
			mtmp->isspell = mtmp->uexp = TRUE;
		} /* end while... */
		break;
	}

	case SPE_ACID_SPHERE:
	{	register int cnt = 1;
		struct monst *mtmp;


		if (rn2(3) && role_skill >= P_SKILLED) cnt += rnd(role_skill - P_BASIC);
		while(cnt--) {
			mtmp = make_helper(PM_ACID_SPHERE, u.ux, u.uy);
			if (!mtmp) continue;
			mtmp->mtame = 10;
			mtmp->mhpmax = mtmp->mhp = 1;
			mtmp->isspell = mtmp->uexp = TRUE;
		} /* end while... */
		break;
	}


	/* KMH -- new spells */
	case SPE_PASSWALL:
		if (!Passes_walls)
			You_feel("ethereal.");
		incr_itimeout(&HPasses_walls, rn1(10, 5));
		break;

	case SPE_DETECT_FOOT:

		pline("Your nose tingles, and you smell feet!");
		if (nolimbs(youmonst.data) || slithy(youmonst.data)) {
			pline("You aren't sure where the feet might be, though.");
		} else {
			pline("You see here a %s on the end of your %s.",body_part(FOOT),body_part(LEG));
		/* Come on sporkhack devteam, this spell could be useful. Let's see if I can make it do something. --Amy */
			pline("Urgh - your head spins from the vile stench!");
		    make_confused(HConfusion + d(10,10), FALSE);
			turn_allmonsters(); /* This even works on Demogorgon. */
		}
		break;
	case SPE_REFLECTION:
		cast_reflection();
		break;
	case SPE_REPAIR_ARMOR:
		/* removes one level of erosion (both types) for a random piece of armor */
		otmp = getobj(all_count, "magically enchant");
		/*otmp = some_armor(&youmonst);*/
		if (otmp) {
			if (!(otmp->owornmask & W_ARMOR) ) { /* bug discovered by Heliokopis - did Sporkhack never fix this? */
	
				pline("You have a feeling of loss.");
			} else if (greatest_erosion(otmp) > 0) {
				if (!Blind) {
					pline("Your %s glows faintly golden for a moment.",xname(otmp));
				}
				if (otmp->oeroded > 0) { otmp->oeroded--; }
				if (otmp->oeroded2 > 0) { otmp->oeroded2--; }
			} else {
				if (!Blind) {
					pline("Your %s glows briefly, but looks as new as ever.",xname(otmp));
				}
			}
		} else {
			/* the player can probably feel this, so no need for a !Blind check :) */
			pline("Your embarrassing skin rash clears up slightly.");
		}
		break;

	case SPE_REROLL_ARTIFACT:

		pline("You may choose an artifact in your inventory to reroll. It may not be a worn one though.");
		otmp = getobj(all_count, "reroll");
		if (!otmp) {
			pline("You decide not to reroll anything.");
			break;
		}
		if (!otmp->oartifact) {
			pline("That is not an artifact, and can therefore not be rerolled!");
			break;
		}
		if (otmp->owornmask) {
			pline("You cannot reroll an artifact that you're wearing!");
			break;
		}
		if (!(otmp->oclass == WEAPON_CLASS || otmp->oclass == ARMOR_CLASS || otmp->oclass == RING_CLASS || otmp->oclass == AMULET_CLASS || otmp->oclass == IMPLANT_CLASS)) {
			pline("You can only reroll weapons, armors, rings, implants or amulets!");
			break;
		}

		switch (otmp->oclass) {
			case WEAPON_CLASS:

				{

					int wpntype; /* 1 = launcher, 2 = ammo, 3 = melee */
					if (is_launcher(otmp)) wpntype = 1;
					else if (is_ammo(otmp) || is_missile(otmp)) wpntype = 2;
					else wpntype = 3;
reroll:
					otmp->otyp = rnd_class(ORCISH_DAGGER,HAWAIIAN_SHIRT-1);
					if (wpntype == 1 && !is_launcher(otmp)) goto reroll;
					if (wpntype == 2 && !is_ammo(otmp) && !is_missile(otmp)) goto reroll;
					if (wpntype == 3 && (is_launcher(otmp) || is_ammo(otmp) || is_missile(otmp))) goto reroll;
				}

				break;
			case ARMOR_CLASS:

				{

					int armortype;
					/* 1 = shield, 2 = helmet, 3 = boots, 4 = gloves, 5 = cloak, 6 = shirt, 7 = suit */
					if (is_shield(otmp)) armortype = 1;
					else if (is_helmet(otmp)) armortype = 2;
					else if (is_boots(otmp)) armortype = 3;
					else if (is_gloves(otmp)) armortype = 4;
					else if (is_cloak(otmp)) armortype = 5;
					else if (is_shirt(otmp)) armortype = 6;
					else armortype = 7;
rerollX:
					otmp->otyp = rnd_class(HAWAIIAN_SHIRT,LEVITATION_BOOTS);
					if (armortype == 1 && !is_shield(otmp)) goto rerollX;
					if (armortype == 2 && !is_helmet(otmp)) goto rerollX;
					if (armortype == 3 && !is_boots(otmp)) goto rerollX;
					if (armortype == 4 && !is_gloves(otmp)) goto rerollX;
					if (armortype == 5 && !is_cloak(otmp)) goto rerollX;
					if (armortype == 6 && !is_shirt(otmp)) goto rerollX;
					if (armortype == 7 && !is_suit(otmp)) goto rerollX;

				}
				break;
			case RING_CLASS:
				otmp->otyp = rnd_class(RIN_ADORNMENT,RIN_TELEPORT_CONTROL);
				break;
			case AMULET_CLASS:
				otmp->otyp = rnd_class(AMULET_OF_CHANGE,AMULET_OF_VULNERABILITY);
				break;
			case IMPLANT_CLASS:
				otmp->otyp = rnd_class(IMPLANT_OF_ABSORPTION,IMPLANT_OF_FREEDOM);
				break;
		}
		pline("Your artifact was rerolled to another base item!");

		pline("The strain of casting such a powerful spell damages your maximum health and mana.");

		u.uhpmax -= rnd(10);
		if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		if (u.uhp < 1) {
			u.youaredead = 1;
			killer = "the strain of casting reroll artifact";
			killer_format = KILLED_BY;
			done(DIED);
			u.youaredead = 0;
		}
		if (Upolyd) {
			u.mhmax -= rnd(10);
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		}

		u.uenmax -= rnd(10);
		if (u.uen > u.uenmax) u.uen = u.uenmax;

		break;

	case SPE_REPAIR_WEAPON:
		/* removes one level of erosion (both types) for your wielded weapon */
		if (uwep && stack_too_big(uwep)) {
			pline("The repair failed due to the stack being too big.");
			break;
		}
		if(uwep && (uwep->oclass == WEAPON_CLASS || uwep->oclass == BALL_CLASS || uwep->oclass == GEM_CLASS || uwep->oclass == CHAIN_CLASS || is_weptool(uwep))) {
			if (greatest_erosion(uwep) > 0) {
				if (!Blind) {
					pline("Your %s glows faintly golden for a moment.",xname(uwep));
				}
				if (uwep->oeroded > 0) { uwep->oeroded--; }
				if (uwep->oeroded2 > 0) { uwep->oeroded2--; }
			} else {
				if (!Blind) {
					pline("Your %s glows briefly, but looks as new as ever.",xname(uwep));
				}
			}

			break;
		} else {
			pline("You don't wield a weapon!");
		}

		break;

	case SPE_CHARACTER_RECURSION:

		if (yn("WARNING!!! This spell will ***PERMANENTLY*** transform your character into another one, and remove ALL of your items and spells. Are you SURE you really want this?") == 'y') {

			while (invent) {
				register struct obj *otmp, *otmp2;
			    for (otmp = invent; otmp; otmp = otmp2) {
			      otmp2 = otmp->nobj;

				if (evades_destruction(otmp) ) dropx(otmp);
				else {
				delete_contents(otmp);
				useup(otmp);}
			    }
			}

			for (n = 0; n < MAXSPELL && spellid(n) != NO_SPELL; n++) {
			    spellid(n) = NO_SPELL;
			}

			if (Aggravate_monster) {
				u.aggravation = 1;
				reset_rndmonst(NON_PM);
			}

			(void) makemon(mkclass(S_HUMAN,0), u.ux, u.uy, NO_MM_FLAGS);
			(void) makemon(mkclass(S_HUMANOID,0), u.ux, u.uy, NO_MM_FLAGS);
			(void) makemon(mkclass(S_DEMON,0), u.ux, u.uy, NO_MM_FLAGS);
			(void) makemon(mkclass(S_GNOME,0), u.ux, u.uy, NO_MM_FLAGS);
			(void) makemon(mkclass(S_OGRE,0), u.ux, u.uy, NO_MM_FLAGS);
			(void) makemon(mkclass(S_GIANT,0), u.ux, u.uy, NO_MM_FLAGS);
			(void) makemon(mkclass(S_KOP,0), u.ux, u.uy, NO_MM_FLAGS);
			(void) makemon(mkclass(S_ORC,0), u.ux, u.uy, NO_MM_FLAGS);

			u.aggravation = 0;

			(void) safe_teleds(FALSE);

			recursioneffect();

		}

		obfree(pseudo, (struct obj *)0);	/* now, get rid of it */
		return(1);

	default:
		/*impossible("Unknown spell %d attempted.", spell);*/
		pline("You attempted to cast a spell %d that either doesn't exist in this game, or it has been genocided.", spell);
		obfree(pseudo, (struct obj *)0);
		return(0);
	}

	/* gain skill for successful cast */
	use_skill(skill, spellev(spell));
	if (rn2(2)) use_skill(skill, spellev(spell)); /* let's make gaining spellcasting skill a little bit easier --Amy */
	if (!rn2(50)) use_skill(skill, (spellev(spell) * 10) ); /* jackpot! */

	/* WAC successful casting increases solidity of knowledge */

	if (!SpellColorCyan) {

		boostknow(spell, (Race_if(PM_DUNADAN) ? DUNADAN_CAST_BOOST : CAST_BOOST));
		if ((rnd(spellev(spell) + 5)) > 5) boostknow(spell, (Race_if(PM_DUNADAN) ? DUNADAN_CAST_BOOST : CAST_BOOST)); /* higher-level spells boost more --Amy */
		if (!rn2(52 - (spellev(spell) * 2) ) && !Race_if(PM_DUNADAN) ) { /* jackpot! */
			boostknow(spell, (CAST_BOOST * 5) );
			boostknow(spell, (CAST_BOOST * spellev(spell) ) );
		}

		if (Role_if(PM_MAHOU_SHOUJO)) boostknow(spell, (Race_if(PM_DUNADAN) ? DUNADAN_CAST_BOOST : CAST_BOOST));

	}

	if (SpellColorCyan) {
		boostknow(spell, -500);
		if (spellknow(spell) < 0) spl_book[spell].sp_know = 0;
	}

	if (pseudo && ( (pseudo->otyp == SPE_ALTER_REALITY) || ((pseudo->otyp == SPE_REBOOT) && !rn2(10)) || (pseudo->otyp == SPE_CLONE_MONSTER) ) ) {

		boostknow(spell, -(rnd(20000)));
		if (spellknow(spell) < 0) spl_book[spell].sp_know = 0;

	}

	if (pseudo && (pseudo->otyp == SPE_THRONE_GAMBLE) && !rn2(20) ) {

		boostknow(spell, -(rnd(50000)));
		if (spellknow(spell) < 0) spl_book[spell].sp_know = 0;

	}

	if (pseudo && (pseudo->otyp == SPE_REROLL_ARTIFACT) && !rn2(5) ) {

		boostknow(spell, -(rnd(50000)));
		if (spellknow(spell) < 0) spl_book[spell].sp_know = 0;

	}

	if (pseudo && (pseudo->otyp == SPE_CHARGING) && !rn2(role_skill == P_SUPREME_MASTER ? 16 : role_skill == P_GRAND_MASTER ? 15 : role_skill == P_MASTER ? 13 : role_skill == P_EXPERT ? 12 : role_skill == P_SKILLED ? 11 : 10) ) {

		boostknow(spell, -(rnd(100000)));
		if (spellknow(spell) < 0) spl_book[spell].sp_know = 0;

	}

	if (pseudo && ((pseudo->otyp == SPE_REPAIR_WEAPON) || (pseudo->otyp == SPE_REPAIR_ARMOR)) && !rn2(role_skill == P_SUPREME_MASTER ? 30 : role_skill == P_GRAND_MASTER ? 25 : role_skill == P_MASTER ? 24 : role_skill == P_EXPERT ? 23 : role_skill == P_SKILLED ? 22 : 20) ) {

		boostknow(spell, -(rnd(25000)));
		if (spellknow(spell) < 0) spl_book[spell].sp_know = 0;

	}

	if ((spell_skilltype(spellid(spell)) == P_OCCULT_SPELL) && !(uarmg && uarmg->oartifact == ART_FATHIEN_ELDER_S_SECRET_POW) ) { /* dangerous realm... */
		if (PlayerCannotUseSkills) {
			if (!rn2(5)) badeffect();
		} else switch (role_skill) {
			default:
			case P_ISRESTRICTED:
			case P_UNSKILLED:
				if (!rn2(Role_if(PM_OCCULT_MASTER) ? 12 : 5)) {
					pline("You fail to control the occult powers and are hit with backlash!");
					badeffect();
				}
				break;
			case P_BASIC:
				if (!rn2(Role_if(PM_OCCULT_MASTER) ? 14 : 6)) {
					pline("You fail to control the occult powers and are hit with backlash!");
					badeffect();
				}
				break;
			case P_SKILLED:
				if (!rn2(Role_if(PM_OCCULT_MASTER) ? 16 : 7)) {
					pline("You fail to control the occult powers and are hit with backlash!");
					badeffect();
				}
				break;
			case P_EXPERT:
				if (!rn2(Role_if(PM_OCCULT_MASTER) ? 20 : 8)) {
					pline("You fail to control the occult powers and are hit with backlash!");
					badeffect();
				}
				break;
			case P_MASTER:
				if (!rn2(Role_if(PM_OCCULT_MASTER) ? 22 : 9)) {
					pline("You fail to control the occult powers and are hit with backlash!");
					badeffect();
				}
				break;
			case P_GRAND_MASTER:
				if (!rn2(Role_if(PM_OCCULT_MASTER) ? 25 : 10)) {
					pline("You fail to control the occult powers and are hit with backlash!");
					badeffect();
				}
				break;
			case P_SUPREME_MASTER:
				if (!rn2(Role_if(PM_OCCULT_MASTER) ? 30 : 11)) {
					pline("You fail to control the occult powers and are hit with backlash!");
					badeffect();
				}
				break;
		}
	}

	/* charging is way too overpowered, let's add another "bullshit downside" --Amy */
	if (pseudo && (pseudo->otyp == SPE_CHARGING) && !rn2(role_skill == P_SUPREME_MASTER ? 16 : role_skill == P_GRAND_MASTER ? 15 : role_skill == P_MASTER ? 13 : role_skill == P_EXPERT ? 12 : role_skill == P_SKILLED ? 11 : 10) ) {

		badeffect();

	}

	obfree(pseudo, (struct obj *)0);	/* now, get rid of it */
	return(1);
}


void
losespells()
{
	boolean confused = (Confusion != 0);
	int  n, nzap, i;

	if (Keen_memory && rn2(20)) return;

	book = 0;
	for (n = 0; n < MAXSPELL && spellid(n) != NO_SPELL; n++)
		continue;
	if (n) {
		nzap = rn2(50) ? 1 : (rnd(n) + confused);
		if (nzap > n) nzap = n;
		for (i = n - nzap; i < n; i++) {
		    spellid(i) = NO_SPELL;
		    exercise(A_WIS, FALSE);	/* ouch! */
		}
	}
}


/* the '+' command -- view known spells */
int
dovspell()
{
	char qbuf[QBUFSZ];
	int splnum, othnum;
	struct spell spl_tmp;

	if (spellid(0) == NO_SPELL)
	    You("don't know any spells right now.");
	else {
	    while (dospellmenu( (SpellColorPink) ? "Your spells are pink." : (SpellColorRed) ? "Your spells are red." : (SpellColorGreen) ? "Your spells are green." : (SpellColorCyan) ? "Your spells are cyan." : (SpellColorBlue) ? "Your spells are blue." : (SpellColorWhite) ? "Your spells are white." : (SpellColorBlack) ? "Your spells are black." : (SpellColorGray) ? "Your spells are completely gray." : (SpellColorYellow ) ? "Your spells are yellow." : "Currently known spells",
			       SPELLMENU_VIEW, &splnum)) {
		sprintf(qbuf, "Reordering spells; swap '%s' with",
			(SpellLoss || u.uprops[SPELLS_LOST].extrinsic || have_spelllossstone()) ? "spell" : spellname(splnum));
		if (!dospellmenu(qbuf, splnum, &othnum)) break;

		spl_tmp = spl_book[splnum];
		spl_book[splnum] = spl_book[othnum];
		spl_book[othnum] = spl_tmp;
	    }
	}
	return 0;
}

STATIC_OVL boolean
dospellmenu(prompt, splaction, spell_no)
const char *prompt;
int splaction;	/* SPELLMENU_CAST, SPELLMENU_VIEW, or spl_book[] index */
int *spell_no;
{
	winid tmpwin;
	int i, n, how;
	char buf[BUFSZ];
	menu_item *selected;
	anything any;

	tmpwin = create_nhwindow(NHW_MENU);
	start_menu(tmpwin);
	any.a_void = 0;		/* zero out all bits */

	/*
	 * The correct spacing of the columns depends on the
	 * following that (1) the font is monospaced and (2)
	 * that selection letters are pre-pended to the given
	 * string and are of the form "a - ".
	 *
	 * To do it right would require that we implement columns
	 * in the window-ports (say via a tab character).
	 */
	if (!iflags.menu_tab_sep)
		sprintf(buf, "%-20s     Level  %-10s Fail  Memory", "    Name", " Category");
	else
		sprintf(buf, "Name\tLevel\t Category\tFail");
	if (flags.menu_style == MENU_TRADITIONAL)
		strcat(buf, iflags.menu_tab_sep ? "\tKey" : "  Key");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	if (!SpellLoss && !u.uprops[SPELLS_LOST].extrinsic && !have_spelllossstone()) {for (i = 0; i < MAXSPELL && spellid(i) != NO_SPELL; i++) {
		sprintf(buf, iflags.menu_tab_sep ?
			"%s\t%-d%s\t%s\t%-d%%" : "%-20s  %2d%s   %-10s %3d%%"
			"   %3d%%",
			spellname(i), spellev(i),
			((spellknow(i) > 1000) || SpellColorCyan) ? " " : (spellknow(i) ? "!" : "*"),
			spelltypemnemonic(spell_skilltype(spellid(i))),
			SpellColorBlack ? 0 : (100 - percent_success(i)),

	/* "Spell memory percentage no longer shows up in menu - Very little reason to have this considering the timeout is so long to begin with, and spellbooks are much more common." In Soviet Russia, people aren't a fan of games with good interfaces, and rather like it if there's as little information conveyed to the player as possible. The next things they'll remove are those obsolete things displayed on the bottom status line, like HP, AC and experience level; there's very little reason to display them either. Ah wait, they might just borrow the spell loss trap and display loss trap code from Slash'EM Extended and make their effect permanent - that gets rid of all the useless information displays at once, and players will be forever grateful! --Amy */

			SpellColorCyan ? 100 : issoviet ? 0 : (spellknow(i) * 100 + (KEEN-1)) / KEEN);
		if (flags.menu_style == MENU_TRADITIONAL)
			sprintf(eos(buf), iflags.menu_tab_sep ?
				"\t%c" : "%4c ", spellet(i) ? spellet(i) : ' ');

		any.a_int = i+1;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			 0, 0, ATR_NONE, buf,
			 (i == splaction) ? MENU_SELECTED : MENU_UNSELECTED);
	      }
	}
	else {for (i = 0; i < MAXSPELL && spellid(i) != NO_SPELL; i++)	{		
				sprintf(buf, iflags.menu_tab_sep ?
				"\t%c" : "%4c ", spellet(i) ? spellet(i) : ' ');
		any.a_int = i+1;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			 0, 0, ATR_NONE, buf,
			 (i == splaction) ? MENU_SELECTED : MENU_UNSELECTED);
	      }

	}
	end_menu(tmpwin, prompt);

	how = PICK_ONE;
	if (splaction == SPELLMENU_VIEW && spellid(1) == NO_SPELL)
	    how = PICK_NONE;	/* only one spell => nothing to swap with */
	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);
	if (n > 0) {
		*spell_no = selected[0].item.a_int - 1;
		/* menu selection for `PICK_ONE' does not
		   de-select any preselected entry */
		if (n > 1 && *spell_no == splaction)
		    *spell_no = selected[1].item.a_int - 1;
		free((void *)selected);
		/* default selection of preselected spell means that
		   user chose not to swap it with anything */
		if (*spell_no == splaction) return FALSE;
		return TRUE;
	} else if (splaction >= 0) {
	    /* explicit de-selection of preselected spell means that
	       user is still swapping but not for the current spell */
	    *spell_no = splaction;
	    return TRUE;
	}
	return FALSE;
}

#ifdef DUMP_LOG
void 
dump_spells()
{
	int i, n;
	char buf[BUFSZ];

	if (spellid(0) == NO_SPELL) {
	    dump("", "You didn't know any spells.");
	    dump("", "");
	    return;
	}
	dump("", "Spells known in the end");

	sprintf(buf, "%-20s   Level    %-10s Fail  Memory", "    Name", " Category");
	dump("  ",buf);
	for (i = 0; i < MAXSPELL && spellid(i) != NO_SPELL; i++) {
		sprintf(buf, "%c - %-20s  %2d%s   %-10s %3d%%  %3d%%",
			spellet(i), spellname(i), spellev(i),
			((spellknow(i) > 1000) || SpellColorCyan) ? " " : (spellknow(i) ? "!" : "*"),
			spelltypemnemonic(spell_skilltype(spellid(i))),
			SpellColorBlack ? 0 : (100 - percent_success(i)),
			SpellColorCyan ? 100 : issoviet ? 0 : (spellknow(i) * 100 + (KEEN-1)) / KEEN);
			/* In Soviet Russia, people don't want to know how much spell memory they have left. --Amy */
		dump("  ", buf);
	}
	dump("","");

} /* dump_spells */
#endif

/* Integer square root function without using floating point. */
STATIC_OVL int
isqrt(val)
int val;
{
    int rt = 0;
    int odd = 1;
    while(val >= odd) {
	val = val-odd;
	odd = odd+2;
	rt = rt + 1;
    }
    return rt;
}


STATIC_OVL int
percent_success(spell)
int spell;
{
	/* Intrinsic and learned ability are combined to calculate
	 * the probability of player's success at cast a given spell.
	 */
	int chance, splcaster, special, statused;
	int difficulty;
	int skill;
	int armorpenalties = 15;
	int shieldpenalties = 15;

	if (!(PlayerCannotUseSkills)) {
		switch (P_SKILL(P_BODY_ARMOR)) {
			case P_BASIC: armorpenalties = 13; break;
			case P_SKILLED: armorpenalties = 11; break;
			case P_EXPERT: armorpenalties = 9; break;
			case P_MASTER: armorpenalties = 7; break;
			case P_GRAND_MASTER: armorpenalties = 5; break;
			case P_SUPREME_MASTER: armorpenalties = 3; break;

		}
	}

	if (!(PlayerCannotUseSkills)) {
		switch (P_SKILL(P_SHIELD)) {
			case P_BASIC: shieldpenalties = 13; break;
			case P_SKILLED: shieldpenalties = 11; break;
			case P_EXPERT: shieldpenalties = 9; break;
			case P_MASTER: shieldpenalties = 7; break;
			case P_GRAND_MASTER: shieldpenalties = 5; break;
			case P_SUPREME_MASTER: shieldpenalties = 3; break;

		}
	}

	splcaster = urole.spelbase;
	special = urole.spelheal;
	statused = ACURR(urole.spelstat);

	/* Calculate armor penalties */
	if (uarm && !(uarm->otyp >= ROBE && uarm->otyp <= ROBE_OF_WEAKNESS)) 
	    splcaster += 2;

	/* Robes are body armour in SLASH'EM */
	if (uarm && is_metallic(uarm) && !is_etheritem(uarm)) {
		armorpenalties = 15;

		switch (objects[(uarm)->otyp].oc_material) {
			default: break;
			case METAL: armorpenalties = 16; break;
			case COPPER: armorpenalties = 21; break;
			case SILVER: armorpenalties = 17; break;
			case GOLD: armorpenalties = 8; break;
			case PLATINUM: armorpenalties = 18; break;
			case MITHRIL: armorpenalties = 13; break;
			case VIVA: armorpenalties = 12; break;
			case POURPOOR: armorpenalties = 20; break;
		}

		if (uwep && (weapon_type(uwep) == P_QUARTERSTAFF)) {
			armorpenalties *= 4;
			armorpenalties /= 5;
		}

		if (uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "velvet gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "barkhatnyye perchatki") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "baxmal qo'lqop") ) ) armorpenalties /= 2;

		splcaster += (urole.spelarmr * armorpenalties / 12);
	}
	if (uarmc && is_metallic(uarmc) && !is_etheritem(uarmc)) {
		armorpenalties = 15;

		switch (objects[(uarmc)->otyp].oc_material) {
			default: break;
			case METAL: armorpenalties = 16; break;
			case COPPER: armorpenalties = 21; break;
			case SILVER: armorpenalties = 17; break;
			case GOLD: armorpenalties = 8; break;
			case PLATINUM: armorpenalties = 18; break;
			case MITHRIL: armorpenalties = 13; break;
			case VIVA: armorpenalties = 12; break;
			case POURPOOR: armorpenalties = 20; break;
		}

		if (uwep && (weapon_type(uwep) == P_QUARTERSTAFF)) {
			armorpenalties *= 4;
			armorpenalties /= 5;
		}

		if (uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "velvet gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "barkhatnyye perchatki") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "baxmal qo'lqop") ) ) armorpenalties /= 2;

		splcaster += (urole.spelarmr * armorpenalties / 36);
	}
	if (uarmu && is_metallic(uarmu) && !is_etheritem(uarmu)) {
		armorpenalties = 15;

		switch (objects[(uarmu)->otyp].oc_material) {
			default: break;
			case METAL: armorpenalties = 16; break;
			case COPPER: armorpenalties = 21; break;
			case SILVER: armorpenalties = 17; break;
			case GOLD: armorpenalties = 8; break;
			case PLATINUM: armorpenalties = 18; break;
			case MITHRIL: armorpenalties = 13; break;
			case VIVA: armorpenalties = 12; break;
			case POURPOOR: armorpenalties = 20; break;
		}

		if (uwep && (weapon_type(uwep) == P_QUARTERSTAFF)) {
			armorpenalties *= 4;
			armorpenalties /= 5;
		}

		if (uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "velvet gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "barkhatnyye perchatki") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "baxmal qo'lqop") ) ) armorpenalties /= 2;

		splcaster += (urole.spelarmr * armorpenalties / 100);
	}
	if (uarms) {
		shieldpenalties = 15;
		if (!is_metallic(uarms) || is_etheritem(uarms)) shieldpenalties /= 3;

		switch (objects[(uarms)->otyp].oc_material) {
			default: break;
			case METAL: shieldpenalties = 16; break;
			case COPPER: shieldpenalties = 21; break;
			case SILVER: shieldpenalties = 17; break;
			case GOLD: shieldpenalties = 8; break;
			case PLATINUM: shieldpenalties = 18; break;
			case MITHRIL: shieldpenalties = 13; break;
			case VIVA: shieldpenalties = 12; break;
			case POURPOOR: shieldpenalties = 20; break;
		}

		if (uwep && (weapon_type(uwep) == P_QUARTERSTAFF)) {
			shieldpenalties *= 4;
			shieldpenalties /= 5;
		}

		if (uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "velvet gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "barkhatnyye perchatki") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "baxmal qo'lqop") ) ) shieldpenalties /= 2;

		splcaster += (urole.spelshld * shieldpenalties / 12);
	}

	if (uarmh && is_metallic(uarmh) && !is_etheritem(uarmh) && uarmh->otyp != HELM_OF_BRILLIANCE) {
		armorpenalties = 15;

		switch (objects[(uarmh)->otyp].oc_material) {
			default: break;
			case METAL: armorpenalties = 16; break;
			case COPPER: armorpenalties = 21; break;
			case SILVER: armorpenalties = 17; break;
			case GOLD: armorpenalties = 8; break;
			case PLATINUM: armorpenalties = 18; break;
			case MITHRIL: armorpenalties = 13; break;
			case VIVA: armorpenalties = 12; break;
			case POURPOOR: armorpenalties = 20; break;
		}

		if (uwep && (weapon_type(uwep) == P_QUARTERSTAFF)) {
			armorpenalties *= 4;
			armorpenalties /= 5;
		}

		if (uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "velvet gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "barkhatnyye perchatki") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "baxmal qo'lqop") ) ) armorpenalties /= 2;

		splcaster += (uarmhbon * armorpenalties / 12);
	}
	if (uarmg && is_metallic(uarmg) && !is_etheritem(uarmg)) {
		armorpenalties = 15;

		switch (objects[(uarmg)->otyp].oc_material) {
			default: break;
			case METAL: armorpenalties = 16; break;
			case COPPER: armorpenalties = 21; break;
			case SILVER: armorpenalties = 17; break;
			case GOLD: armorpenalties = 8; break;
			case PLATINUM: armorpenalties = 18; break;
			case MITHRIL: armorpenalties = 13; break;
			case VIVA: armorpenalties = 12; break;
			case POURPOOR: armorpenalties = 20; break;
		}

		if (uwep && (weapon_type(uwep) == P_QUARTERSTAFF)) {
			armorpenalties *= 4;
			armorpenalties /= 5;
		}

		if (uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "velvet gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "barkhatnyye perchatki") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "baxmal qo'lqop") ) ) armorpenalties /= 2;

		splcaster += (uarmgbon * armorpenalties / 12);
	}
	if (uarmf && is_metallic(uarmf) && !is_etheritem(uarmf)) {
		armorpenalties = 15;

		switch (objects[(uarmf)->otyp].oc_material) {
			default: break;
			case METAL: armorpenalties = 16; break;
			case COPPER: armorpenalties = 21; break;
			case SILVER: armorpenalties = 17; break;
			case GOLD: armorpenalties = 8; break;
			case PLATINUM: armorpenalties = 18; break;
			case MITHRIL: armorpenalties = 13; break;
			case VIVA: armorpenalties = 12; break;
			case POURPOOR: armorpenalties = 20; break;
		}

		if (uwep && (weapon_type(uwep) == P_QUARTERSTAFF)) {
			armorpenalties *= 4;
			armorpenalties /= 5;
		}

		if (uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "velvet gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "barkhatnyye perchatki") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "baxmal qo'lqop") ) ) armorpenalties /= 2;

		splcaster += (uarmfbon * armorpenalties / 12);
	}

	if (spellid(spell) == urole.spelspec)
		splcaster += urole.spelsbon;

	/* `healing spell' bonus */
	if (spell_skilltype(spellid(spell)) == P_HEALING_SPELL)
		splcaster += special;

	if (uarm && uarm->otyp == ROBE_OF_POWER) splcaster -= 3;
	if (uarm && uarm->otyp == ROBE_OF_SPELL_POWER) splcaster -= 3;
	if (uarmg && uarmg->oartifact == ART_GAUNTLETS_OF_SPELL_POWER) splcaster -= 3;
	if (uarm && uarm->oartifact == ART_SPIDERSILK) splcaster -= 3;
	if (uarm && uarm->oartifact == ART_WEB_OF_LOLTH) splcaster -= 3;
	if (uwep && uwep->oartifact == ART_ARYFAERN_KERYM) splcaster -= 3;

	if (Role_if(PM_PALADIN)) splcaster -= 3; /* it is assumed some of their power is granted by the Lord of Light himself */


	/* Calculate learned ability */

	/* Players basic likelihood of being able to cast any spell
	 * is based of their `magic' statistic. (Int or Wis)
	 */
	chance = 11 * statused / 2;

	/*
	 * High level spells are harder.  Easier for higher level casters.
	 * The difficulty is based on the hero's level and their skill level
	 * in that spell type.
	 */
	skill = P_SKILL(spell_skilltype(spellid(spell)));
	if (PlayerCannotUseSkills) skill = P_ISRESTRICTED;

	/* come on, you should be able to cast better if your skill is higher! --Amy */
	if ( skill == P_BASIC) splcaster -= 3;
	if ( skill == P_SKILLED) splcaster -= 6;
	if ( skill == P_EXPERT) splcaster -= 9;
	if ( skill == P_MASTER) splcaster -= 12;
	if ( skill == P_GRAND_MASTER) splcaster -= 15;
	if ( skill == P_SUPREME_MASTER) splcaster -= 18;

	/* casting it often (and thereby keeping it in memory) should also improve chances... */
	if ( spellknow(spell) >= 20000) splcaster -= 1;
	if (( spellknow(spell) >= 23333) && (spellev(spell) < 5) ) splcaster -= 1;
	if (( spellknow(spell) >= 26666) && (spellev(spell) < 6) ) splcaster -= 1;
	if (( spellknow(spell) >= 30000) && (spellev(spell) < 7) ) splcaster -= 1;
	if (( spellknow(spell) >= 33333) && (spellev(spell) < 8) ) splcaster -= 1;
	if ( spellknow(spell) >= 36666) splcaster -= 1;
	if (( spellknow(spell) >= 40000) && (spellev(spell) < 5) ) splcaster -= 1;
	if (( spellknow(spell) >= 43333) && (spellev(spell) < 6) ) splcaster -= 1;
	if (( spellknow(spell) >= 46666) && (spellev(spell) < 7) ) splcaster -= 1;
	if (( spellknow(spell) >= 50000) && (spellev(spell) < 8) ) splcaster -= 1;
	if ( spellknow(spell) >= 53333) splcaster -= 1;
	if (( spellknow(spell) >= 56666) && (spellev(spell) < 5) ) splcaster -= 1;
	if (( spellknow(spell) >= 60000) && (spellev(spell) < 6) ) splcaster -= 1;
	if (( spellknow(spell) >= 63333) && (spellev(spell) < 7) ) splcaster -= 1;
	if (( spellknow(spell) >= 66666) && (spellev(spell) < 8) ) splcaster -= 1;
	if ( spellknow(spell) >= 69000) splcaster -= 1;

	skill = max(skill,P_UNSKILLED) - 1;	/* unskilled => 0 */
	difficulty= (spellev(spell)-1) * 3 - ((skill * 6) + (u.ulevel/3) + 1);

	splcaster += 5;

	if (splcaster < 0) splcaster = 0;
	if (splcaster > 25) splcaster = 25;

	if (difficulty > 0) {
		/* Player is too low level or unskilled. */
		chance -= isqrt(900 * difficulty + 2000);
	} else {
		/* Player is above level.  Learning continues, but the
		 * law of diminishing returns sets in quickly for
		 * low-level spells.  That is, a player quickly gains
		 * no advantage for raising level.
		 */
		int learning = 15 * -difficulty / spellev(spell);
		chance += learning > 20 ? 20 : learning;
	}

	/* Clamp the chance: >18 stat and advanced learning only help
	 * to a limit, while chances below "hopeless" only raise the
	 * specter of overflowing 16-bit ints (and permit wearing a
	 * shield to raise the chances :-).
	 */
	if (chance < 0) chance = 0;
	if (chance > 120) chance = 120;

	/* Wearing anything but a light shield makes it very awkward
	 * to cast a spell.  The penalty is not quite so bad for the
	 * player's class-specific spell.
	 */
	/* if (uarms && weight(uarms) > (int) objects[SMALL_SHIELD].oc_weight) {
		if (spellid(spell) == urole.spelspec) {
			chance /= 2;
		} else {
			chance /= 3;
		}
	}*/

	/* Finally, chance (based on player intell/wisdom and level) is
	 * combined with ability (based on player intrinsics and
	 * encumbrances).  No matter how intelligent/wise and advanced
	 * a player is, intrinsics and encumbrance can prevent casting;
	 * and no matter how able, learning is always required.
	 */
	chance = chance * (30-splcaster) / 10;

	if ( (CasterProblem || u.uprops[CASTER_PROBLEM].extrinsic || have_antimagicstone() || (uarm && uarm->oartifact == ART_ROBE_OF_THE_ARCHMAGI) || (uarmc && uarmc->oartifact == ART_HERETICAL_FIGURE) || (uarm && uarm->oartifact == ART_SHIVANHUNTER_S_UNUSED_PRIZ) ) && chance > 0) {

		chance = (chance * u.uhp / u.uhpmax);

	}

	switch (spellev(spell)) {

		case 1:
		case 2:
		case 3:
		case 4:
		default:
			break;
		case 5:
			chance -= 10;
			break;
		case 6:
			chance -= 30;
			break;
		case 7:
			chance -= 60;
			break;
		case 8:
			chance -= 100;
			break;

	}

	if (SpellColorBlack) {

		switch (spellev(spell)) {

			case 1:
				chance -= 10;
				break;
			case 2:
				chance -= 25;
				break;
			case 3:
				chance -= 50;
				break;
			case 4:
				chance -= 100;
				break;
			case 5:
				chance -= 150;
				break;
			case 6:
				chance -= 200;
				break;
			case 7:
				chance -= 250;
				break;
			case 8:
				chance -= 300;
				break;

		}

	}

	if (issoviet) chance -= 30;

	if (Race_if(PM_PLAYER_SKELETON)) chance -= 50;

	if (spell_skilltype(spellid(spell)) == P_CHAOS_SPELL) { /* more difficult! */

	switch (spellev(spell)) {

			case 1:
				chance -= 5;
				if (!Role_if(PM_CHAOS_SORCEROR)) chance -= 5;
				break;
			case 2:
				chance -= 10;
				if (!Role_if(PM_CHAOS_SORCEROR)) chance -= 10;
				break;
			case 3:
				chance -= 15;
				if (!Role_if(PM_CHAOS_SORCEROR)) chance -= 15;
				break;
			case 4:
				chance -= 25;
				if (!Role_if(PM_CHAOS_SORCEROR)) chance -= 25;
				break;
			case 5:
				chance -= 40;
				if (!Role_if(PM_CHAOS_SORCEROR)) chance -= 40;
				break;
			case 6:
				chance -= 60;
				if (!Role_if(PM_CHAOS_SORCEROR)) chance -= 60;
				break;
			case 7:
				chance -= 80;
				if (!Role_if(PM_CHAOS_SORCEROR)) chance -= 80;
				break;
			case 8:
				chance -= 100;
				if (!Role_if(PM_CHAOS_SORCEROR)) chance -= 100;
				break;

		}

	}

	if (uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "uncanny gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "sverkh''yestestvennyye perchatki") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "dahshatli qo'lqop") )) chance -= 10;

	if (uarm && uarm->oartifact == ART_DRAGON_PLATE) chance -= 20;
	if (uarm && uarm->oartifact == ART_ROFLCOPTER_WEB) chance += 10;
	if (uarm && uarm->otyp == ROBE_OF_SPELL_POWER) chance += 20;
	if (uarmh && uarmh->oartifact == ART_ZERO_PERCENT_FAILURE) chance += 10;
	if (uarmc && uarmc->oartifact == ART_HENRIETTA_S_HEAVY_CASTER) chance += 15;
	if (uarmf && uarmf->oartifact == ART_SUNALI_S_SUMMONING_STORM) chance += 15;
	if (uwep && uwep->otyp == OLDEST_STAFF) chance += 10;
	if (uleft && uleft->oartifact == ART_HENRIETTA_S_MAGICAL_AID) chance += 50;
	if (uright && uright->oartifact == ART_HENRIETTA_S_MAGICAL_AID) chance += 50;
	if (Role_if(PM_ARCHEOLOGIST) && uamul && uamul->oartifact == ART_ARCHEOLOGIST_SONG) chance += 10;
	if (uarmh && OBJ_DESCR(objects[uarmh->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "knowledgeable helmet") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "znayushchikh shlem") || !strcmp(OBJ_DESCR(objects[uarmh->otyp]), "bilimdon dubulg'a") ) ) chance += 10;
	if (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "science cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "nauka plashch") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "ilm-fan plash") ) ) chance += 10;

	if (Upolyd && dmgtype(youmonst.data, AD_SPEL) ) {
		chance += 5;
	}
	if (Upolyd && dmgtype(youmonst.data, AD_CLRC) ) {
		chance += 5;
	}
	if (Upolyd && dmgtype(youmonst.data, AD_CAST) ) {
		chance += 10;
	}


	if (Race_if(PM_INKA) && spellid(spell) == SPE_NATURE_BEAM)
		chance += 100;

	if (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "dnethack cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "podzemeliy i vnezemnyye plashch vzlomat'") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "hamzindon va dunyo bo'lmagan doirasi so'yish plash") )) chance -= 10;
	if (RngeDnethack) chance -= 10;
	if (RngeUnnethack) chance -= 33;

	/* Clamp to percentile */
	if (chance > 100) chance = 100;

	if (uarmc && OBJ_DESCR(objects[uarmc->otyp]) && (!strcmp(OBJ_DESCR(objects[uarmc->otyp]), "shell cloak") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "plashch obolochki") || !strcmp(OBJ_DESCR(objects[uarmc->otyp]), "qobiq plash") ) ) chance -= 20;

	if (is_grassland(u.ux, u.uy)) chance -= 10;

	if (Role_if(PM_FAILED_EXISTENCE)) chance /= 2; /* at least 50% fail for all spells */
	if (uarmc && uarmc->oartifact == ART_ARTIFICIAL_FAKE_DIFFICULTY) {
		chance *= 5;
		chance /= 6;
	}

	/* artifacts and other items that boost the chance after "hard" penalties are applied go here --Amy */

	if (uarm && uarm->oartifact == ART_MOTHERFUCKER_TROPHY) chance += 20;

	/* REALLY clamp chance now */
	if (chance > 100) chance = 100;

	if (chance < (issoviet ? 0 : (spellev(spell) == 8) ? 0 : (spellev(spell) == 7) ? 1 : (spellev(spell) == 6) ? 2 : (spellev(spell) == 5) ? 5 : 10) ) chance = (issoviet ? 0 : (spellev(spell) == 8) ? 0 : (spellev(spell) == 7) ? 1 : (spellev(spell) == 6) ? 2 : (spellev(spell) == 5) ? 5 : 10); /* used to be 0, but that was just stupid in my opinion --Amy */

	return chance;
}

/* Learn a spell during creation of the initial inventory */
void
initialspell(obj)
struct obj *obj;
{
	int i;

	for (i = 0; i < MAXSPELL; i++) {
	    if (spellid(i) == obj->otyp) { /* not a bug - after all, you might e.g. play a haxor --Amy */
	    	/* pline("Error: Spell %s already known.", OBJ_NAME(objects[obj->otyp])); */

		/* In Soviet Russia, enhancements aren't a thing. In fact, they don't even know how to spell the word 'enhancement'. Therefore, if someone goes ahead and suggests an enhancement that consists of double spellbooks giving twice the starting spellcasting memory, they say NOPE THAT IS INCOMPATIBLE WITH COMMUNISM and refuse to implement it. --Amy */
		if (!issoviet) {
			incrnknow(i);
			if (Role_if(PM_MAHOU_SHOUJO)) incrnknow(i);

			if (spl_book[i].sp_lev == 3) incrnknow(i);
			if (spl_book[i].sp_lev == 4) { incrnknow(i); incrnknow(i);}
			if (spl_book[i].sp_lev == 5) { incrnknow(i); incrnknow(i); incrnknow(i);}
			if (spl_book[i].sp_lev == 6) { incrnknow(i); incrnknow(i); incrnknow(i); incrnknow(i);}
			if (spl_book[i].sp_lev == 7) { incrnknow(i); incrnknow(i); incrnknow(i); incrnknow(i); incrnknow(i);}
			if (spl_book[i].sp_lev == 8) { incrnknow(i); incrnknow(i); incrnknow(i); incrnknow(i); incrnknow(i); incrnknow(i);}
		}

	         return;
	    }
	    if (spellid(i) == NO_SPELL)  {
	        spl_book[i].sp_id = obj->otyp;
	        spl_book[i].sp_lev = objects[obj->otyp].oc_level;
	        incrnknow(i);
			if (Role_if(PM_MAHOU_SHOUJO)) incrnknow(i);

			/* high-level starting spells will be known for a longer time
			 * since you might not be able to cast them at all when you're just starting --Amy */
			if (spl_book[i].sp_lev == 3) incrnknow(i);
			if (spl_book[i].sp_lev == 4) { incrnknow(i); incrnknow(i);}
			if (spl_book[i].sp_lev == 5) { incrnknow(i); incrnknow(i); incrnknow(i);}
			if (spl_book[i].sp_lev == 6) { incrnknow(i); incrnknow(i); incrnknow(i); incrnknow(i);}
			if (spl_book[i].sp_lev == 7) { incrnknow(i); incrnknow(i); incrnknow(i); incrnknow(i); incrnknow(i);}
			if (spl_book[i].sp_lev == 8) { incrnknow(i); incrnknow(i); incrnknow(i); incrnknow(i); incrnknow(i); incrnknow(i);}

	        return;
	    }
	}
	impossible("Too many spells memorized!");
	return;
}

boolean
studyspell()
{
	/*Vars are for studying spells 'W', 'F', 'I', 'N'*/
	int spell_no;

	if (getspell(&spell_no)) {
		if (spellknow(spell_no) <= 0) {
			You("are unable to focus your memory of the spell.");
			return (FALSE);
		} else if (spellknow(spell_no) <= 1000) {
			Your("focus and reinforce your memory of the spell.");
			incrnknow(spell_no);
			if (Role_if(PM_MAHOU_SHOUJO)) incrnknow(spell_no);
			exercise(A_WIS, TRUE);      /* extra study */
			return (TRUE);
		} else /* 1000 < spellknow(spell_no) <= 5000 */
			You("know that spell quite well already.");
	}
	return (FALSE);
}

/* Assumes u.dx, u.dy already set up */
static int
spell_dash()
{
	register int dashrange = 2;

	if (!(PlayerCannotUseSkills) && P_SKILL(P_ELEMENTAL_SPELL) >= P_SKILLED) {
		switch (P_SKILL(P_ELEMENTAL_SPELL)) {
			default: break;
			case P_SKILLED: dashrange = 3; break;
			case P_EXPERT: dashrange = 4; break;
			case P_MASTER: dashrange = 5; break;
			case P_GRAND_MASTER: dashrange = 6; break;
			case P_SUPREME_MASTER: dashrange = 7; break;
		}
	}

	while (dashrange > 2) {
		pline("Current dash range: %d.", dashrange);
		if (yn("Reduce the range by one?") == 'y') {
			dashrange--;
		} else goto dashrangefinish;
	}
	
dashrangefinish:

	if ((!Punished || carried(uball)) && !u.utrap)
	    You("dash forwards!");
	hurtle(u.dx, u.dy, dashrange, FALSE);
	multi = 0;		/* No paralysis with dash */
	return 1;
}

void
wonderspell()
{
	register int randomspell = SPE_FORCE_BOLT + rn2((SPE_PSYBEAM + 1) - SPE_FORCE_BOLT);
	char splname[BUFSZ];
	int i;

	sprintf(splname, OBJ_NAME(objects[randomspell]) );

	for (i = 0; i < MAXSPELL; i++)  {
		if (spellid(i) == randomspell)  {
			if (spellknow(i) <= MAX_CAN_STUDY) {
				Your("knowledge of the %s spell is keener.", splname);
				incrnknow(i);
				if (uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "runic gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "runa rukovitsakh") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "runi qo'lqop") ) && !rn2(2) ) incrnknow(i);
				if (Role_if(PM_MAHOU_SHOUJO)) incrnknow(i);

			} else {
			    You("know %s quite well already.", splname);
			}
			break;
		} else if (spellid(i) == NO_SPELL)  {
			spl_book[i].sp_id = randomspell;
			spl_book[i].sp_lev = objects[randomspell].oc_level;
			incrnknow(i);
			if (uarmg && OBJ_DESCR(objects[uarmg->otyp]) && ( !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "runic gloves") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "runa rukovitsakh") || !strcmp(OBJ_DESCR(objects[uarmg->otyp]), "runi qo'lqop") ) && !rn2(2) ) incrnknow(i);
			if (Role_if(PM_MAHOU_SHOUJO)) incrnknow(i);
			You("gain knowledge of the %s spell.", splname);
			if (randomspell == SPE_FORBIDDEN_KNOWLEDGE) {
				u.ugangr += 15;
				if (flags.soundok) You_hear("a thunderous growling and rumbling...");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Pozdravlyayu, teper' vashe bozhestvo ochen' zol. Pochemu ty ne molish'sya? Eto mozhet byt' prosto privesti svoyu zhizn' do kontsa, a zatem vy mozhete svernut' luchshe, nesovetskimi kharakter nakonets-to! SDELAY ETO SEYCHAS!" : "Grrrrrrr! Grummel! Wummmmmm!");
			}

			if (randomspell == SPE_AMNESIA) {
				You_feel("dizzy!");
				forget(ALL_MAP);
			}
			break;
		}
	}

}

/*spell.c*/
