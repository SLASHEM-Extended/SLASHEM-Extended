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
#define MAX_KNOW 	200000	/* Absolute Max timeout */
#define MAX_CAN_STUDY 	190000	/* Can study while timeout is less than */

#define MAX_STUDY_TIME 	  300	/* Max time for one study session */
#define MAX_SPELL_STUDY    30	/* Uses before spellbook crumbles */

#define spellknow(spell)	spl_book[spell].sp_know 
#define spellmemorize(spell)	spl_book[spell].sp_memorize

static NEARDATA const char revivables[] = { ALLOW_FLOOROBJ, FOOD_CLASS, 0 };

static const char all_count[] = { ALLOW_COUNT, ALL_CLASSES, 0 };
static const char allnoncount[] = { ALL_CLASSES, 0 };
static const char allowall[] = { ALL_CLASSES, 0 };


#define spellev(spell)		spl_book[spell].sp_lev
#define spellid(spell)          spl_book[spell].sp_id
#define spellname(spell)	OBJ_NAME(objects[spellid(spell)])
#define spellet(spell)	\
	((char)((spell < 26) ? ('a' + spell) : \
	        (spell < 52) ? ('A' + spell - 26) : \
		(spell < 78) ? ('0' + spell - 52) : \
		(spell < 104) ? ('a' + spell - 78) : \
		(spell < 130) ? ('A' + spell - 104) : \
		(spell < 156) ? ('0' + spell - 130) : 0 ))

STATIC_DCL int spell_let_to_idx(CHAR_P);
STATIC_DCL boolean cursed_book(struct obj *bp);
STATIC_DCL boolean confused_book(struct obj *);
STATIC_DCL void deadbook(struct obj *);
STATIC_PTR int learn(void);
STATIC_DCL void do_reset_learn(void);
STATIC_DCL boolean getspell(int *, BOOLEAN_P);
STATIC_DCL boolean dospellmenu(const char *,int,int *, int);
STATIC_DCL int percent_success(int);
STATIC_DCL void cast_protection(void);
STATIC_DCL void cast_reflection(void);
STATIC_DCL void spell_backfire(int);
STATIC_DCL const char *spelltypemnemonic(int);
static int spell_dash(void);
STATIC_DCL void boostknow(int, int);
STATIC_DCL void drainknow(int, int);
STATIC_DCL void incrnknow(int, BOOLEAN_P);
STATIC_DCL void handleartifactspellbook(struct obj *);
STATIC_PTR void do_megafloodings(int, int, void *);

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

#define uarmhbon 3 /* Metal helmets interfere with the mind */
#define uarmgbon 5 /* Casting channels through the hands */
#define uarmfbon 1 /* All metal interferes to some degree */

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
do_megafloodings(x, y, poolcnt)
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

	if (In_sokoban(&u.uz) && rn2(5)) return;

	if (Aggravate_monster) {
		u.aggravation = 1;
		reset_rndmonst(NON_PM);
	}

	while (randomamount) {
		randomamount--;
		randomx = rn1(COLNO-3,2);
		randomy = rn2(ROWNO);
		if (isok(randomx, randomy) && !MON_AT(randomx, randomy) && (levl[randomx][randomy].typ == ROOM || levl[randomx][randomy].typ == CORR) ) {

			levl[randomx][randomy].typ = MOAT;
			makemon(mkclass(S_EEL,0), randomx, randomy, NO_MM_FLAGS);

			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
	}

	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) || !rn2(3) || 
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR) || MON_AT(x, y))
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */

		levl[x][y].typ = MOAT;
		makemon(mkclass(S_EEL,0), x, y, NO_MM_FLAGS);

		del_engr_at(x, y);

		if ((mtmp = m_at(x, y)) != 0) {
			(void) minliquid(mtmp);
		} else {
			newsym(x,y);
		}
	} else if ((x == u.ux) && (y == u.uy)) {
		(*(int *)poolcnt)--;
	}

	u.aggravation = 0;

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
	blockorunblock_point(x,y);
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
		    if (uarmg->oerodeproof || (Race_if(PM_CHIQUAI) && rn2(4)) || (uarmg->oartifact && rn2(4)) || (objects[uarmg->otyp].oc_material == MT_GREEN_STEEL && rn2(2)) || !is_corrodeable(uarmg)) {
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
		losestr(StrongPoison_resistance ? 1 : Poison_resistance ? rno(3) : rnd(5), TRUE);
		losehp(rnd(Poison_resistance ? 6 : 10),
		       "contact-poisoned spellbook", KILLED_BY_AN);
		bp->in_use = TRUE;
		break;
	case 6:
		if(Antimagic && !Race_if(PM_KUTAR) && rn2(StrongAntimagic ? 20 : 5) ) {
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
	return ((bp->spe < 0) && rn2(2)) ? TRUE : FALSE;
}

/* study while confused: returns TRUE if the book is destroyed */
STATIC_OVL boolean
confused_book(spellbook)
struct obj *spellbook;
{
	boolean gone = FALSE;

	if (!rn2(50) && spellbook->otyp != SPE_BOOK_OF_THE_DEAD) {
	    spellbook->in_use = TRUE;	/* in case called from learn */
	    pline("Being confused you have difficulties in controlling your actions.");
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
				blockorunblock_point(randomx,randomy);
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

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a wall at x, y */
		if (levl[x][y].typ != DOOR) levl[x][y].typ = STONE;
		else levl[x][y].typ = CROSSWALL;
		blockorunblock_point(x,y);
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
			blockorunblock_point(randomx,randomy);
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

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = TREE;
		blockorunblock_point(x,y);
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
do_gravefloodg(x, y, poolcnt)
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
			levl[randomx][randomy].typ = GRAVEWALL;
			blockorunblock_point(randomx,randomy);
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

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = GRAVEWALL;
		blockorunblock_point(x,y);
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
do_tunnelfloodg(x, y, poolcnt)
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
			levl[randomx][randomy].typ = TUNNELWALL;
			blockorunblock_point(randomx,randomy);
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

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = TUNNELWALL;
		blockorunblock_point(x,y);
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
do_farmfloodg(x, y, poolcnt)
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
			levl[randomx][randomy].typ = FARMLAND;
			blockorunblock_point(randomx,randomy);
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

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = FARMLAND;
		blockorunblock_point(x,y);
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
do_mountainfloodg(x, y, poolcnt)
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
			levl[randomx][randomy].typ = MOUNTAIN;
			blockorunblock_point(randomx,randomy);
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

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = MOUNTAIN;
		blockorunblock_point(x,y);
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
do_watertunnelfloodg(x, y, poolcnt)
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
			levl[randomx][randomy].typ = WATERTUNNEL;
			blockorunblock_point(randomx,randomy);
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

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = WATERTUNNEL;
		blockorunblock_point(x,y);
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
do_crystalwaterfloodg(x, y, poolcnt)
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
			levl[randomx][randomy].typ = CRYSTALWATER;
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

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = CRYSTALWATER;
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
do_crystalwaterfloodgX(x, y, poolcnt) /* transforms regular water into crystal water as well, as long as you're not on the plane of water, you cheater */
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
			levl[randomx][randomy].typ = CRYSTALWATER;
			del_engr_at(randomx, randomy);
	
			if ((mtmp = m_at(randomx, randomy)) != 0) {
				(void) minliquid(mtmp);
			} else {
				newsym(randomx,randomy);
			}

		}
		if (!Is_waterlevel(&u.uz)) {
			if (isok(randomx, randomy) && (levl[randomx][randomy].typ == WATER || levl[randomx][randomy].typ == MOAT || levl[randomx][randomy].typ == POOL) ) {
				levl[randomx][randomy].typ = CRYSTALWATER;
				del_engr_at(randomx, randomy);
	
				if ((mtmp = m_at(randomx, randomy)) != 0) {
					(void) minliquid(mtmp);
				} else {
					newsym(randomx,randomy);
				}

			}
		}
	}
	if ((rn2(1 + distmin(u.ux, u.uy, x, y))) ||
	    (sobj_at(BOULDER, x, y)) || (levl[x][y].typ != ROOM && levl[x][y].typ != CORR && (Is_waterlevel(&u.uz) || (levl[x][y].typ != WATER && levl[x][y].typ != MOAT && levl[x][y].typ != POOL) ) ) || MON_AT(x, y) )
		return;

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = CRYSTALWATER;
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
do_moorfloodg(x, y, poolcnt)
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
			levl[randomx][randomy].typ = MOORLAND;
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

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = MOORLAND;
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
do_urinefloodg(x, y, poolcnt)
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
			levl[randomx][randomy].typ = URINELAKE;
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

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = URINELAKE;
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
do_shiftingsandfloodg(x, y, poolcnt)
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
			levl[randomx][randomy].typ = SHIFTINGSAND;
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

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = SHIFTINGSAND;
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
do_styxfloodg(x, y, poolcnt)
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
			levl[randomx][randomy].typ = STYXRIVER;
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

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = STYXRIVER;
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
do_snowfloodg(x, y, poolcnt)
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
			levl[randomx][randomy].typ = SNOW;
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

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = SNOW;
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
do_ashfloodg(x, y, poolcnt)
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
			levl[randomx][randomy].typ = ASH;
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

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = ASH;
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
do_sandfloodg(x, y, poolcnt)
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
			levl[randomx][randomy].typ = SAND;
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

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = SAND;
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
do_pavementfloodg(x, y, poolcnt)
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
			levl[randomx][randomy].typ = PAVEDFLOOR;
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

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = PAVEDFLOOR;
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
do_highwayfloodg(x, y, poolcnt)
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
			levl[randomx][randomy].typ = HIGHWAY;
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

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = HIGHWAY;
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
do_grassfloodg(x, y, poolcnt)
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
			levl[randomx][randomy].typ = GRASSLAND;
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

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = GRASSLAND;
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
do_nethermistfloodg(x, y, poolcnt)
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
			levl[randomx][randomy].typ = NETHERMIST;
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

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = NETHERMIST;
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
do_stalactitefloodg(x, y, poolcnt)
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
			levl[randomx][randomy].typ = STALACTITE;
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

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = STALACTITE;
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
do_cryptfloodg(x, y, poolcnt)
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
			levl[randomx][randomy].typ = CRYPTFLOOR;
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

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = CRYPTFLOOR;
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
do_bubblefloodg(x, y, poolcnt)
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
			levl[randomx][randomy].typ = BUBBLES;
			blockorunblock_point(randomx,randomy);
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

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = BUBBLES;
		blockorunblock_point(x,y);
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
do_raincloudfloodg(x, y, poolcnt)
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
			levl[randomx][randomy].typ = RAINCLOUD;
			blockorunblock_point(randomx,randomy);
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

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = RAINCLOUD;
		blockorunblock_point(x,y);
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
			blockorunblock_point(randomx,randomy);
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

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = CLOUD;
		blockorunblock_point(x,y);
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
			blockorunblock_point(randomx,randomy);
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

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = randomwalltype();
		blockorunblock_point(x,y);
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
			blockorunblock_point(randomx,randomy);
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

	(*(int *)poolcnt)++;

	if (!((*(int *)poolcnt) && (x == u.ux) && (y == u.uy))) {
		/* Put a pool at x, y */
		levl[x][y].typ = IRONBARS;
		blockorunblock_point(x,y);
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
	    pline("Teper' vy bol'she ne mozhete pobezhdat' v igre, vy sovershenno glupyy igrok. Vy nikogda ne smozhete podnyat' proklyatiye, i on nikogda ne budet rabotat', poka kniga proklyata kha-kha-kha-kha-kha-kha BWAR KHAR KHAR!");
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
	    mtmp->mfrenzied = 1;
	    set_malign(mtmp);
	}
	/* next handle the affect on things you're carrying */
	(void) unturn_dead(&youmonst);
	/* last place some monsters around you */
	mm.x = u.ux;
	mm.y = u.uy;
	mkundead(&mm, TRUE, NO_MINVENT|MM_ANGRY|MM_FRENZIED, TRUE);
	badeffect();
	aggravate();
    } else if(book2->blessed) {
	for(mtmp = fmon; mtmp; mtmp = mtmp2) {
	    mtmp2 = mtmp->nmon;		/* tamedog() changes chain */
	    if (DEADMONSTER(mtmp)) continue;

	    if ((is_undead(mtmp->data) || mtmp->egotype_undead) && !mtmp->mfrenzied && cansee(mtmp->mx, mtmp->my)) {
		mtmp->mpeaceful = TRUE;
		if(sgn(mtmp->data->maligntyp) == sgn(u.ualign.type)
		   && distu(mtmp->mx, mtmp->my) < 4)
		    if (mtmp->mtame) {
			if (mtmp->mtame < 20)
			    mtmp->mtame++;
		    } else
			(void) tamedog(mtmp, (struct obj *)0, FALSE);
		else monflee(mtmp, rnd(10), FALSE, TRUE, FALSE);
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
	if (delay < end_delay && ublindf && ublindf->otyp == SHIELD_PATE_GLASSES && rn2(2))
	    delay++;
	if (delay < end_delay && ublindf && ublindf->otyp == BOSS_VISOR && rn2(2))
	    delay++;
	if (delay < end_delay && u.tempbooklenses && rn2(2))
	    delay++;
	if (delay < end_delay && uarmh && uarmh->oartifact == ART_ALEJANDRA_S_EDUCATION && rn2(2))
	    delay++;

	if (Confusion && (book->otyp != SPE_BOOK_OF_THE_DEAD) && (book->oartifact != ART_A_LOT_OF_ENYAS_IN_THE_CAST) && (book->oartifact != ART_QUIET_NOTEBOOK) && !(Conf_resist && rn2(StrongConf_resist ? 25 : 5)) && !rn2((Role_if(PM_LIBRARIAN) || Role_if(PM_PSYKER)) ? 500 : 50) ) {		/* became confused while learning */

	    (void) confused_book(book);
	    book = 0;			/* no longer studying */
	    if ((delay - end_delay) < 0) {
			if (!issoviet) {

				register int actualdelay;
				actualdelay = rno(-(delay - end_delay));
				if (!isstunfish && (actualdelay > 0)) actualdelay = isqrt(actualdelay); /* a lot of reduction --Amy */

				nomul(-(actualdelay), "reading a confusing book", TRUE); /* remaining delay is uninterrupted */
			} else { /* soviet mode */
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

	if(booktype == SPE_BLADE_ANGER) {
		impossible("player trying to read spellbook of blade anger");
		return 0;
	}
	if(booktype == SPE_BEAMSWORD) {
		impossible("player trying to read spellbook of beamsword");
		return 0;
	}
	if(booktype == SPE_DRAINING_ARROW) {
		impossible("player trying to read spellbook of draining arrow");
		return 0;
	}
	if(booktype == SPE_MENSTRUATION) {
		impossible("player trying to read spellbook of menstruation");
		return 0;
	}

	sprintf(splname, objects[booktype].oc_name_known ?
			"\"%s\"" : "the \"%s\" spell",
		OBJ_NAME(objects[booktype]) );
	for (i = 0; i < MAXSPELL; i++)  {
		if (spellid(i) == booktype)  {
			if ((book->spestudied > MAX_SPELL_STUDY) && issoviet) { /* stupid feature, which has no place in SLEX, so let's relegate it to soviet mode */
			    pline("This spellbook is too faint to be read anymore.");
			    if (issoviet) pline("mua kha kha kha teper' kniga zaklinaniy bol'she ne rabotayet, ty tupoy osel!");
			    if (book) book->otyp = booktype = SPE_BLANK_PAPER;
			} else if (spellknow(i) <= MAX_CAN_STUDY) {
			    Your("knowledge of that spell is keener.");

			    handleartifactspellbook(book); /* ATTENTION: apparently this can cause "book" to be removed */

				/* ultra annoyingly, "boostknow" cannot be used in handleartifactspellbook(), because lol why would we have a function that
				 * allows you to just say "increase knowledge of the spell with this ID". nooooooo, we have to make it a function that increases
				 * the knowledge of "spell at a certain point in your list of spells", that's like so annoying I don't even... --Amy
				 * ATTENTION !!!!!!! handleartifactspellbook() is called again below so all those artifacts that do "boostknow" stuff will
				 * ***HAVE*** to also be handled again below where that other function call is or they won't work right !!!!! */

				if (book && book->oartifact == ART_YOU_CAN_HURT) {
					losehp( (u.uhpmax / 2) + 2, "being hurt by a spellbook", KILLED_BY);
					boostknow(i, 50000);
					You("can hurt, but the spell will last for a generous time.");
				}
				if (book && book->oartifact == ART_TY___GLUPAYA_LEDYANAYA_GLY) {
					boostknow(i, 20000);
					pline_The("spell has a lot of memory now.");
				}
				if (book && book->oartifact == ART_FARTOMAT) {
					boostknow(i, 10000);
					pline_The("spell has a lot of memory now.");
				}
				if (book && book->oartifact == ART_BACKLASHPROTECT) {
					boostknow(i, 40000);
					pline_The("spell has a lot of memory now.");
				}
				if (book && book->oartifact == ART_MULTIPLY_ME) {
					boostknow(i, Role_if(PM_OCCULT_MASTER) ? 40000 : 10000);
					pline_The("spell memory bonus was multiplied!");
				}
				if (book && (book->oartifact == ART_PAGAN_POETRY || book->oartifact == ART_FLOEPFLOEPFLOEPFLOEPFLOEPF || book->oartifact == ART_HELIOKOPIS_S_SHORT_REAPPEA || book->oartifact == ART_K_OR_EA_SY)) {
					u.superspecialspell = booktype;
					pline_The("spell is your super special spell now.");
				}

			    use_skill(P_MEMORIZATION, spellev(i) + rnd(spellev(i) * 3) );
			    if (!rn2(3)) u.uenmax++;
			    u.cnd_spellbookcount++;
			    incrnknow(i, FALSE);

			    if (u.emynluincomplete) boostknow(i, 1000);
			    if (uarmc && uarmc->oartifact == ART_READ_UP_ON_IT) boostknow(i, 20000);
				if (uarmg && itemhasappearance(uarmg, APP_RUNIC_GLOVES) && !rn2(2) ) incrnknow(i, FALSE);
				if (Role_if(PM_MAHOU_SHOUJO)) incrnknow(i, FALSE);
			    if (book) book->spestudied++;

				if (!PlayerCannotUseSkills && P_SKILL(P_MEMORIZATION) >= P_BASIC) {

					char nervbuf[QBUFSZ];
					char thisisannoying = 0;

					if (!u.youhavememorized) {
						u.youhavememorized = TRUE;
						if (!iflags.memorizationknown) pline("You have the memorization skill, which allows you to gain extra spell memory for newly learned spells. Whenever you learn a spell, you are asked whether you want to use the skill to boost the new spell's memory. In the case of doubt you should ALWAYS ANSWER YES. If you answer no, you just throw the bonus away. (Exception is if you want a forgotten spell, but you only ever need one of those normally.)");
					}

					if (!iflags.memorizationknown) sprintf(nervbuf, "Memorize this spell to add more spell memory? In the case of doubt you should always answer yes, unless you want the bonus to go to waste.");
					else sprintf(nervbuf, "Memorize this spell to add more spell memory?");
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
						pline("Spell memory increased! You gained %d%% extra spell memory.", memoboost * 10);
					} else {
						spl_book[i].sp_memorize = FALSE;
						pline("You decided to throw away the spell memory bonus. The spell was set to non-memorization mode. If you did that by mistake, you should open the spell menu and turn memorization for this spell back on so that it properly benefits from memorization skill.");
					}

				}

			    if (book && end_delay) {
			    	boostknow(i, end_delay * ((book->spe > 0) ? 20 : 10));

				use_skill(spell_skilltype(book->otyp), end_delay / ((book->spe > 0) ? 10 : 20));
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
			spl_book[i].sp_memorize = TRUE;
			use_skill(P_MEMORIZATION, spellev(i) + rnd(spellev(i) * 3) );
			if (!rn2(3)) u.uenmax++;
			u.cnd_spellbookcount++;
			incrnknow(i, TRUE);
			if (u.emynluincomplete) boostknow(i, 1000);
			if (uarmc && uarmc->oartifact == ART_READ_UP_ON_IT) boostknow(i, 20000);
			if (uarmg && itemhasappearance(uarmg, APP_RUNIC_GLOVES) && !rn2(2) ) incrnknow(i, TRUE);
			if (Role_if(PM_MAHOU_SHOUJO)) incrnknow(i, TRUE);
			if (book) book->spestudied++;
			You("have keen knowledge of the spell.");
			You(i > 0 ? "add %s to your repertoire." : "learn %s.",
			    splname);

			handleartifactspellbook(book); /* ATTENTION: apparently this can cause "book" to be removed */

			if (book && book->oartifact == ART_YOU_CAN_HURT) {
				losehp( (u.uhpmax / 2) + 2, "being hurt by a spellbook", KILLED_BY);
				boostknow(i, 50000);
				You("can hurt, but the spell will last for a generous time.");
			}
			if (book && book->oartifact == ART_TY___GLUPAYA_LEDYANAYA_GLY) {
				boostknow(i, 20000);
				pline_The("spell has a lot of memory now.");
			}
			if (book && book->oartifact == ART_FARTOMAT) {
				boostknow(i, 10000);
				pline_The("spell has a lot of memory now.");
			}
			if (book && book->oartifact == ART_BACKLASHPROTECT) {
				boostknow(i, 40000);
				pline_The("spell has a lot of memory now.");
			}
			if (book && book->oartifact == ART_MULTIPLY_ME) {
				boostknow(i, Role_if(PM_OCCULT_MASTER) ? 40000 : 10000);
				pline_The("spell memory bonus was multiplied!");
			}
			if (book && (book->oartifact == ART_PAGAN_POETRY || book->oartifact == ART_FLOEPFLOEPFLOEPFLOEPFLOEPF || book->oartifact == ART_HELIOKOPIS_S_SHORT_REAPPEA || book->oartifact == ART_K_OR_EA_SY)) {
				u.superspecialspell = booktype;
				pline_The("spell is your super special spell now.");
			}

			if (booktype == SPE_FORBIDDEN_KNOWLEDGE) {
				u.ugangr += 15;
				if (flags.soundok) You_hear("a thunderous growling and rumbling...");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Pozdravlyayu, teper' vashe bozhestvo ochen' zol. Pochemu ty ne molish'sya? Eto mozhet byt' prosto privesti svoyu zhizn' do kontsa, a zatem vy mozhete svernut' luchshe, nesovetskimi kharakter nakonets-to! SDELAY ETO SEYCHAS!" : "Grrrrrrr! Grummel! Wummmmmm!");
			}

			makeknown((int)booktype);
			if (booktype == SPE_AMNESIA) {
				You_feel("dizzy!");
				forget(ALL_MAP, FALSE);
			}

			if (!PlayerCannotUseSkills && P_SKILL(P_MEMORIZATION) >= P_BASIC) {

				char nervbuf[QBUFSZ];
				char thisisannoying = 0;

				if (!u.youhavememorized) {
					u.youhavememorized = TRUE;
					if (!iflags.memorizationknown) pline("You have the memorization skill, which allows you to gain extra spell memory for newly learned spells. Whenever you learn a spell, you are asked whether you want to use the skill to boost the new spell's memory. In the case of doubt you should ALWAYS ANSWER YES. If you answer no, you just throw the bonus away. (Exception is if you want a forgotten spell, but you only ever need one of those normally.)");
				}

				if (!iflags.memorizationknown) sprintf(nervbuf, "Memorize this spell to add more spell memory? In the case of doubt you should always answer yes, unless you want the bonus to go to waste.");
				else sprintf(nervbuf, "Memorize this spell to add more spell memory?");
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
					pline("Spell memory increased! You gained %d%% extra spell memory.", memoboost * 10);
				} else {
					spl_book[i].sp_memorize = FALSE;
					pline("You decided to throw away the spell memory bonus. The spell was set to non-memorization mode. If you did that by mistake, you should open the spell menu and turn memorization for this spell back on so that it properly benefits from memorization skill.");
				}

			}

			break;
		}
	}
	if (i == MAXSPELL) impossible("Too many spells memorized!");

	if (book && (book->cursed || book->spe < 1) && !(uimplant && uimplant->oartifact == ART_DOMPFINATION) && !Role_if(PM_LIBRARIAN) && !Role_if(PM_PSYKER) && !(booktype == SPE_BOOK_OF_THE_DEAD) ) {	/* maybe a demon cursed it */
	    if (cursed_book(book)) {
		if (carried(book)) useup(book);
		else useupf(book, 1L);
		pline_The("book falls apart.");
		book = 0;
		return 0;
	    }
	}
	if (book && (book->oartifact == ART_CHRONICLE_OF_THE_LAST_DAWN)) {
		if (carried(book)) useup(book);
		else useupf(book, 1L);
		pline_The("book falls apart.");
		book = 0;
		return 0;

	}
	if (book && costly) check_unpaid(book);
	if (book) book = 0;
	return(0);
}

int
study_book(spellbook)
register struct obj *spellbook;
{
	register int	 booktype = spellbook->otyp;
	register boolean confused = ((Confusion != 0) && !(spellbook->oartifact == ART_QUIET_NOTEBOOK) && !(Conf_resist && (StrongConf_resist ? rn2(3) : !rn2(3)) ));
	boolean too_hard = FALSE;

	if (delay && !confused && spellbook == book &&
		    /* handle the sequence: start reading, get interrupted,
		       have book become erased somehow, resume reading it */
		    booktype != SPE_BLANK_PAPER) {
		You("continue your efforts to memorize the spell.");
		if (AutoDestruct || u.uprops[AUTO_DESTRUCT].extrinsic || (uarmf && uarmf->oartifact == ART_KHOR_S_REQUIRED_IDEA) || have_autodestructstone() || (uarm && uarm->oartifact == ART_DESTRUCTO_S_COAT) || (uchain && uchain->oartifact == ART_SIYID) ) learn();
	} else {
		/* KMH -- Simplified this code */
		if (booktype == SPE_BLANK_PAPER) {
			pline("This spellbook is all blank.");
			makeknown(booktype);
			return(1);
		}

		if (ACURR(A_INT) == 1 && !rn2(4)) {

			check_unpaid_usage(spellbook, TRUE);
			pline("%s", generate_garbage_string());

			{
				const char *line;
				char buflin[BUFSZ];
				if (rn2(2)) line = getrumor(-1, buflin, TRUE);
				else line = getrumor(0, buflin, TRUE);
				if (!*line) line = "Slash'EM rumors file closed for renovation.";
				pline("%s", line);
			}

			pline("%s", generate_garbage_string());

			consume_obj_charge(spellbook, FALSE); /* even if you have good devices skill --Amy */

			if (!rn2(2)) pline("%s", generate_garbage_string());
			if (!rn2(5)) {
				badeffect();
				pline("%s", generate_garbage_string());
			}
			spellbook->in_use = FALSE;

			return(1);
		}

		if (spellbook->spe > 0 && confused && rn2((Role_if(PM_LIBRARIAN) || Role_if(PM_PSYKER)) ? 2 : 10) ) {
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
		} else if (spellbook->spe == 0) spellbook->spe--;

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

		/* Memorization skill by Amy: reduces the time required to read a spellbook, but not below 1 (obviously) */
		if (!PlayerCannotUseSkills && (delay < -1) && P_SKILL(P_MEMORIZATION) >= P_BASIC) {

			int memreduction = 100;

			switch (P_SKILL(P_MEMORIZATION)) {
				case P_BASIC: memreduction = 80; break;
				case P_SKILLED: memreduction = 60; break;
				case P_EXPERT: memreduction = 40; break;
				case P_MASTER: memreduction = 20; break;
				case P_GRAND_MASTER: memreduction = 10; break;
				case P_SUPREME_MASTER: memreduction = 5; break;
			}

			delay *= memreduction;
			delay /= 100;
			if (delay > -1) delay = -1; /* fail safe */

		}

		if (StrongBlind_resistance && delay < -1) {
			delay /= 2;
			if (delay > -1) delay = -1; /* fail safe */
		}

		/* Books are often wiser than their readers (Rus.) */
		spellbook->in_use = TRUE;
		if (!spellbook->blessed &&
		    spellbook->otyp != SPE_BOOK_OF_THE_DEAD) {
		    if ( ((spellbook->cursed && rn2(4)) || (spellbook->spe < 1 && rn2(3)) ) && !Role_if(PM_LIBRARIAN) && !Role_if(PM_PSYKER) && booktype != SPE_BOOK_OF_THE_DEAD ) {
			too_hard = TRUE;
		    } else {
			/* uncursed - chance to fail */
			int read_ability = ACURR(A_INT) + 4 + GushLevel/2
			    - 2*objects[booktype].oc_level
			    + (u.tempbooklenses ? 2 : 0)
			    + ((ublindf && (ublindf->otyp == LENSES || ublindf->otyp == RADIOGLASSES || ublindf->otyp == SHIELD_PATE_GLASSES || ublindf->otyp == BOSS_VISOR)) ? 2 : 0);
			/* only wizards know if a spell is too difficult */
			/* Amy edit: others may randomly know it sometimes */
			if ((Role_if(PM_WIZARD) || !rn2(4)) && read_ability < 20 &&
			    !confused && ((spellbook->spe < 1) || spellbook->cursed)) {
			    char qbuf[QBUFSZ];
			    sprintf(qbuf,
		      "This spellbook is %sdifficult to comprehend. Continue?",
				    (read_ability < 5 ? "extremely " : read_ability < 12 ? "very " : ""));
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

		if ( (too_hard || rn2(2)) && !(booktype == SPE_BOOK_OF_THE_DEAD) && ( (spellbook->cursed && !Role_if(PM_LIBRARIAN) && !Role_if(PM_PSYKER) ) || ((spellbook->spe < 1) && !(booktype == SPE_BOOK_OF_THE_DEAD) ) )) {
		    boolean gone = cursed_book(spellbook);

		    if (delay < 0) {
				if (!issoviet) {

					register int actualdelay;
					actualdelay = rno(-(delay));
					if (!isstunfish && (actualdelay > 1)) actualdelay = isqrt(actualdelay); /* a lot of reduction */

					nomul(-(actualdelay), "reading a cursed book", TRUE); /* study time */
				} else { /* soviet mode */
					nomul((delay), "reading a cursed book", TRUE);
					pline("Vy tol'ko chto podpisal svoy smertnyy prigovor, potomu chto sovetskaya ne zabotitsya o igrovoy balans. Ne dazhe nebol'shoye nemnogo.");
				}

		    }
		    delay = 0;
		    if ((gone || ((spellbook->spe < 0) && !rn2(4)) || !rn2(5)) && !(booktype == SPE_BOOK_OF_THE_DEAD) ) {
			if (!gone && !(booktype == SPE_BOOK_OF_THE_DEAD)) pline_The("spellbook crumbles to dust!");
			else if (!(booktype == SPE_BOOK_OF_THE_DEAD)) pline_The("spellbook has been destroyed.");
			if (!objects[spellbook->otyp].oc_name_known &&
				!objects[spellbook->otyp].oc_uname)
			    docall(spellbook);
				if (carried(spellbook)) useup(spellbook);
				else useupf(spellbook, 1L);
		    } else
			spellbook->in_use = FALSE;
		    return(1);
		} else if (confused && !(Conf_resist && rn2(StrongConf_resist ? 25 : 5)) && !rn2((Role_if(PM_LIBRARIAN) || Role_if(PM_PSYKER)) ? 250 : 50) && spellbook->oartifact != ART_A_LOT_OF_ENYAS_IN_THE_CAST && spellbook->otyp != SPE_BOOK_OF_THE_DEAD) {
		    if (!confused_book(spellbook)) {
			spellbook->in_use = FALSE;
		    }
		    if (delay < 0) {
				if (!issoviet) {
					register int actualdelay;
					actualdelay = rno(-(delay));
					if (actualdelay > 1) actualdelay = isqrt(actualdelay); /* a lot of reduction */

					nomul(-(actualdelay), "reading a book while confused", TRUE);

				} else { /* soviet mode */
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
		    if (spellbook->spe > 0) {
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

			if (spellbook->oartifact == ART_SABINE_S_STUBBORNNESS) {
				u.tempuninterapprf += 50;
				You_feel("like reading the hell out of that book, disregarding anyone who tries to interrupt you.");
			}

			if (spellbook->oartifact == ART_A_LOT_OF_ENYAS_IN_THE_CAST) {
				incr_itimeout(&HConf_resist, rnz(5000));
				You_feel("resistant to confusion.");
			}

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
	if (Keen_memory && !rn2(StrongKeen_memory ? 3 : 5)) /* sync this with cyan spells, see below! --Amy */
		return;

	/* !!! ATTENTION !!! keep *cyan spells* synchronized (much further below)!!! --Amy */

	if (uarm && uarm->oartifact == ART_COPE_OF_THE_ELDRITCH_KNIGH && rn2(2)) return;

	if (uarms && uarms->oartifact == ART_UNUSUAL_ENCH && !rn2(5)) return;

	if (uarm && uarm->oartifact == ART_FLEECELGIRL_FROM_THE_OLD_T && !rn2(2)) return;

	/* the spell color trap that causes your memory to decrease when casting shouldn't be too awfully harsh... --Amy
	 * ATTENTION: cyan spells reduces memory when casting, please keep that synchronized!!! */
	if (SpellColorCyan && rn2(10))
		return;

	/*
	 * The time relative to the hero (a pass through move
	 * loop) causes all spell knowledge to be decremented.
	 * The hero's speed, rest status, conscious status etc.
	 * does not alter the loss of memory.
	 */
	for (i = 0; i < MAXSPELL && spellid(i) != NO_SPELL; i++)
	    if (spellknow(i) ) {

		if (!(uarmc && itemhasappearance(uarmc, APP_GUILD_CLOAK) ) && !(uamul && uamul->oartifact == ART_SUPER_MEMORY) && !tech_inuse(T_SPELL_SPAM) ) {

			/* 10000 turns is nothing compared to the length of the game; have it last longer, please! --Amy */
			if (spellmemorize(i) && !rn2(2)) continue;

			/* higher spell retention means your char is more of a caster and should therefore benefit --Amy */
			if (spellmemorize(i) && (rnd(urole.spelrete + 100) > 100) ) continue;

			/* higher intelligence was doing nothing??? come on! --Amy */
			if (spellmemorize(i) && ACURR(A_INT) > 14) {
				int intelsave = ACURR(A_INT) - 14;
				if (intelsave > rn2(66)) continue;
			}

			/* Memorization skill by Amy: if the spell is set to memorization mode, have a skill-based chance here
			 * that on any given turn the spell memory will not decrease.
			 * cyan spells uses the same calculation below, any changes need to go into that function as well */

			if (!PlayerCannotUseSkills && spellmemorize(i) && P_SKILL(P_MEMORIZATION) >= P_BASIC) {

				int savememochance = 0;

				switch (P_SKILL(P_MEMORIZATION)) {
					case P_BASIC: savememochance = 1; break;
					case P_SKILLED: savememochance = 2; break;
					case P_EXPERT: savememochance = 3; break;
					case P_MASTER: savememochance = 4; break;
					case P_GRAND_MASTER: savememochance = 5; break;
					case P_SUPREME_MASTER: savememochance = 6; break;
				}

				if (savememochance > rn2(10)) continue;

			}

			if (!PlayerCannotUseSkills && spellmemorize(i) && P_SKILL(spell_skilltype(spellid(i))) >= P_BASIC) {

				int savememochance = 0;

				switch (P_SKILL(spell_skilltype(spellid(i)))) {
					case P_BASIC: savememochance = 1; break;
					case P_SKILLED: savememochance = 2; break;
					case P_EXPERT: savememochance = 3; break;
					case P_MASTER: savememochance = 4; break;
					case P_GRAND_MASTER: savememochance = 5; break;
					case P_SUPREME_MASTER: savememochance = 6; break;
				}

				if (savememochance > rn2(20)) continue;

			}

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

		if (!issoviet && !SpellColorCyan && !(SpellForgetting || u.uprops[SPELL_FORGETTING].extrinsic || have_spellforgettingstone()) && !YouHaveLostSpells && spellknow(i) == 1000) pline("Your %s spell is beginning to fade from your memory.", spellname(i));
		if (!issoviet && !SpellColorCyan && !(SpellForgetting || u.uprops[SPELL_FORGETTING].extrinsic || have_spellforgettingstone()) && !YouHaveLostSpells && spellknow(i) == 100) pline("You are about to forget the %s spell.", spellname(i));
		if (!issoviet && !SpellColorCyan && !(SpellForgetting || u.uprops[SPELL_FORGETTING].extrinsic || have_spellforgettingstone()) && !YouHaveLostSpells && spellknow(i) == 0) pline("You no longer know how to cast the %s spell.", spellname(i));

		if (spellknow(i) && uarmc && itemhasappearance(uarmc, APP_FORGETFUL_CLOAK) ) {
			decrnknow(i);
			if (!issoviet && !SpellColorCyan && !(SpellForgetting || u.uprops[SPELL_FORGETTING].extrinsic || have_spellforgettingstone()) && !YouHaveLostSpells && spellknow(i) == 1000) pline("Your %s spell is beginning to fade from your memory.", spellname(i));
			if (!issoviet && !SpellColorCyan && !(SpellForgetting || u.uprops[SPELL_FORGETTING].extrinsic || have_spellforgettingstone()) && !YouHaveLostSpells && spellknow(i) == 100) pline("You are about to forget the %s spell.", spellname(i));
			if (!issoviet && !SpellColorCyan && !(SpellForgetting || u.uprops[SPELL_FORGETTING].extrinsic || have_spellforgettingstone()) && !YouHaveLostSpells && spellknow(i) == 0) pline("You no longer know how to cast the %s spell.", spellname(i));

		}

		if (spellknow(i) && RngeForgetting) {
			decrnknow(i);
			if (!issoviet && !SpellColorCyan && !(SpellForgetting || u.uprops[SPELL_FORGETTING].extrinsic || have_spellforgettingstone()) && !YouHaveLostSpells && spellknow(i) == 1000) pline("Your %s spell is beginning to fade from your memory.", spellname(i));
			if (!issoviet && !SpellColorCyan && !(SpellForgetting || u.uprops[SPELL_FORGETTING].extrinsic || have_spellforgettingstone()) && !YouHaveLostSpells && spellknow(i) == 100) pline("You are about to forget the %s spell.", spellname(i));
			if (!issoviet && !SpellColorCyan && !(SpellForgetting || u.uprops[SPELL_FORGETTING].extrinsic || have_spellforgettingstone()) && !YouHaveLostSpells && spellknow(i) == 0) pline("You no longer know how to cast the %s spell.", spellname(i));

		}

	    }
	return;
}

/*
 * Return TRUE if a spell was picked, with the spell index in the return
 * parameter.  Otherwise return FALSE.
 */
STATIC_OVL boolean
getspell(spell_no, goldspellpossible)
	int *spell_no;
	boolean goldspellpossible;
{
	int nspells, idx, n, thisone, choicenumber;
	char ilet, lets[BUFSZ], qbuf[QBUFSZ];

	if (spellid(0) == NO_SPELL)  {
	    You("don't know any spells right now.");
	    return FALSE;
	}

	if ((Goldspells || u.uprops[GOLDSPELLS].extrinsic || have_goldspellstone()) && goldspellpossible && rn2(10)) {

		for (n = 0; n < MAXSPELL && spellid(n) != NO_SPELL; n++)
			continue;
		if (n) {
			thisone = -1;
			choicenumber = 0;
			for (n = 0; n < MAXSPELL && spellid(n) != NO_SPELL; n++) {
				if (!choicenumber || (!rn2(choicenumber + 1)) ) {
					thisone = n;
				}
				choicenumber++;
			}

			if (choicenumber > 0 && thisone >= 0) {
				pline("You cast %s.", spellname(thisone));
				spelleffects(thisone, FALSE, FALSE);
				if (SpellColorPlatinum && u.uen < 0) {
					u.uenmax -= (0 - u.uen);
					if (u.uenmax < 0) {
						u.uhpmax -= (0 - u.uenmax);
						if (u.uhpmax < 1) u.uhpmax = 1;
						if (Upolyd) {
							u.mhmax -= (0 - u.uenmax);
							if (u.mhmax < 1) u.mhmax = 1;
						}
						losehp((0 - u.uenmax) * 10, "overcasting a spell", KILLED_BY);
						u.uenmax = 0;
					}
					u.uen = 0;
					if (u.uenmax < 0) u.uenmax = 0;
				}
				TimerunBug += 1; /* ugh, ugly hack --Amy */
				return FALSE;
			}
		}
		/* we somehow didn't choose a spell */
		pline("You fail to cast a spell.");
		TimerunBug += 1; /* ugh, ugly hack --Amy */
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
	if (SpellColorPink) return dospellmenu("Your spells are pink.", SPELLMENU_CAST, spell_no, 0);
	else if (SpellColorBrightCyan) return dospellmenu("Your spells are bright cyan.", SPELLMENU_CAST, spell_no, 0);
	else if (SpellColorCyan) return dospellmenu("Your spells are cyan.", SPELLMENU_CAST, spell_no, 0);
	else if (SpellColorBlack) return dospellmenu("Your spells are black.", SPELLMENU_CAST, spell_no, 0);
	else if (SpellColorOrange) return dospellmenu("Your spells are orange.", SPELLMENU_CAST, spell_no, 0);
	else if (SpellColorRed) return dospellmenu("Your spells are red.", SPELLMENU_CAST, spell_no, 0);
	else if (SpellColorPlatinum) return dospellmenu("Your spells are platinum.", SPELLMENU_CAST, spell_no, 0);
	else if (SpellColorSilver) return dospellmenu("Your spells are silver.", SPELLMENU_CAST, spell_no, 0);
	else if (SpellColorMetal) return dospellmenu("Your spells are metal.", SPELLMENU_CAST, spell_no, 0);
	else if (SpellColorGreen) return dospellmenu("Your spells are green.", SPELLMENU_CAST, spell_no, 0);
	else if (SpellColorBlue) return dospellmenu("Your spells are blue.", SPELLMENU_CAST, spell_no, 0);
	else if (SpellColorGray) return dospellmenu("Your spells are completely gray.", SPELLMENU_CAST, spell_no, 0);
	else if (SpellColorBrown) return dospellmenu("Your spells are brown.", SPELLMENU_CAST, spell_no, 0);
	else if (SpellColorWhite) return dospellmenu("Your spells are white.", SPELLMENU_CAST, spell_no, 0);
	else if (SpellColorViolet) return dospellmenu("Your spells are violet.", SPELLMENU_CAST, spell_no, 0);
	else if (SpellColorYellow) return dospellmenu("Your spells are yellow.", SPELLMENU_CAST, spell_no, 0);
	else return dospellmenu("Choose which spell to cast", SPELLMENU_CAST, spell_no, 0);

}

/* the 'Z' command -- cast a spell */
int
docast()
{
	int spell_no;

	int whatreturn;

	/* Amy note: use autismweaponcheck and not uwep here!!! after all, this is a *negative* effect */

	if (u.antimagicshell || (uarmh && uarmh->otyp == HELM_OF_ANTI_MAGIC) || (uwep && uwep->otyp == DARK_SWORD) || (u.twoweap && uswapwep && uswapwep->otyp == DARK_SWORD) || (RngeAntimagicA && (moves % 10 == 0)) || (u.tempantimagic33 && (moves % 3 == 0) ) || (u.tempantimagic10 && (moves % 10 == 0) ) || (RngeAntimagicB && (moves % 5 == 0)) || (RngeAntimagicC && (moves % 2 == 0)) || (RngeAntimagicD) || (uarmc && uarmc->oartifact == ART_SHELLY && (moves % 3 == 0)) || (uarmc && uarmc->oartifact == ART_BLACK_VEIL_OF_BLACKNESS) || (uarmc && uarmc->oartifact == ART_ARABELLA_S_WAND_BOOSTER) || (uarmu && uarmu->oartifact == ART_ANTIMAGIC_SHELL) || autismweaponcheck(ART_LONG_SWORD_OF_ETERNITY) || autismweaponcheck(ART_HEAVY_CROSSBOW_OF_ETERNITY) || (uarmu && uarmu->oartifact == ART_ANTIMAGIC_FIELD) || (Role_if(PM_UNBELIEVER) && !(uwep && uwep->oartifact == ART_MAGE_STAFF_OF_ETERNITY) ) || autismweaponcheck(ART_ARK_OF_THE_COVENANT) || (uarmc && uarmc->oartifact == ART_SPELL_WARDED_WRAPPINGS_OF_) || autismweaponcheck(ART_ANTIMAGICBANE) ) {

		pline("Your anti-magic shell prevents spellcasting.");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return 0;
	}

	if (getspell(&spell_no, TRUE)) {

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

		whatreturn = spelleffects(spell_no, FALSE, FALSE);

		if (powerfulimplants() && uimplant && uimplant->oartifact == ART_SPINAL_MATRIX_OF_RAISTLIN && !rn2(10)) {
			pline_The("spell fires twice!");
			spelleffects(spell_no, FALSE, FALSE);
		}
		if (uarmh && uarmh->oartifact == ART_ANTON_S_CROWN_OF_ECHOES && !rn2(10)) {
			pline_The("spell fires twice!");
			spelleffects(spell_no, FALSE, FALSE);
		}

		if (SpellColorPlatinum && u.uen < 0) {
			u.uenmax -= (0 - u.uen);
			if (u.uenmax < 0) {
				u.uhpmax -= (0 - u.uenmax);
				if (u.uhpmax < 1) u.uhpmax = 1;
				if (Upolyd) {
					u.mhmax -= (0 - u.uenmax);
					if (u.mhmax < 1) u.mhmax = 1;
				}
				losehp((0 - u.uenmax) * 10, "overcasting a spell", KILLED_BY);
				u.uenmax = 0;
			}
			u.uen = 0;
			if (u.uenmax < 0) u.uenmax = 0;
		}
		else if (u.uen < 0) {
			int manafiredamage = abs(u.uen);
			u.uen = 0;
			losehp(manafiredamage, "mana fire", KILLED_BY);
			You("took %d points of damage from mana fire.", manafiredamage);
		}

		return whatreturn;
	}
	return 0;
}

void
castinertiaspell()
{
	char buf[BUFSZ];
	char c;
	boolean willcastinertiaspell = FALSE;

	pline("You control the %s spell flow.", spellname(u.inertiacontrolspellno));

	if (flags.inertiaconfirm) {
		getlin ("Cast it? [y/yes/no/q/quit]",buf);
		(void) lcase (buf);
		if (!(strcmp (buf, "yes")) || !(strcmp (buf, "y")) || !(strcmp (buf, "ye")) || !(strcmp (buf, "ys"))) willcastinertiaspell = TRUE;
		if (!(strcmp (buf, "q")) || !(strcmp (buf, "quit"))) {
			You("ended inertia control prematurely.");
			u.inertiacontrol = 0;
			u.inertiacontrolspell = -1;
			u.inertiacontrolspellno = -1;
			return;
		}
	} else {
		c = yn_function("Cast it?", ynqchars, 'n');
		if (c == 'y') willcastinertiaspell = TRUE;
		else if (c == 'q') {
			You("ended inertia control prematurely.");
			u.inertiacontrol = 0;
			u.inertiacontrolspell = -1;
			u.inertiacontrolspellno = -1;
			return;
		}
	}

	if (willcastinertiaspell) {

		if (spellid(u.inertiacontrolspellno) != u.inertiacontrolspell) {
			pline("The inertia controlled spell is no longer in place, and therefore cannot be auto-casted!");
			u.inertiacontrol = 0;
			u.inertiacontrolspell = -1;
			u.inertiacontrolspellno = -1;
			return;
		}
		spelleffects(u.inertiacontrolspellno, FALSE, FALSE);

		if (SpellColorPlatinum && u.uen < 0) {
			u.uenmax -= (0 - u.uen);
			if (u.uenmax < 0) {
				u.uhpmax -= (0 - u.uenmax);
				if (u.uhpmax < 1) u.uhpmax = 1;
				if (Upolyd) {
					u.mhmax -= (0 - u.uenmax);
					if (u.mhmax < 1) u.mhmax = 1;
				}
				losehp((0 - u.uenmax) * 10, "overcasting a spell", KILLED_BY);
				u.uenmax = 0;
			}
			u.uen = 0;
			if (u.uenmax < 0) u.uenmax = 0;
		}

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
	int l = GushLevel;
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
	if (Race_if(PM_MAYMES)) gain *= 2;

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

	    if (Race_if(PM_MAYMES)) {
		u.uspmtime *= 2;
	    }

	    if (!u.usptime)
		u.usptime = u.uspmtime;

	    find_ac();
	} else {
	    Your("%s feels warm for a moment.", body_part(BODY_SKIN));
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

STATIC_OVL void
handleartifactspellbook(book2)
struct obj *book2;
{
	if (!book2) {
		impossible("handleartifactspellbook called without book?!");
		return;
	}

	/* Amy dummy for copypasting when new artifact books are added

	if (book2->oartifact == ART_) {

	}

	*/

	if (book2->oartifact == ART_PURPLE_RAIN__PURPLE_RAIN__) {
		register struct monst *prainmon;
		int purplerain = rn1(3,2);
		while (purplerain > 0) {
			purplerain--;
			prainmon = makemon(&mons[PM_PURPLE_WORM], 0, 0, MM_ADJACENTOK);
			if (prainmon) {
				prainmon = tamedog(prainmon, (struct obj *) 0, TRUE);
			}
		}
	}

	if (book2->oartifact == ART_LOVE_IS_A_GREAT_STORY__LOV) {
		int peacefulmons = rnd(7);
		while (peacefulmons > 0) {
			peacefulmons--;
			(void) makemon(specialtensmon(115), u.ux, u.uy, MM_ADJACENTOK); /* M2_PEACEFUL */
		}
		(void) makemon(specialtensmon(320), u.ux, u.uy, MM_ADJACENTOK); /* M4_TAME */

	}

	if (book2->oartifact == ART_TELE_FANTASTIC) {
		u.temptelefantastic += rnz(10000);
		You("gain fantastic teleportitis!");
	}

	if (book2->oartifact == ART_WORLD_WILL_BE_REDONE_BY_A_) {
		makemon(&mons[PM_FEMALE_SYMBOL], 0, 0, MM_ADJACENTOK);
	}

	if (book2->oartifact == ART_MY_CRUDE_DISTORTION) {
		u.antitelespelltimeout += rnz(5000);
		pline("An anti-teleportation field has been activated!");
	}

	if (book2->oartifact == ART_BAD_BEY) {
		You("hold your breath.");
		incr_itimeout(&HMagical_breathing, 10000);
	}

	if (book2->oartifact == ART_DEMO_HAS_BOGUS_NUMBER_OF_D) {
		u.tempnoctrlp += rnz(5000);
	}

	if (book2->oartifact == ART_I__GONNA_SUCK_UP_THE_SUN__) {
		pline_The("sun comes out.");
		u.currentweather = WEATHER_SUNNY;
		tell_main_weather();
	}

	if (book2->oartifact == ART_CHANGE_CHANGE_CHANGE_CHANG) {
		if (rn2(2)) {
			pline("It starts to rain.");
			u.currentweather = WEATHER_RAIN;
		} else {
			pline("A thunderstorm comes in!");
			u.currentweather = WEATHER_THUNDERSTORM;
		}
		tell_main_weather();

	}

	if (book2->oartifact == ART_IN_THE_BED_GEWICKED) {
		u.tempssex5 += 10000;
		incr_itimeout(&HSleep_resistance, 10000);
		if (FunnyHallu)
			pline("Too much coffee!");
		else
			You("no longer feel tired.");

	}

	if (book2->oartifact == ART_LEVALA_STING_LOVE) {
		if (!FemtrapActiveRuth) pline("You just know that your legs are going to be ripped open by very sharp-edged combat boot heels.");
		FemaleTrapSandra += 10000;
		(void) adjattrib(A_CHA, 1, FALSE, TRUE);
	}

	if (book2->oartifact == ART_WHEN_THE_CITY_SWEEPS) {
		u.monstermultiplier = 190;
		u.currentweather = WEATHER_SANDSTORM;
		pline("There's a storm sweeping into the dungeon...");
		tell_main_weather();
	}

	if (book2->oartifact == ART_YOU_SMELL_GEESH_MAMA) {
		u.monstermultiplier = 10;
		u.currentweather = WEATHER_HAIL;
		pline("Suddenly, the dungeon feels really icy...");
		tell_main_weather();
	}

	if (book2->oartifact == ART_MEMORABLE_READ) {
		use_skill(P_MEMORIZATION, 20);
	}

	if (book2->oartifact == ART_NUKA_ROLL) {
		int nukaroll = rnd(110); /* keyword: "marlena" */

		u.tempnukaefftype = nukaroll;
		u.tempnukaeffect += rnz(5000);

		pline_The("bottle of Nuka Cola is named %s today!", feminismtrapname(nukaroll));

		lesshungry(500);
	}

	if (book2->oartifact == ART_O_SACCADO) {
		use_skill(P_SPIRITUALITY, 100);
		if (u.ublesscnt < 1001) u.ublesscnt = 0;
		else u.ublesscnt /= 2;
		You_feel("the gods watching over you.");
	}

	if (book2->oartifact == ART_ANGEL_LACK_BOWAY) {

		u.aggravation = 1;
		u.heavyaggravation = 1;

		register struct monst *specialangel;
		specialangel = makemon(mkclass(S_ANGEL,0), 0, 0, MM_NOSPECIALS);
		if (specialangel) {
			u.mongetshack = 33; /* make sure the items can drop regardless of timebasedlowerchance crap --Amy */
			(void) mongets(specialangel, LACQUERED_PLATE_MAIL);
			(void) mongets(specialangel, rnd_class(ORCISH_BOW, WILDHILD_BOW));
			u_teleport_monD(specialangel, FALSE);

			u.mongetshack = 0;
		}

		u.aggravation = 0;
		u.heavyaggravation = 0;

	}

	if (book2->oartifact == ART_FREE_OF_WEED) {
		(void) make_hallucinated(0L, TRUE, 0L);
	}

	if (book2->oartifact == ART_THIRST_HOLES) {

		Thirst += rnz(5000);

		if (levl[u.ux][u.uy].typ != ROOM && levl[u.ux][u.uy].typ != CORR) {
			You_feel("claustrophobic!");
		} else {
			pline("You build a fountain.");
			levl[u.ux][u.uy].typ = FOUNTAIN;
		}

		u.tempfountainbuild += 5000;
	}

	if (book2->oartifact == ART_RRRRRRRRRRRRHHH__RRRRRRRRR) {
		u.breathenhancetimer += 5000;
		Your("breath is magically enhanced!");
	}

	if (book2->oartifact == ART_MORTON_S_ICECAVE_PRICKLE) {
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_crystalwaterfloodgX, (void *)&madepoolQ);

	}

	if (book2->oartifact == ART_I_M_GONNA_TELL_MY_HEART) {
		incr_itimeout(&HDiminishedBleeding, 10000);
		pline("Your %s is boiling!", body_part(BLOOD));
	}

	if (book2->oartifact == ART_SYLVIE_S_YAWNING_HISTORY) {
		fall_asleep(-(rn1(20, 20)), TRUE);
		upnivel(FALSE);
	}

	if (book2->oartifact == ART_RADIUS_REQUEST_TIMED_OUT) {
		u.powerfailure += 5000;
		u.tempradiusto += 5000;
		pline("Radius request timed out.");
	}

	if (book2->oartifact == ART_WA_WA__WA_WA__WA_WA_) {
		incr_itimeout(&HFear_factor, 10000);
		You_feel("like running away from the enemy!");
	}

	if (book2->oartifact == ART_LIVE_IN_THE_SENSE_OF_YOUR_) {
		AEXE(A_WIS) += 10;
		if (AEXE(A_WIS) > 50) AEXE(A_WIS) = 50;
	}

	if (book2->oartifact == ART_NICHT_SPEER_WERFEN) {
		u.tempnospearthrow += 10000;
		pline("Spears can no longer be thrown.");
	}

	if (book2->oartifact == ART_AND_THEN_THE_TESTER_CAME) {
		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		(void) makemon(specialtensmon(430), 0, 0, MM_ADJACENTOK); /* AD_DEBT */

		u.aggravation = 0;

	}

	if (book2->oartifact == ART_CANCELLATOION) {
		(void) cancel_monst(&youmonst, (struct obj *)0, FALSE, TRUE, FALSE);
	}

	if (book2->oartifact == ART_CAMA_CAMA_CAMA_MAYBE) {
		if (rn2(2)) {
			change_luck(1);
			You_feel("lucky.");
		} else {
			change_luck(-1);
			You_feel("unlucky.");
		}
	}

	if (book2->oartifact == ART_FOUR_ON_THE_FLOOR__I_WILL_) {
		u.tempnoflysee += 5000;
		You("can no longer fly, but are capable of seeing any monsters that can.");
	}

	if (book2->oartifact == ART_STRETCHER_CAPACITY) {
		u.tempcarrcap1000 += 10000;
		You("can carry more stuff for a while!");
	}

	if (book2->oartifact == ART_AND_I_LOVE_THE_BAUR) {
		adjattrib(A_STR, 1, FALSE, TRUE);
		if (!FemtrapActiveRuth) pline("Those farmer girls plan to use block-heeled lady boots, wooden sandals and scentful sneakers on you!");
		FemaleTrapKerstin += rnz(5000);
	}

	if (book2->oartifact == ART_CONUNDRAL_PLAYBACK) {
		int conundralitem = DAGGER;
		int attempts = 50000;
		struct obj *uammo;

		while (attempts > 1) { /* roll random equipment pieces until one made of conundrum is rolled */
			attempts--;

			switch (rnd(21)) {
				case 1:
					conundralitem = randartmeleeweapon();
					break;
				case 2:
					conundralitem = randartlauncher();
					break;
				case 3:
					conundralitem = randartmissile();
					break;
				case 4:
					conundralitem = randartlightsaber();
					break;
				case 5:
					conundralitem = randartunihorn();
					break;
				case 6:
					conundralitem = randartpickaxe();
					break;
				case 7:
					conundralitem = randartsuit();
					break;
				case 8:
					conundralitem = randartshirt();
					break;
				case 9:
					conundralitem = randartcloak();
					break;
				case 10:
					conundralitem = randarthelm();
					break;
				case 11:
					conundralitem = randartgloves();
					break;
				case 12:
					conundralitem = randartshield();
					break;
				case 13:
					conundralitem = randartboots();
					break;
				case 14:
					conundralitem = randartring();
					break;
				case 15:
					conundralitem = randartamulet();
					break;
				case 16:
					conundralitem = randartimplant();
					break;
				case 17:
					conundralitem = randartwand();
					break;
				case 18:
					conundralitem = randartball();
					break;
				case 19:
					conundralitem = randartchain();
					break;
				case 20:
					conundralitem = randartgem();
					break;
				case 21:
					conundralitem = randartvenom();
					break;
			}

			if (objects[conundralitem].oc_material == MT_CONUNDRUM) break; /* take this one */

		}

		uammo = mksobj(conundralitem, TRUE, FALSE, FALSE);
		if (uammo) {
			uammo->owt = weight(uammo);
			dropy(uammo);
			stackobj(uammo);
			pline("An item made of conundrum has been dropped on the ground!");
		}

	}

	if (book2->oartifact == ART_YOU_LOOK_LIKE_YOU_HAVE_SEE) {

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		coord dd;
		coord cc;
		int cx,cy;
		int i;
		int randsp = rn1(5,5);
		cx = rn2(COLNO);
		cy = rn2(ROWNO);

		for (i = 0; i < randsp; i++) {

			if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;

			(void) makemon(specialtensmon(322), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
		}

		u.aggravation = 0;
	}

	if (book2->oartifact == ART_AMY__WHAT_THE_HELL_IS_A_BE) {

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		coord dd;
		coord cc;
		int cx,cy;
		int i;
		int randsp = rnz(10);
		cx = rn2(COLNO);
		cy = rn2(ROWNO);

		for (i = 0; i < randsp; i++) {

			if (!enexto(&dd, u.ux, u.uy, (struct permonst *)0) ) continue;

			(void) makemon(beehivemon(), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
		}

		u.aggravation = 0;
	}

	if (book2->oartifact == ART_READ_OUT_EKE) {
		if (Upolyd) {
			u.mh++;
			u.mhmax++;
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		} else {
			u.uhp++;
			u.uhpmax++;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		}
		if (uinsymbiosis) {
			u.usymbiote.mhpmax++;
			maybe_evolve_symbiote();
			if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
		}
		You_feel("vitalized.");
		flags.botl = 1;
	}

	if (book2->oartifact == ART_BAD_LSD_TRIP__HAHAHAHAH) {
		incr_itimeout(&HStone_resistance, 500);
		You_feel("more limber. Let's eat some cockatrice meat!");
	}

	if (book2->oartifact == ART_Q___WHA_BOYS) {
		u.temprumormessages += rnz(3000);
	}

	if (book2->oartifact == ART_GREENE_BLIEGE) {
		u.greencrosschance = rnd(20);
	}

	if (book2->oartifact == ART_STD_IDENTIFY) {
		You_feel("self-knowledgeable...");
		display_nhwindow(WIN_MESSAGE, FALSE);
		enlightenment(FALSE, TRUE);
		pline_The("feeling subsides.");
		exercise(A_WIS, TRUE);
	}

	if (book2->oartifact == ART_GRAIN_FARMING__CLICK_) {
		int i;

		for (i = 0; i < MAXSPELL && spellid(i) != NO_SPELL; i++) {
			if (spellknow(i)) {
				if (spellknow(i) > 20000) {
					spellknow(i) = 20000;
				} else if (spellknow(i) < 10200) {
					boostknow(i, 5000);
				}
			}
		}
		You_feel("a redistribution of spell memory.");
	}

	if (book2->oartifact == ART_BEGINNER_S_PRIMER) {
		int i;

		for (i = 0; i < MAXSPELL && spellid(i) != NO_SPELL; i++) {
			if (spellknow(i)) {
				if (spellev(i) == 1) {
					boostknow(i, 2000);
				}
			}
		}
		You_feel("an increase of your basic spell knowledge.");
	}

	if (book2->oartifact == ART_NORTHHOME) {
		u.tempgoodsuppreffects += 2500;
	}

	if (book2->oartifact == ART_SEVENTH_GAME__DEDEDE_) {
		int i;

		for (i = 0; i < MAXSPELL && spellid(i) != NO_SPELL; i++) {
			if (spellknow(i)) {
				if (spellev(i) == 7) {
					boostknow(i, 10000);
				} else if (spellev(i) < 7) {
					spellknow(i) -= 10000;
					if (spellknow(i) < 0) spellknow(i) = 0;
				}
			}
		}
		You_feel("like casting some high-level spells.");
	}

	if (book2->oartifact == ART_CODEX_OF_ECHOES) {
		int i;

		for (i = 0; i < MAXSPELL && spellid(i) != NO_SPELL; i++) {
			if (spellknow(i)) {
				boostknow(i, 500);
			}
		}
		You_feel("an increase of your spell knowledge.");

		u.tempenemyspell33 += rnz(2000);
		pline("All enemy spellcasters are inhibited for a while, and may randomly fail their spells.");

	}

	if (book2->oartifact == ART_AURORA_CODEX) {
		u.uenmax += 10;
		flags.botl = TRUE;
		You("reinforce your memory...");
		use_temporary_tech(T_REINFORCE);
		u.tempspellcost90 += 2000;
	}

	if (book2->oartifact == ART_ASKING_TELNET_PLESCHE) {
		LatencyBugEffect += rn1(500,500);
		incr_itimeout(&HDetect_monsters, 100);
		see_monsters();
	}

	if (book2->oartifact == ART_I_DISCOVERED_EQUALMUCH) {
		int nivelamount = (u.uhpmax += u.uenmax) / 2;
		if (nivelamount < 1) nivelamount = 1;
		u.uhpmax = u.uenmax = nivelamount;
		if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		if (u.uen > u.uenmax) u.uen = u.uenmax;
		flags.botl = TRUE;
		Your("HP and Pw maximums have been nivellated.");
	}

	if (book2->oartifact == ART_DOESMUJETE) {
		if (FemtrapActiveJanet) {
			FemaleTrapJanet += rnz(10000);
			if (u.tempsuperfemtrap_janet < FemaleTrapJanet) u.tempsuperfemtrap_janet = FemaleTrapJanet;
		} else {
			FemaleTrapJanet += rnz(10000);
		}
		if (!FemtrapActiveRuth) pline("You get a bad feeling about the attacks of those women all of a sudden...");

		if (P_MAX_SKILL(P_SQUEAKING) == P_ISRESTRICTED) {
			unrestrict_weapon_skill(P_SQUEAKING);
			P_MAX_SKILL(P_SQUEAKING) = P_SKILLED;
			pline("You can now learn the squeaking skill!");
		} else if (P_MAX_SKILL(P_SQUEAKING) == P_UNSKILLED) {
			unrestrict_weapon_skill(P_SQUEAKING);
			P_MAX_SKILL(P_SQUEAKING) = P_SKILLED;
			pline("You can now learn the squeaking skill!");
		} else if (P_MAX_SKILL(P_SQUEAKING) == P_BASIC) {
			P_MAX_SKILL(P_SQUEAKING) = P_SKILLED;
			pline("You can now become skilled in squeaking!");
		} else if (!rn2(3) && (P_MAX_SKILL(P_SQUEAKING) == P_SKILLED)) {
			P_MAX_SKILL(P_SQUEAKING) = P_EXPERT;
			pline("You can now become expert in squeaking!");
		} else if (!rn2(3) && (P_MAX_SKILL(P_SQUEAKING) == P_EXPERT)) {
			P_MAX_SKILL(P_SQUEAKING) = P_MASTER;
			pline("You can now become master in squeaking!");
		} else if (!rn2(3) && (P_MAX_SKILL(P_SQUEAKING) == P_MASTER)) {
			P_MAX_SKILL(P_SQUEAKING) = P_GRAND_MASTER;
			pline("You can now become grand master in squeaking!");
		} else if (!rn2(3) && (P_MAX_SKILL(P_SQUEAKING) == P_GRAND_MASTER)) {
			P_MAX_SKILL(P_SQUEAKING) = P_SUPREME_MASTER;
			pline("You can now become supreme master in squeaking!");
		} else if (P_MAX_SKILL(P_SQUEAKING) >= P_SUPREME_MASTER) {
			pline("Sadly your knowledge of the squeaking skill is already maxed.");
		}

		use_skill(P_SQUEAKING, rnz(50));

	}

	if (book2->oartifact == ART_CELEBRATION_EVENING) {
		if (eveningtime()) {

			pline("It's celebration evening, which is actually specific to Germany for some reason because the Americans don't celebrate when their working hours are over (wtf)!");
			if (FunnyHallu) pline("The German 'Feierabend' doesn't even have a specific expression in the English language for that reason...");

			if (HWinceState & TIMEOUT) {
				int wincecure = (HWinceState & TIMEOUT);
				if (wincecure > 5000) wincecure = 5000;

				HWinceState -= wincecure;

				if (!HWinceState) {
					You("no longer have to wince.");
				} else {
					You("seem to be wincing a bit less.");
				}

			}

			if (HInvertedState & TIMEOUT) {
				int invertcure = (HInvertedState & TIMEOUT);
				if (invertcure > 5000) invertcure = 5000;

				HInvertedState -= invertcure;

				if (!HInvertedState) {
					You("manage to stand up.");
				} else {
					You("feel a bit better, but cannot stand up yet.");
				}
			}

			if (HBurdenedState & TIMEOUT) {
				int burdencure = (HBurdenedState & TIMEOUT);
				if (burdencure > 5000) burdencure = 5000;

				HBurdenedState -= burdencure;

				if (!HBurdenedState) {
					You("feel lighter!");
				} else {
					You("still feel burdened, but hope that it'll end soon.");
				}
			}

			if (u.inertia) {
				if (u.inertia < 5001) {
					u.inertia = 0;
					You("don't feel lazy anymore!");
				} else {
					u.inertia -= 5000;
					You_feel("a bit less lazy.");
				}
			}
		}
	}

	if (book2->oartifact == ART_CODEX_OF_THE_ENDLESS_LABYR) {
		tele();
		if (!level.flags.nommap) {
			do_mapping();
		}
	}

	if (book2->oartifact == ART_HIGH_ABOVE_THE_CLOUDS) {
		int ulx, uly;
		for (ulx = 1; ulx < (COLNO); ulx++)
		  for (uly = 0; uly < (ROWNO); uly++) {
			if (levl[ulx][uly].typ == MOORLAND || levl[ulx][uly].typ == URINELAKE || levl[ulx][uly].typ == SHIFTINGSAND || levl[ulx][uly].typ == LAVAPOOL || levl[ulx][uly].typ == STYXRIVER) {
				if (!rn2(5)) {
					levl[ulx][uly].typ = CLOUD;
					newsym(ulx, uly);
					blockorunblock_point(ulx,uly);
				}
			}
		}

		int i, j, bd = 3;

		for(i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if (levl[u.ux + i][u.uy+ j].typ == MOORLAND || levl[u.ux + i][u.uy + j].typ == URINELAKE || levl[u.ux + i][u.uy + j].typ == SHIFTINGSAND || levl[u.ux + i][u.uy + j].typ == LAVAPOOL || levl[u.ux + i][u.uy + j].typ == STYXRIVER) {
				levl[u.ux + i][u.uy + j].typ = CLOUD;
					newsym(u.ux + i, u.uy + j);
					blockorunblock_point(u.ux + i,u.uy + j);
			}
		}
		You_feel("the liquids turning into clouds.");
	}

	if (book2->oartifact == ART_DAEDAEDAEDAEDAEDAEUEUEUEUE) {
		int i, j, bd = 5;
		struct monst *mtmp;

		u.youaredead = 1;
		pline("Daedaedaedaedaedaeueueueueuei! %s exploded and died!", playeraliasname);

		for(i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if ((mtmp = m_at(u.ux + i, u.uy + j)) != 0) {
				mtmp->mhp -= rnd(u.uhpmax * 10);
				pline("%s is hit by the explosion!", Monnam(mtmp));
				if (mtmp->mhp <= 0) {
					pline("%s is killed!", Monnam(mtmp));
					xkilled(mtmp, 0);
				} else wakeup(mtmp); /* monster becomes hostile */
			}

		}

		killer = "exploding";
		killer_format = KILLED_BY;
		done(DIED);
		u.youaredead = 0;
	}

	if (book2->oartifact == ART_CODEX_PANDEMONIUM) {
		spell_metronome(1, 8);
		spell_metronome(1, 8);
	}

	if (book2->oartifact == ART_I_SEE_NO_REASON_BECAUSE_TH) {
		if (week_day() == 1) {
			forget(1, FALSE);
			u.tempincrdmg1 += 5000;
			You_feel("dizzy and somewhat angry.");
		}
	}

	if (book2->oartifact == ART_MATPLAS_OLGE) {
		struct obj *uammo;
		uammo = mksobj(!rn2(10) ? SCR_SUPERIOR_MATERIAL : !rn2(3) ? SCR_REGULAR_MATERIAL : SCR_INFERIOR_MATERIAL, TRUE, FALSE, FALSE);
		if (uammo) {
			uammo->quan = 1;
			uammo->owt = weight(uammo);
			dropy(uammo);
			stackobj(uammo);
			pline("A scroll has formed on the ground!");
		}
	}

	if (book2->oartifact == ART_WHAT_IF_I_NEED_TO_KNOW_) {
		struct obj *uammo;
		uammo = mksobj(SCR_STANDARD_ID, TRUE, FALSE, FALSE);
		if (uammo) {
			uammo->quan = rnd(5);
			uammo->owt = weight(uammo);
			dropy(uammo);
			stackobj(uammo);
			pline("A stack of standard identify scrolls has formed on the ground!");
		}
	}

	if (book2->oartifact == ART_CLINICAL_CLEANLINESSES) {
		You_feel("an anti-sexual aura.");
		u.sterilized += 2000;
	}

	if (book2->oartifact == ART_BUILDING_M_WORK) {
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_lockfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("The area is walled off!");
		madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_treefloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("Trees start to grow rapidly!");

	}

	if (book2->oartifact == ART_ORANI_IS_SOMEBODY) {
		(void) make_familiar((struct obj *)0, u.ux, u.uy, FALSE, FALSE);
	}

	if (book2->oartifact == ART_MOE_S_COLLECTION) {
		struct obj *uammo;
		uammo = mksobj(CARROT, TRUE, FALSE, FALSE);
		if (uammo) {
			uammo->quan = rn1(5,5);
			uammo->owt = weight(uammo);
			dropy(uammo);
			stackobj(uammo);
			pline("A stack of carrots has formed on the ground!");
		}
	}

	if (book2->oartifact == ART_THAT_S_SO_MEDIEVAL_) {
		struct obj *uammo;
		int medievalweapontype = SPEAR;

		switch (rnd(20)) {
			case 1:
				medievalweapontype = randartpolearm();
				break;
			case 2:
				medievalweapontype = randartlongsword();
				break;
			case 3:
				medievalweapontype = randartbroadsword();
				break;
			case 4:
				medievalweapontype = randartshortsword();
				break;
			case 5:
				medievalweapontype = randartscimitar();
				break;
			case 6:
				medievalweapontype = randarttrident();
				break;
			case 7:
				medievalweapontype = randartflail();
				break;
			case 8:
				medievalweapontype = randartlance();
				break;
			case 9:
				medievalweapontype = randartcrossbow();
				break;
			case 10:
				medievalweapontype = randarthammer();
				break;
			case 11:
				medievalweapontype = randartmorningstar();
				break;
			case 12:
				medievalweapontype = randartaxe();
				break;
			case 13:
				medievalweapontype = randartspear();
				break;
			case 14:
				medievalweapontype = randarttwohandedsword();
				break;
			case 15:
				medievalweapontype = randartmace();
				break;
			case 16:
				medievalweapontype = randartdagger();
				break;
			case 17:
				medievalweapontype = randartquarterstaff();
				break;
			case 18:
				medievalweapontype = randartbow();
				break;
			case 19:
				medievalweapontype = randartjavelin();
				break;
			case 20:
				medievalweapontype = randartsaber();
				break;
		}

		uammo = mksobj(medievalweapontype, TRUE, FALSE, FALSE);
		if (uammo) {
			uammo->quan = 1;
			uammo->owt = weight(uammo);
			dropy(uammo);
			stackobj(uammo);
			pline("A medieval weapon has formed on the ground!");
		}

	}

	if (book2->oartifact == ART_FIELD_GUIDE_TO_FAMILIAR_SP) {
		(void) make_familiar((struct obj *)0, u.ux, u.uy, FALSE, FALSE);
	}

	if (book2->oartifact == ART_YOU_SIR_ARE_A_LO_SIR) {
		make_feared(HFeared + 200, FALSE);
		You_feel("like a loser.");
	}

	if (book2->oartifact == ART_QUIET_NOTEBOOK) {
		make_confused(0L, TRUE);
		make_stunned(0L, TRUE);
		u.tempspellchance20 += 500;
		You("can cast spells more easily for a while.");
	}

	if (book2->oartifact == ART_RUNIC_LEXICON_OF_ELDAMIR) {
		u.tempdblspelldmg += 5000;
		You("feel your spell power increasing greatly!");

		if (!rn2(10)) {
			int spell_no;

			pline("Select a spell to increase spell memory.");

			if (getspell(&spell_no, FALSE)) {
				if (spellknow(spell_no) <= 0) {
					You("are unable to focus your memory of the spell.");
					return;
				} else {
					Your("focus and reinforce your memory of the spell.");
					incrnknow(spell_no, FALSE);
				}
			}

		}
	}

	if (book2->oartifact == ART_FULLY_CHAOTIC_MONTE) {

		u.tempspelllossmagvac += 200000;
		You("feel like you're not gonna be able to use your spells effectively for a good long while...");

		int chaoticmontespell = SPE_FORCE_BOLT;
		while (chaoticmontespell <= SPE_PSYBEAM) {
			if (objects[chaoticmontespell].oc_subtyp == P_CHAOS_SPELL) wonderspell(chaoticmontespell, 0);
			chaoticmontespell++;
		}
	}

	if (book2->oartifact == ART_RODIN_THINKS_CRITICAL_HITS) {
		u.tempcritical10 += 5000;
		You_feel("that critical hits are beautiful!");
	}

	if (book2->oartifact == ART_ES_TWO_TO_EK) {
		u.tempspiderspawns += 5000;
	}

	if (book2->oartifact == ART_MORE_COMMERCIAL_GOES_REALL) {
		int newloudspeakers = rnz(5);
		int tryct = 0;
		int x, y;

		while (newloudspeakers > 0) {
			newloudspeakers--;
			for (tryct = 0; tryct < 2000; tryct++) {
				x = rn1(COLNO-3,2);
				y = rn2(ROWNO);

				if (isok(x, y) && !(t_at(x, y)) ) {
					(void) maketrap(x, y, LOUDSPEAKER, 0, FALSE);
					break;
				}
			}
		}
	}

	if (book2->oartifact == ART_YOUR_BUTT_HAS_A_TRACK_CLOS) {
		u.tempnofartloud += 10000;
		You_feel("that you won't hear any loud farting noises for a while.");
		if (FunnyHallu) pline("Probably the monsters' butts have a track closure.");
	}

	if (book2->oartifact == ART_CODEX_OF_MEDEA) {
		wonderspell(SPE_TOXIC, 0);
		incr_itimeout(&HPoison_resistance, 5000);
		You_feel("resistant to poison.");

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		int medeamons = rnd(6);

		while (medeamons > 0) {
			medeamons--;
			makemon(mkclass(S_BAD_FOOD,0), 0, 0, MM_XFRENZIED);
		}

		medeamons = rnd(6);

		while (medeamons > 0) {
			medeamons--;
			makemon(mkclass(S_FUNGUS,0), 0, 0, MM_XFRENZIED);
		}

		u.aggravation = 0;
	}

	if (book2->oartifact == ART_SCRIPTURE_OF_THE_SILENT_CH) {
		pline("You produce an anti-magic shell.");
		u.antimagicshell += 100;
		u.tempantimagic10 += 5000;
		You_feel("the presence of a weak antimagic aura.");
	}

	if (book2->oartifact == ART_FRACTURED_CODEX) {
		alter_reality(0);
	}

	if (book2->oartifact == ART_APPRENTICE_S_LEXICON) {
		u.tempintwisboost2 += 200;
		You_feel("smarter and wiser for a while.");
		if (!rn2(5)) {
			You("reinforce your memory...");
			use_temporary_tech(T_REINFORCE);
		}
	}

	if (book2->oartifact == ART_CHRONICLE_OF_THOTH) {
		u.tempintwisboost2 += 5000;
		You_feel("smarter and wiser for a while.");
		use_skill(P_MEMORIZATION, 30);
		if (!rn2(5)) {
			if(invent) {
				identify_pack(1, 0, 0);
			}
		}
	}

	if (book2->oartifact == ART_BOOK_OF_THE_ASHEN_STAR) {
		coord cc;
		struct monst *psychmonst;
		pline("Select a monster to strike with a meteor");
		cc.x = u.ux;
		cc.y = u.uy;
		getpos(&cc, TRUE, "the spot to attack");
		if (cc.x == -10) return; /* user pressed esc */
		psychmonst = m_at(cc.x, cc.y);

		if (!psychmonst || (!canseemon(psychmonst) && !canspotmon(psychmonst))) {
			You("don't see a monster there!");
			return;
		}

		if (psychmonst) {

			pline("%s is hit by a meteor!", Monnam(psychmonst));

			if (resists_fire(psychmonst)) {
				pline("But %s is unaffected!", mon_nam(psychmonst));
			} else {
				psychmonst->mhp -= 100;
				psychmonst->mhpmax -= rnd(3);
				if (psychmonst->mhpmax < 1) psychmonst->mhpmax = 1;
				if (psychmonst->mhp < 1) {
					pline("%s dies!", Monnam(psychmonst));
					xkilled(psychmonst,0);
				} else wakeup(psychmonst); /* monster becomes hostile */
			}

		}
		u.tempfirelightspellbonus += 5000;
		You_feel("the power of fire and light spells increasing.");
	}

	if (book2->oartifact == ART_SOLOMON_S_KEY_OF_WISDOM) {
		int i, j, bd = 3;
		struct monst *mtmp;

		for(i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if ((mtmp = m_at(u.ux + i, u.uy + j)) != 0 && (is_demon(mtmp->data))) {

			    if (!resist(mtmp, SPBOOK_CLASS, 0, NOTELL)) {
				(void) tamedog(mtmp, (struct obj *) 0, FALSE);
			    }
			}
		}
	}

	if (book2->oartifact == ART_UNCURSEONE) {
		if (uwep && !stack_too_big(uwep)) uncurse(uwep, FALSE);
	}

	if (book2->oartifact == ART_PALLYHACK) {
		u.holyshield += 200;
		You("activate your holy shield.");
	}

	if (book2->oartifact == ART_BLUE_ROUGE_DROP) {
		u.magicshield += 2000;
		pline("You activate your magic shield!");
	}

	if (book2->oartifact == ART_SLOWUNG_FIELD) {
		int i, j, bd = 4;
		struct monst *mtmp;

		for(i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if ((mtmp = m_at(u.ux + i, u.uy + j)) != 0) {

			    if (mtmp->mpeaceful) continue;

			    if (!resist(mtmp, SPBOOK_CLASS, 0, NOTELL)) {
				mon_adjust_speed(mtmp, -1, (struct obj *)0);
				m_dowear(mtmp, FALSE); /* might want speed boots */

			    }
			}
		}
	}

	if (book2->oartifact == ART_ONE_BEFORE_LAST) {
		register int randomspell;
		int attempts = 50000;

newbeforespell:
		randomspell = SPE_FORCE_BOLT + rn2((SPE_PSYBEAM + 1) - SPE_FORCE_BOLT);

		if (objects[randomspell].oc_level != 7) {
			if (attempts > 0) {
				attempts--;
				goto newbeforespell;
			}
		}

		wonderspell(randomspell, 0);
	}



	if (book2->oartifact == ART_MERLIN_S_TESTAMENT) {
		make_confused(HConfusion + rn1(20, 20), FALSE);
		wonderspell(-1, 10000);
	}

	if (book2->oartifact == ART_GRIMOIRE_OF_THASSILON) {
		getdir(NULL);
		buzz(21,4,u.ux,u.uy,u.dx,u.dy); /* fire */
		buzz(22,4,u.ux,u.uy,u.dx,u.dy); /* cold */
		buzz(25,4,u.ux,u.uy,u.dx,u.dy); /* lightning */
		use_skill(P_ELEMENTAL_SPELL, 20);
	}

	if (book2->oartifact == ART_TOME_OF_MORGANA) {
		int i, j, bd = 1;
		struct monst *mtmp;

		for(i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if ((mtmp = m_at(u.ux + i, u.uy + j)) != 0) {

			    if (!resist(mtmp, SPBOOK_CLASS, 0, NOTELL)) {
				(void) tamedog(mtmp, (struct obj *) 0, FALSE);
			    }
			}
		}

		bd = 3;

		for(i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if ((mtmp = m_at(u.ux + i, u.uy + j)) != 0) {

			    if (mtmp->mpeaceful) continue;

			    if (!resist(mtmp, SPBOOK_CLASS, 0, NOTELL)) {
				mtmp->mconf = TRUE;
				pline("%s is confused!", Monnam(mtmp));
			    }
			}
		}

		adjattrib(A_CHA, 1, FALSE, TRUE);
		u.tempspellchance20 += 5000;
		You_feel("more capable of casting spells.");
	}

	if (book2->oartifact == ART_BLOOMING_SCRIPTURE) {
		incr_itimeout(&HRegeneration, 5000);
		You("start regenerating.");
		if (!rn2(5)) {
	 	    (void) makemon(mkclass(S_BAD_FOOD,0), u.ux, u.uy, MM_ADJACENTOK|MM_ANGRY);
		}
	}

	if (book2->oartifact == ART_BEN_MN_A) {
		adjalign(1);
	}

	if (book2->oartifact == ART_YEAH_HOMIE_FIX_IT_UP) {
		struct obj *otmp;
		if (CannotSelectItemsInPrompts) return;
repairhomiechoice:
		otmp = getobj(allnoncount, "magically enchant");
		/*otmp = some_armor(&youmonst);*/
		if (otmp) {
			if (!(otmp->owornmask & W_ARMOR) ) { /* bug discovered by Heliokopis - did Sporkhack never fix this? */
	
				pline("You have a feeling of loss.");
			} else if (greatest_erosion(otmp) > 0) {
				if (!Blind) {
					pline("Your %s glows golden for a moment.",xname(otmp));
				}
				if (otmp->oeroded > 0) { otmp->oeroded = 0; }
				if (otmp->oeroded2 > 0) { otmp->oeroded2 = 0; }
				if (otmp && objects[(otmp)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(otmp)) {
					if (!otmp->cursed) bless(otmp);
					else uncurse(otmp, FALSE);
				}
			} else {
				if (!Blind) {
					pline("Your %s glows briefly, but looks as new as ever.",xname(otmp));
				}
			}
		} else {
			if (yn("Really exit with no object selected?") == 'y')
				pline("You just wasted the opportunity to enchant your armor.");
			else goto repairhomiechoice;
			/* the player can probably feel this, so no need for a !Blind check :) */
			pline("Your embarrassing %s rash clears up slightly.", body_part(BODY_SKIN));
		}
	}

	if (book2->oartifact == ART_FROSTFIELD) {
		int frostbombs = 8;
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_icefloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("The landscape is winterized!");

		while (frostbombs > 0) {
			makemon(&mons[PM_BOEBOEBOEBOEBOE_M_M], 0, 0, NO_MM_FLAGS);
			frostbombs--;
		}
	}

	if (book2->oartifact == ART_BINI_PIENSI) {
		u.negativeprotection++;
		if (evilfriday && u.ublessed > 0) {
			u.ublessed -= 1;
			if (u.ublessed < 0) u.ublessed = 0;
		}
		You_feel("less protected!");

		if (u.ualign.type == A_CHAOTIC) adjalign(5);
	}

	if (book2->oartifact == ART_HEY__I_ADORE_YOU__OKAY_) {
		int i, j, bd = 4;
		struct monst *mtmp;

		for(i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if ((mtmp = m_at(u.ux + i, u.uy + j)) != 0) {

			    if (mtmp->mfrenzied) continue;
			    if (mtmp->data->geno & G_UNIQ) continue;
			    if (mtmp->mpeaceful) continue;

			    if (!resist(mtmp, SPBOOK_CLASS, 0, NOTELL)) {
				mtmp->mpeaceful = TRUE;
				pline("%s is pacified!", Monnam(mtmp));
			    }
			}
		}

	}

	if (book2->oartifact == ART_BEGINNER_S_COMPENDIUM_OF_C) {
		if (u.uspellprot < 2) {
			u.uspellprot = 2;
			u.uspmtime = 10;
			if (!u.usptime) u.usptime = u.uspmtime;
			find_ac();
		}
		flags.botl = TRUE;
		incr_itimeout(&HRegeneration, 100);
		incr_itimeout(&HFast, 50);
		You("feel faster, and start regenerating!");

		register struct monst *nexusmon, *nextmon;

		for(nexusmon = fmon; nexusmon; nexusmon = nextmon) {
		    nextmon = nexusmon->nmon; /* trap might kill mon */
		    if (DEADMONSTER(nexusmon)) continue;

		    if (!sensemon(nexusmon) && !canseemon(nexusmon)) continue;

		    if (nexusmon->data->geno & G_UNIQ) continue;
		    if (nexusmon->mpeaceful) continue;
		    if (nexusmon->mfrenzied) continue;

		    if (!resist(nexusmon, SPBOOK_CLASS, 0, NOTELL)) {
			nexusmon->mpeaceful = TRUE;
			pline("%s is pacified!", Monnam(nexusmon));
		    }

		}

	}

	if (book2->oartifact == ART_SEFER_RAZIEL) {
		if (u.uenmax > 2) {
			u.uenmax -= rnd(3);
			flags.botl = TRUE;
		} else drain_alla(300);

		if (u.usteed) {
			u.usteed->mhp = u.usteed->mhpmax;
			u.usteed->mfrozen = 0;
			u.usteed->msleeping = 0;
			u.usteed->masleep = 0;
			u.usteed->mcanmove = 1;
			u.usteed->mflee = 0;
			u.usteed->mcansee = 1;
			u.usteed->mblinded = 0;
			u.usteed->mstun = 0;
			u.usteed->mconf = 0;
			pline("%s is cured.", Monnam(u.usteed));
		}

		register struct monst *nexusmon, *nextmon;

		for(nexusmon = fmon; nexusmon; nexusmon = nextmon) {
		    nextmon = nexusmon->nmon; /* trap might kill mon */
		    if (DEADMONSTER(nexusmon)) continue;

		    if (!nexusmon->mtame) continue;

			nexusmon->mhp = nexusmon->mhpmax;
			nexusmon->mfrozen = 0;
			nexusmon->msleeping = 0;
			nexusmon->masleep = 0;
			nexusmon->mcanmove = 1;
			nexusmon->mflee = 0;
			nexusmon->mcansee = 1;
			nexusmon->mblinded = 0;
			nexusmon->mstun = 0;
			nexusmon->mconf = 0;
			pline("%s is cured.", Monnam(nexusmon));


		}

		int k, l;
		struct monst *mtmp3;

		for (k = -4; k <= 4; k++) for(l = -4; l <= 4; l++) {
			if (!isok(u.ux + k, u.uy + l)) continue;
			if ( ((mtmp3 = m_at(u.ux + k, u.uy + l)) != 0))

			{

				if (!mtmp3->mpeaceful && (mtmp3->data->maligntyp < 0)) {
					mtmp3->mhp -= (GushLevel * 3);
					pline("%s is hit by divine retribution!", Monnam(mtmp3));
					if (mtmp3->mhp <= 0) {
						pline("%s is killed!", Monnam(mtmp3));
						xkilled(mtmp3, 0);
					} else wakeup(mtmp3); /* monster becomes hostile */

				}

			}
		} /* for loop */
	}

	if (book2->oartifact == ART_CELESTIAL_CODEX) {

		make_blinded(Blinded + rn1(10,10), TRUE);

		int k, l;
		struct monst *mtmp3;

		for (k = -4; k <= 4; k++) for(l = -4; l <= 4; l++) {
			if (!isok(u.ux + k, u.uy + l)) continue;
			if ( ((mtmp3 = m_at(u.ux + k, u.uy + l)) != 0))

			{
				if (is_demon(mtmp3->data) || is_undead(mtmp3->data)) {
					mtmp3->mhp -= (GushLevel * 3);
					pline("%s is hit by divine retribution!", Monnam(mtmp3));
					if (mtmp3->mhp <= 0) {
						pline("%s is killed!", Monnam(mtmp3));
						xkilled(mtmp3, 0);
					} else wakeup(mtmp3); /* monster becomes hostile */

				}
			}
		} /* for loop */

	}

	if (book2->oartifact == ART_FLICKERING_PALIMPSEST) {
		register struct obj *acqo;

		acqo = mkobj_at(RANDOM_CLASS, u.ux, u.uy, FALSE, FALSE);
		if (acqo) {
			pline("An object appeared at your %s!", makeplural(body_part(FOOT)));
		} else {
			pline("Nothing happens...");
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}

	}

	if (book2->oartifact == ART_I_PONSER_TO_MAKE_STINGER_B) {
		struct obj *uammo;
		uammo = mksobj(randartboots(), TRUE, FALSE, FALSE);
		if (uammo) {
			uammo->quan = 1;
			uammo->owt = weight(uammo);
			uammo->enchantment = RNGE_THORNS;
			dropy(uammo);
			stackobj(uammo);
			pline("A pair of boots has formed on the ground!");
		}
	}

	if (book2->oartifact == ART_CRUELTY_PROOF) {
		use_skill(P_AXE, 50);
	}

	if (book2->oartifact == ART_DUEUEUEUEUEI_ACCELEROTOR_) {
		incr_itimeout(&HFast, rn1(20, 20));
		You_feel("faster.");
	}

	if (book2->oartifact == ART_TOME_OF______TONGUES) {
		spellmemorynull();
		wonderspell(-1, 0);
	}
	if (book2->oartifact == ART_BLACK_CHRONICLE) {
		u.uhpmax += 5;

		if (Upolyd) u.mhmax += 5;

		if (uinsymbiosis) {
			u.usymbiote.mhpmax += 5;
			maybe_evolve_symbiote();
			if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
		}

		flags.botl = TRUE;

		struct monst *mtmp3;
		int k, l;
		for (k = -6; k <= 6; k++) for(l = -6; l <= 6; l++) {
			if (!isok(u.ux + k, u.uy + l)) continue;

			if ( (mtmp3 = m_at(u.ux + k, u.uy + l)) != 0) {

				if (!mtmp3->mpeaceful) {

					if (!resist(mtmp3, SPBOOK_CLASS, 0, NOTELL)) {
						mtmp3->mconf = TRUE;
					}
					if (!resist(mtmp3, SPBOOK_CLASS, 0, NOTELL)) {
						mtmp3->mstun = TRUE;
					}
					if (!resist(mtmp3, SPBOOK_CLASS, 0, NOTELL)) {
						monflee(mtmp3, rn1(15,15), FALSE, TRUE, FALSE);
					}
					if (!resist(mtmp3, SPBOOK_CLASS, 0, NOTELL)) {
						mtmp3->mhpmax = (mtmp3->mhpmax * 4 / 5);
						if (mtmp3->mhpmax < 1) mtmp3->mhpmax = 1;
						if (mtmp3->mhp > mtmp3->mhpmax) mtmp3->mhp = mtmp3->mhpmax;
					}
					if (!resist(mtmp3, SPBOOK_CLASS, 0, NOTELL) && mtmp3->mcansee) {
						mtmp3->mcansee = FALSE;
						mtmp3->mblinded = rn1(15,15);
					}
					removemonsteregotypes(mtmp3);
				}

			}
		}


	}

	if (book2->oartifact == ART_CLOCKWORK_GRIMOIRE) {
		pline((Role_if(PM_SAMURAI) || Role_if(PM_NINJA)) ? "Jikan ga teishi shimashita." : "Time has stopped.");
		TimeStopped += rn1(2,2);

	      u_slow_down();
		u.uprops[DEAC_FAST].intrinsic += 120;
		u.inertia += 30;
		monstermoves += 30;
		moves += 30;
	}

	if (book2->oartifact == ART_NULL_SCRIPT) {
		spellmemorynull();
		removeforgottenspell(TRUE);
		u.tempsupereregen += 100;
		You("start regenerating mana rapidly!");
	}

	if (book2->oartifact == ART_LIBER_ARCANUM_OF_MERLIN) {
		switch (rnd(3)) {
			case 1:
				castspecificspell(SPE_INVISIBILITY);
				break;
			case 2:
				castspecificspell(SPE_HASTE_SELF);
				break;
			case 3:
				castspecificspell(SPE_FIREBALL);
				break;
		}

		if (!rn2(3)) {

			register struct monst *tamegolem;
			tamegolem = makemon(mkclass(S_GOLEM,0), u.ux, u.uy, MM_NOSPECIALS);
			if (tamegolem) {
				tamegolem = tamedog(tamegolem, (struct obj *) 0, FALSE);
				if (tamegolem) You("dominate %s!", mon_nam(tamegolem));
			}
		}
	}

	if (book2->oartifact == ART_BOOK_OF_BABA_YAGA) {
		LuckLoss += 1000;
		register struct monst *tamegolem;
		tamegolem = makemon(mkclass(S_GOLEM,0), u.ux, u.uy, MM_NOSPECIALS);
		if (tamegolem) {
			tamegolem = tamedog(tamegolem, (struct obj *) 0, FALSE);
			if (tamegolem) You("dominate %s!", mon_nam(tamegolem));
		}
	}

	if (book2->oartifact == ART_TRAVELOGUE_OF_THE_WANDERIN) {
		phase_door(0);
		incr_itimeout(&HTeleport_control, 100);
		You_feel("a bit more in control of yourself.");
	}

	if (book2->oartifact == ART_PUNYBODY) {
		prayer_done();
	}

	if (book2->oartifact == ART_MORGANA_S_MIRROR_SCRIPT) {
		make_confused(HConfusion + rn1(5, 5), FALSE);

		register struct monst *nexusmon, *nextmon;

		for(nexusmon = fmon; nexusmon; nexusmon = nextmon) {
			nextmon = nexusmon->nmon; /* trap might kill mon */
			if (DEADMONSTER(nexusmon)) continue;

			if (resist(nexusmon, SPBOOK_CLASS, 0, NOTELL)) continue;
			nexusmon->mconf = TRUE;
		}
		You_feel("a confusing aura!");
	}

	if (book2->oartifact == ART_TABLET_OF_CIRCE) {

		struct monst *mtmp3;
		int k, l;
		for (k = -2; k <= 2; k++) for(l = -2; l <= 2; l++) {
			if (!isok(u.ux + k, u.uy + l)) continue;

			if ( (mtmp3 = m_at(u.ux + k, u.uy + l)) != 0) {

				if (!resist(mtmp3, SPBOOK_CLASS, 0, NOTELL)) {
					mon_spec_poly(mtmp3, (struct permonst *)0, 0L, 0, canseemon(mtmp3), FALSE, FALSE);
				}

			}
		}

		incr_itimeout(&HPolymorph_control, 5000);
		You_feel("capable of controlling your polymorphs.");
	}

	if (book2->oartifact == ART_SONGBOOK_OF_THE_MEADOW) {

		struct monst *mtmp3;
		int k, l;
		for (k = -5; k <= 5; k++) for(l = -5; l <= 5; l++) {
			if (!isok(u.ux + k, u.uy + l)) continue;

			if ( (mtmp3 = m_at(u.ux + k, u.uy + l)) != 0) {

				if (is_animal(mtmp3->data)) {
					if (!resist(mtmp3, SPBOOK_CLASS, 0, NOTELL)) {
						if (!mtmp3->mfrenzied) mtmp3->mfrenzied = TRUE;
					}	
				}

			}
		}

		if (u.usteed && u.usteed->mtame < 30) u.usteed->mtame += 1;

		register struct monst *nexusmon, *nextmon;

		for(nexusmon = fmon; nexusmon; nexusmon = nextmon) {
			nextmon = nexusmon->nmon; /* trap might kill mon */
			if (DEADMONSTER(nexusmon)) continue;
			if (!nexusmon->mtame) continue;

			if (nexusmon->mtame < 30) nexusmon->mtame += 1;
		}

		You_feel("a peaceful aura.");
	}

	if (book2->oartifact == ART_LANTERN_LEXICON) {
		castspecificspell(SPE_LIGHT);
	}

	if (book2->oartifact == ART_POCKET_ALMANAC) {
		castspecificspell(SPE_MAGIC_MAPPING);
		incr_itimeout(&HRegeneration, 500);
		You("start regenerating.");
	}

	if (book2->oartifact == ART_LIBRAM_OF_THE_LIVING_WORD) {
		AlwaysEgotypeMonsters += rnz(10000);
		wonderspell(-1, 0);
		wonderspell(-1, 0);
		wonderspell(-1, 0);
	}

	if (book2->oartifact == ART_MIRRORBINDER_S_JOURNAL) {
		struct monst *mirrormon;

		mirrormon = makemon(&mons[urole.malenum], u.ux, u.uy, NO_MM_FLAGS);
		if (mirrormon) (void) tamedog(mirrormon, (struct obj *) 0, TRUE);
		You("summon a clone of yourself!");
	}

	if (book2->oartifact == ART_LIBER_UMBRAE) {
		register struct monst *nexusmon, *nextmon, *umbramon;

		for(nexusmon = fmon; nexusmon; nexusmon = nextmon) {
			nextmon = nexusmon->nmon; /* trap might kill mon */
			if (DEADMONSTER(nexusmon)) continue;
			if (nexusmon->umbraeconf) continue;

			umbramon = makemon(&mons[monsndx(nexusmon->data)], nexusmon->mx, nexusmon->my, MM_ANGRY|MM_XFRENZIED|MM_ADJACENTOK);
			if (umbramon) {
				umbramon->umbraeconf = TRUE;
				umbramon->mpeaceful = umbramon->mtame = FALSE;
			}
		}
		You_feel("the arrival of some shadows...");
	}

	if (book2->oartifact == ART_DREAMWEAVER_S_JOURNAL) {
		fall_asleep(-(rn1(10, 10)), TRUE);
		adjattrib(A_INT, 1, FALSE, TRUE);
		adjattrib(A_WIS, 1, FALSE, TRUE);
		u.tempmonssleepconf += 500;
	}

	if (book2->oartifact == ART_UNWRITTEN_GOSPEL) {
		u.tempsenserlistener += rnz(1000);
	}

	if (book2->oartifact == ART_BOOK_OF_THOTH) {
		Itemcursing += rnz(2500);
		if (!Blind) {
			You("notice a %s glow surrounding you.", hcolor(NH_BLACK));
			if (PlayerHearsSoundEffects) pline(issoviet ? "Vashe der'mo tol'ko chto proklinal." : "Woaaaaaa-AAAH!");
		}
		rndcurse();
		adjattrib(A_INT, 1, FALSE, TRUE);
	}

	if (book2->oartifact == ART_KRT_DESERT_TUB) {
		wonderspell(SPE_TELEPORT_SELF, 0);
		wonderspell(SPE_TELEPORT_AWAY, 0);
		wonderspell(SPE_PHASE_DOOR, 0);
		use_skill(P_BODY_SPELL, 50);
		use_skill(P_OCCULT_SPELL, 25);
	}

	if (book2->oartifact == ART_STAYHEEL) {
		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		(void) makemon(specialtensmon(116), u.ux, u.uy, MM_ADJACENTOK); /* M2_DOMESTIC */

		u.aggravation = 0;
	}

	if (book2->oartifact == ART_GRIMOIRE_OF_THE_BLACK_SUN) {

		int darkgrimmonst = rn1(3,3);

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		while (darkgrimmonst > 0) {
			darkgrimmonst--;
			(void) makemon(specialtensmon(239), u.ux, u.uy, MM_ADJACENTOK); /* AD_DARK */
		}

		int ulx, uly;
		for (ulx = 1; ulx < (COLNO); ulx++)
	        for (uly = 0; uly < (ROWNO); uly++) {
			levl[ulx][uly].lit = 0;
		}
		DeLightBug += rnz(4000);

		use_skill(P_OCCULT_SPELL, rnz(50));

		u.aggravation = 0;
	}

	if (book2->oartifact == ART_ATLAS_OF_FORGOTTEN_WORLDS) {
		WallTrapping += rnz(5000);
		ChaosTerrain += rnz(5000);

		if (((u.uhave.amulet) && !u.freeplaymode) || CannotTeleport || (u.usteed && mon_has_amulet(u.usteed))) {
			Your("soul was drained!");
			drain_alla(200);
		} else if (playerlevelportdisabled()) {
			pline("For some reason you resist the banishment!");
			drain_alla(200);
		} else {
			banishplayer();
			You("entered the forgotten world!");
		}

	}

	if (book2->oartifact == ART_PERCENTES_SPELL) {
		if (rn2(2)) goodeffect();
		else badeffect();
	}

	if (book2->oartifact == ART_GOFFIC_COOLNESS) {
		int vampirepolypm = PM_LONG_WORM_TAIL;
		while (!is_vampire(&mons[vampirepolypm]) || !polyok(&mons[vampirepolypm]) || (notake(&mons[vampirepolypm]) && rn2(4) ) || ((mons[vampirepolypm].mlet == S_BAT) && rn2(2)) || ((mons[vampirepolypm].mlet == S_EYE) && rn2(2) ) || ((mons[vampirepolypm].mmove == 1) && rn2(4) ) || ((mons[vampirepolypm].mmove == 2) && rn2(3) ) || ((mons[vampirepolypm].mmove == 3) && rn2(2) ) || ((mons[vampirepolypm].mmove == 4) && !rn2(3) ) || ( (mons[vampirepolypm].mlevel < 10) && ((mons[vampirepolypm].mlevel + 1) < rnd(u.ulevel)) ) || (!haseyes(&mons[vampirepolypm]) && rn2(2) ) || ( is_nonmoving(&mons[vampirepolypm]) && rn2(5) ) || ( is_eel(&mons[vampirepolypm]) && rn2(5) ) || ( is_nonmoving(&mons[vampirepolypm]) && rn2(20) ) || (is_jonadabmonster(&mons[vampirepolypm]) && rn2(20)) || ( uncommon2(&mons[vampirepolypm]) && !rn2(4) ) || ( uncommon3(&mons[vampirepolypm]) && !rn2(3) ) || ( uncommon5(&mons[vampirepolypm]) && !rn2(2) ) || ( uncommon7(&mons[vampirepolypm]) && rn2(3) ) || ( uncommon10(&mons[vampirepolypm]) && rn2(5) ) || ( is_eel(&mons[vampirepolypm]) && rn2(20) ) ) {
			vampirepolypm = rn2(NUMMONS);
		}
		if (!Unchanging) {
			polymon(vampirepolypm);

			register struct obj *vampheels;
			vampheels = mksobj(HIPPIE_HEELS, TRUE, FALSE, FALSE);
			if (vampheels) {
				if (!vampheels->enchantment) vampheels->enchantment = HALLUC_RES;
				(void) pickup_object(vampheels, vampheels->quan, TRUE, TRUE);
				if (uarmf) remove_worn_item(uarmf, TRUE);
				setworn(vampheels, W_ARMF);
				Boots_on();
				if (vampheels) curse(vampheels);

			}
		}
	}

	if (book2->oartifact == ART_ALLA_DANN_) {
		gain_alla(100);
		pline("Alla dann!");
	}

	if (book2->oartifact == ART_PARANORMAL) {
		lesshungry(200);
		Your("stomach fills a bit.");
	}

	if (book2->oartifact == ART_WAYFARER_S_ALMANAC) {
		lesshungry(100);
		Your("stomach fills a bit.");
		if (!BClairvoyant)
		    do_vicinity_mapX();
		incr_itimeout(&HFull_nutrient, 100);
		incr_itimeout(&HClairvoyant, 100);
	}

	if (book2->oartifact == ART_ONLY_MM_HAHA) {
		u.uenmax++;
		flags.botl = TRUE;
		mightbooststat(A_WIS);
	}

	if (book2->oartifact == ART_SUDDEN_MEMORY_FAILURE) {
		if (LLMMessages) {
			u.sterilized = rnz(1500);
			/* no message because the memory trap effect means you can't read it anyway --Amy */
		}
	}

	if (book2->oartifact == ART_KNOW_HOW_TO_DO_IT_) {

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		int morgueamount = 5;
		while (morgueamount > 0) {
			morgueamount--;
			(void) makemon(morguemon(), u.ux, u.uy, MM_ADJACENTOK);
		}

		u.aggravation = 0;
	}

	if (book2->oartifact == ART_FARTOMAT) {
		if (!FemtrapActiveRuth) pline("Your sexy butt signals that it wants to produce beautiful farting noises!");
		FemaleTrapMaurah += rnz(3000);
		change_luck(1);
		You_feel("lucky.");
	}

	if (book2->oartifact == ART_SPORKSTA_PANE) {
		int sporkstatemp = rnz(5000);
		u.monstertimeout += sporkstatemp;
		u.monstertimefinish += sporkstatemp;
		u.monstertimefinish += rnz(10000);
	}

	if (book2->oartifact == ART_HOPES_AND_DREAMS) {
		u.ascensiontimelimit += rnz(10000);
	}

	if (book2->oartifact == ART_WE_WOULD_LOVE_TO_WEAR_HIGH) {
		if (PlayerInHighHeels) {
			forget(15, FALSE);
		} else {
			bad_equipment_heel();
		}
	}

	if (book2->oartifact == ART_TRI_DELAY_LIGHTNING) {
		u.trilightningdelay = 150;
		You("prepare your lightning-based techniques.");
	}

	if (book2->oartifact == ART_RHEA_S_AURA) {
		incr_itimeout(&HPoison_resistance, 1000);
		You_feel("resistant to poison.");

		buzz(16, 8, u.ux, u.uy, -1, 0);
		buzz(16, 8, u.ux, u.uy, 1, 0);
		buzz(16, 8, u.ux, u.uy, -1, 1);
		buzz(16, 8, u.ux, u.uy, 1, 1);
		buzz(16, 8, u.ux, u.uy, 0, 1);
		buzz(16, 8, u.ux, u.uy, -1, -1);
		buzz(16, 8, u.ux, u.uy, 1, -1);
		buzz(16, 8, u.ux, u.uy, 0, -1);

	}

	if (book2->oartifact == ART_GOTSYM) {
		if (!uinsymbiosis) {
			getrandomsymbiote(FALSE, FALSE);
		}
	}

	if (book2->oartifact == ART_HUB_ONE_S_FOOLING) {
		if (!uimplant) bad_equipment_implant();
		else if (uimplant) {
			curse(uimplant); curse(uimplant); curse(uimplant); curse(uimplant); curse(uimplant); curse(uimplant); curse(uimplant); curse(uimplant); curse(uimplant); curse(uimplant);
			You_feel("a very dark aura surrounding your implant...");
		}
	}

	if (book2->oartifact == ART_UTE_S_CHALLENGE) {

		register struct monst *utemon;
		utemon = makemon(&mons[PM_UTE], 0, 0, NO_MM_FLAGS);

		if (utemon) {
			register struct obj *uteheels;
			uteheels = mksobj(find_appearance_armor(APP_MISSYS), FALSE, 2, FALSE);
			if (uteheels) uteheels = mk_artifact(uteheels, (aligntyp)A_NONE, TRUE);
			if (uteheels) {
				(void) mpickobj(utemon, uteheels, TRUE);
			}
			You_feel("challenged by the beautiful Ute!");
		}

	}

	if (book2->oartifact == ART_WHAT_FOR_A_T_SPRECHER) {
		int tryct = 0;
		int x, y;

		for (tryct = 0; tryct < 2000; tryct++) {
			x = rn1(COLNO-3,2);
			y = rn2(ROWNO);

			if (isok(x, y) && !(t_at(x, y)) ) {
				(void) maketrap(x, y, LOUDSPEAKER, 0, FALSE);
				break;
			}
		}

	}

	if (book2->oartifact == ART_TURN_ON_THE_ICE) {
		int k, l;

		for (k = -1; k <= 1; k++) for(l = -1; l <= 1; l++) {
			if (!isok(u.ux + k, u.uy + l)) continue;

			if (isok(u.ux + k, u.uy + l) && (levl[u.ux + k][u.uy + l].typ == ICE) ) {

				levl[u.ux + k][u.uy + l].typ = CORR;
				blockorunblock_point(u.ux + k, u.uy + l);
				newsym(u.ux + k, u.uy + l);

			}

		}
		You_feel("the ice thawing.");
	}

	if (book2->oartifact == ART_KWUKSPLASH) {
		if (levl[u.ux][u.uy].typ != ROOM && levl[u.ux][u.uy].typ != CORR) {
			You_feel("claustrophobic!");
		} else {
			pline("You build a toilet.");
			levl[u.ux][u.uy].typ = TOILET;
		}
	}

	if (book2->oartifact == ART_SALTING_THE_WATER) {
		if (level.flags.lethe == 0) {
			level.flags.lethe = 1;
			if (!(HProtection & INTRINSIC))  {
				HProtection |= FROMOUTSIDE;
				if (!u.ublessed) u.ublessed = (Race_if(PM_MAYMES) ? 6 : 3);
			} else {
				u.ublessed += 3;
				if (Race_if(PM_MAYMES)) u.ublessed += 3;
			}
			You_feel("protected, but also realize that the water has started to sparkle.");
		}
	}

	if (book2->oartifact == ART_FORGETTEN_EASY) {
		use_skill(P_MEMORIZATION, 100);
		forget(3, FALSE);
	}

	if (book2->oartifact == ART_ACTUAL_CONE_FORM) {
		use_temporary_tech(T_SIGIL_TEMPEST);
	}

	if (book2->oartifact == ART_SPRITZWHOOSH) {
		use_temporary_tech(T_SIGIL_DISCHARGE);
	}

	if (book2->oartifact == ART_ELIF_S_ASIAN_BUTT) {
		if (!FemtrapActiveRuth) pline("A ghostly girl (named Elif) starts following you around, and apparently she wants to play with you!");
		FemaleTrapElif += rnz(2000);
		use_skill(P_SHII_CHO, 100);
	}

	if (book2->oartifact == ART_ATTENTION__LET_OP_) {
		use_temporary_tech(T_SPIRIT_BOMB);
	}

	if (book2->oartifact == ART_YOU_AND_YOUR_WELDING_FEET_) {
		if (!FemtrapActiveRuth) pline("Wear Sabrina's platform boots! Now! You just know that if you don't, some angry women will try to kill you.");
		FemaleTrapSabrina += rnz(5000);
		if (!FemtrapActiveRuth) pline("Those farmer girls plan to use block-heeled lady boots, wooden sandals and scentful sneakers on you!");
		FemaleTrapKerstin += rnz(5000);
	}

	if (book2->oartifact == ART_MAFIA_BOSS_METHOD) {
		if (uwep && uwep->oclass == WEAPON_CLASS) {
			if (!stack_too_big(uwep)) {
				uwep->opoisoned = TRUE;
				Your("weapon was poisoned!");
			} else {
				Your("stack of weapons was too big and therefore didn't get poisoned.");
			}
		} else pline("Sadly, you weren't wielding a weapon, so the poisoning didn't work.");
	}

	if (book2->oartifact == ART_AAAAAAAAAAAAAAAAAAAAAAAA) {
		int angelsummon = 8;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		while (angelsummon > 0) {
			angelsummon--;
			(void) makemon(mkclass(S_ANGEL,0), u.ux, u.uy, MM_ADJACENTOK|MM_ANGRY|MM_XFRENZIED);
		}

		u.aggravation = 0;

		You("feel that the angels are out to punish you for your misdeeds!");

	}

	if (book2->oartifact == ART_YAVANNA_S_PLACITUDE) {
		if (u.ualign.type == A_LAWFUL) {
			u.alignlim += 10;
			adjalign(10);
			You_feel("that the gods are pleased with your progress.");
		}
	}

	if (book2->oartifact == ART_PATRICIA_S_ABILITY) {
		if (!FemtrapActiveRuth) pline("You feel like a beautifully thick girl, capable of splitting enemies' skulls with blunt weapons!");
		FemaleTrapPatricia += rnz(10000);
	}

	if (book2->oartifact == ART_AMY_S_CHILDHOOD_FEAR) {
		if (u.uhpmax < 11) u.uhpmax = 1;
		else u.uhpmax -= 10;
		if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		if (Upolyd) {
			u.mhmax -= 10;
			if (u.mhmax < 1) u.mhmax = 1;
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		}
		flags.botl = TRUE;
		Your("health was damaged.");

		if (((u.uhave.amulet) && !u.freeplaymode) || CannotTeleport || (u.usteed && mon_has_amulet(u.usteed))) {
			pline("You shudder for a moment.");
		} else if (playerlevelportdisabled()) { 
			pline("For some reason, you resist the banishment!");
		} else {
			make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */
			/* delay the banishment, because you might be reading the book off the ground --Amy */
			if (!u.banishmentbeam) {
				u.banishmentbeam = 1;
				nomul(-2, "being banished", FALSE); /* because it's not called until you get another turn... */
			}
		}
	}

	if (book2->oartifact == ART_MATERNITY) {
		flags.female = TRUE;
		flags.botl = TRUE;
		FemaleTrapNatalia += rnz(5000);
		pline("Ack! You suddenly have to deal with the effects of your menstruational period!");

		if (!(HProtection & INTRINSIC))  {
			HProtection |= FROMOUTSIDE;
			if (!u.ublessed) u.ublessed = (Race_if(PM_MAYMES) ? 2 : 1);
		} else {
			u.ublessed++;
			if (Race_if(PM_MAYMES)) u.ublessed++;
		}
		You("feel more protected.");
	}

	if (book2->oartifact == ART_FULL_MONTE_HALL) {
		play_blackjack(TRUE);
	}

	if (book2->oartifact == ART_I_EPPERE) {
		if (u.tremblingamount > 0) {
			u.tremblingamount--;
			You("control your trembling.");
			if (!u.tremblingamount) pline("In fact, you aren't trembling at all anymore.");
		}
	}

	if (book2->oartifact == ART_DOWNCOME_IN_THE_YEARS) {
		int x, y;
		int houzanhaamount = 50;
		register struct rm *lev;
		while (houzanhaamount) {
			houzanhaamount--;
			if (houzanhaamount < 0) houzanhaamount = 0;
			x = rn1(COLNO-3,2);
			y = rn2(ROWNO);
			lev = &levl[x][y];
			if (isok(x,y) && lev->typ != STAIRS && lev->typ != LADDER && !(lev->typ == ALTAR && (Is_astralevel(&u.uz) || Is_sanctum(&u.uz)) ) && !(lev->wall_info & W_NONDIGGABLE) && lev->typ != STAIRS && lev->typ != LADDER) {
				lev->typ = CORR;
			}
		}
		pline("Haaaaaaaa!");
		hunkajunkriver();
		randhunkrivers();
		pline("What a hunk 'a junk there is on this level.");

		if (!(InterfaceScrewed || u.uprops[INTERFACE_SCREW].extrinsic || have_interfacescrewstone())) doredraw();
	}

	if (book2->oartifact == ART_UNIT_NINE_IN_NATE_OF_THE_S) {
		verisiertEffect += rnz(2000);
	}

	if (book2->oartifact == ART_SPIRAL_MANUSCRIPT) {
		You_feel("like you're going to throw up.");
	      make_vomiting(Vomiting+20, TRUE);
		if (Sick && Sick < 100)
			set_itimeout(&Sick, (Sick * 2) + 10); /* higher chance to survive long enough --Amy */
		NoReturnEffect += rnz(2000);
		You("can't teleport for a while.");
	}

	if (book2->oartifact == ART_WE_GOT_A_______IN_CENTRAL_) {
		u.copwantedlevel += 5000;
		pline("Now your copilot wanted level has increased!");
	}

	if (book2->oartifact == ART_DN_DN_DN__DN_DN_DN_) {
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_tunnelfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("Lots of tunnels are built!");
	}

	if (book2->oartifact == ART_COLORFUL_LOVENAILS) {
		u.nailpolish = 10;
		Your("cute lovenails are painted very colorfully now! <3");
	}

	if (book2->oartifact == ART_HELIOKOPIS_S_SHORT_REAPPEA) {
		if (EscapePastEffect < 25000) EscapePastEffect = 25000;
	}

	if (book2->oartifact == ART_WHY__THEREFORE_) {
		LongingEffect += rnz(5000);
	}

	if (book2->oartifact == ART_HORRENDOUS_EXPERIENCE) {

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		coord cc, dd;
		int cx,cy, randsp, i;

		cx = rn2(COLNO);
		cy = rn2(ROWNO);

		randsp = rn1(5,5);

		for (i = 0; i < randsp; i++) {
			if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

			int attempts = 0;
			register struct permonst *ptrZ;
newbossHORR:
			do {

				ptrZ = rndmonst();
				attempts++;
				if (!rn2(2000)) reset_rndmonst(NON_PM);

			} while ( (!ptrZ || (ptrZ && !(attackdamagetype(ptrZ, AT_GAZE, AD_INSA)) && !(attackdamagetype(ptrZ, AT_GAZE, AD_SANI)) && !(attackdamagetype(ptrZ, AT_GAZE, AD_SPC2)) ) ) && attempts < 50000);

			if (ptrZ && (attackdamagetype(ptrZ, AT_GAZE, AD_INSA) || attackdamagetype(ptrZ, AT_GAZE, AD_SANI) || attackdamagetype(ptrZ, AT_GAZE, AD_SPC2) ) ) {
				(void) makemon(ptrZ, cx, cy, MM_ANGRY|MM_ADJACENTOK);
			}
			else if (rn2(50)) {
				attempts = 0;
				goto newbossHORR;
			}

		}

		u.aggravation = 0;

		u.alignlim += 5;
		adjalign(5);

		more_experienced(u.ulevel * 1000, 0);
		newexplevel();
	}

	if (book2->oartifact == ART_SPRAY_THE_MON_OUT_) {
		u.tempmorepokemon += 5000;
		You_feel("that pokemon are more likely to spawn now!");
	}

	if (book2->oartifact == ART_SHOEPOCALYPSE) {
		wonderspell(-1, 0);
	}

	if (book2->oartifact == ART_ALL_UNITS_TO_THE_BILLITY_R) {
		RespawnProblem += rnz(5000);
	}

	if (book2->oartifact == ART_WYNY) {
		wonderspell(-1, 0);
	}

	if (book2->oartifact == ART_MIRA_S_REFRESHING_BATH) {
		if (!Is_waterlevel(&u.uz)) {
			int i, j;
			for (i = 0; i <= COLNO; i++) for(j = 0; j <= ROWNO; j++) {

				if (!isok(i, j)) continue;
				if (levl[i][j].typ == POOL || levl[i][j].typ == WATER || levl[i][j].typ == MOAT) {

					if (!rn2(10)) {
						levl[i][j].typ = URINELAKE;
						blockorunblock_point(i,j);
					}

				}

			}
			You("hear Mira advertising her 'refreshing' bath to you...");
		}
	}

	if (book2->oartifact == ART_YOU_AS_STUPID_AS_ORPHEUS_A) {
		if (!level.flags.nommap) {
			do_mapping();
		}
	}

	if (book2->oartifact == ART_LET_RHEA_HELP_YOU) {
		if (!FemtrapActiveRuth) pline("Ugh, the girls and women are really going to beguile you with the horrible odor of their perfume!");
		FemaleTrapRhea += 2000;
		incr_itimeout(&HScentView, 2000);
		incr_itimeout(&HPoison_resistance, 2000);
		incr_itimeout(&HBlind_resistance, 2000);
		Your("sense of smell expands, and you feel resistant to poison and blindness.");
	}

	if (book2->oartifact == ART_NOT_OBVIOUS_IN_ONE_HUNDRED) {
		reducesanity(1000);
	}

	if (book2->oartifact == ART_NOT_JUST_A_LITTLE_IMBA) {
		if(uwep && (uwep->oclass == WEAPON_CLASS || uwep->oclass == BALL_CLASS || uwep->oclass == GEM_CLASS || uwep->oclass == CHAIN_CLASS || uwep->oclass == VENOM_CLASS || is_weptool(uwep))) {
			if (stack_too_big(uwep)) {
				pline("Unfortunately your stack of weapons was too big and therefore the enchantment failed!");
			} else if (uwep->spe < 13) {
				uwep->spe++;
				Your("weapon was enchanted!");
			} else pline("Unfortunately your weapon cannot be enchanted further.");
		} else {
			You("didn't wield a weapon, so the enchantment didn't work!");
		}
	}

	if (book2->oartifact == ART_X_TREME_FIREBRANDING) {
		if(uwep && (uwep->oclass == WEAPON_CLASS || uwep->oclass == BALL_CLASS || uwep->oclass == GEM_CLASS || uwep->oclass == CHAIN_CLASS || uwep->oclass == VENOM_CLASS || is_weptool(uwep))) {
			if (stack_too_big(uwep)) {
				pline("Unfortunately your stack of weapons was too big and therefore the branding didn't work.");
			} else if (!uwep->enchantment) {
				struct obj *tempwep = uwep;
				long savewornmask;
				savewornmask = tempwep->owornmask;
				setworn((struct obj *)0, tempwep->owornmask);
				tempwep->enchantment = WEAPEGO_FIERY;
				setworn(tempwep, savewornmask);

				Your("weapon is now branded with fire!");
			} else pline("Unfortunately your weapon already had an egotype.");
		} else {
			You("didn't wield a weapon, so the branding didn't work!");
		}
	}

	if (book2->oartifact == ART_SHADE_IGNORES_YOU_) {
		nomul(-10, "being ignored by the shade", FALSE);
		incr_itimeout(&Invulnerable, 10);
		You_feel("that the shade ignores you.");
	}

	if (book2->oartifact == ART_LORILORE) {

		if (P_MAX_SKILL(P_DIVINATION_SPELL) == P_ISRESTRICTED) {
			unrestrict_weapon_skill(P_DIVINATION_SPELL);
			pline("You can now learn the divination spells skill!");
		} else if (P_MAX_SKILL(P_DIVINATION_SPELL) == P_UNSKILLED) {
			unrestrict_weapon_skill(P_DIVINATION_SPELL);
			pline("You can now learn the divination spells skill!");
			P_MAX_SKILL(P_DIVINATION_SPELL) = P_BASIC;
		} else if (!rn2(3) && (P_MAX_SKILL(P_DIVINATION_SPELL) == P_BASIC)) {
			P_MAX_SKILL(P_DIVINATION_SPELL) = P_SKILLED;
			pline("You can now become skilled in divination spells!");
		} else if (!rn2(9) && (P_MAX_SKILL(P_DIVINATION_SPELL) == P_SKILLED)) {
			P_MAX_SKILL(P_DIVINATION_SPELL) = P_EXPERT;
			pline("You can now become expert in divination spells!");
		} else if (!rn2(27) && (P_MAX_SKILL(P_DIVINATION_SPELL) == P_EXPERT)) {
			P_MAX_SKILL(P_DIVINATION_SPELL) = P_MASTER;
			pline("You can now become master in divination spells!");
		} else if (!rn2(81) && (P_MAX_SKILL(P_DIVINATION_SPELL) == P_MASTER)) {
			P_MAX_SKILL(P_DIVINATION_SPELL) = P_GRAND_MASTER;
			pline("You can now become grand master in divination spells!");
		} else if (!rn2(243) && (P_MAX_SKILL(P_DIVINATION_SPELL) == P_GRAND_MASTER)) {
			P_MAX_SKILL(P_DIVINATION_SPELL) = P_SUPREME_MASTER;
			pline("You can now become supreme master in divination spells!");
		} else if (P_MAX_SKILL(P_DIVINATION_SPELL) >= P_SUPREME_MASTER) {
			pline("Sadly your knowledge of the divination spell skill is already maxed.");
		}

	}

	if (book2->oartifact == ART_GRIMOIRE_OF_CIRCE) {
		use_temporary_tech(T_POLYFORM);
	}

	if (book2->oartifact == ART_CHILD_S_BOOK_OF_FAIRY_TALE) {
		use_temporary_tech(T_PACIFY);
	}

	if (book2->oartifact == ART_EXPENSIVE_PHYSICIAN_FEES) {
		u.ugold += rn1(500, 500);
		You("gain some money!");
	}

	if (book2->oartifact == ART_BOOK_OF_MORMON) {
		if ((u.ualign.type != A_CHAOTIC) && (u.ualignbase[A_CURRENT] == u.ualignbase[A_ORIGINAL]) ) {
			You("have a strong feeling that %s is angry...", u_gname());
			pline("%s accepts your allegiance.", align_gname(A_CHAOTIC));

			u.ualign.type = u.ualignbase[A_CURRENT] = A_CHAOTIC;
			u.ublessed = 0;
			flags.botl = 1;

			You("have a sudden sense of a new direction.");
			/* Beware, Conversion is costly */
			change_luck(-3);
			u.ublesscnt += (ishaxor ? 150 : 300);
			adjalign((int)(u.ualignbase[A_ORIGINAL] * (u.alignlim / 2)));
		} else {
			HateTrapEffect |= FROMOUTSIDE;
		}

		int vampirepolypm = PM_LONG_WORM_TAIL;
		while (!is_vampire(&mons[vampirepolypm]) || !polyok(&mons[vampirepolypm]) || (notake(&mons[vampirepolypm]) && rn2(4) ) || ((mons[vampirepolypm].mlet == S_BAT) && rn2(2)) || ((mons[vampirepolypm].mlet == S_EYE) && rn2(2) ) || ((mons[vampirepolypm].mmove == 1) && rn2(4) ) || ((mons[vampirepolypm].mmove == 2) && rn2(3) ) || ((mons[vampirepolypm].mmove == 3) && rn2(2) ) || ((mons[vampirepolypm].mmove == 4) && !rn2(3) ) || ( (mons[vampirepolypm].mlevel < 10) && ((mons[vampirepolypm].mlevel + 1) < rnd(u.ulevel)) ) || (!haseyes(&mons[vampirepolypm]) && rn2(2) ) || ( is_nonmoving(&mons[vampirepolypm]) && rn2(5) ) || ( is_eel(&mons[vampirepolypm]) && rn2(5) ) || ( is_nonmoving(&mons[vampirepolypm]) && rn2(20) ) || (is_jonadabmonster(&mons[vampirepolypm]) && rn2(20)) || ( uncommon2(&mons[vampirepolypm]) && !rn2(4) ) || ( uncommon3(&mons[vampirepolypm]) && !rn2(3) ) || ( uncommon5(&mons[vampirepolypm]) && !rn2(2) ) || ( uncommon7(&mons[vampirepolypm]) && rn2(3) ) || ( uncommon10(&mons[vampirepolypm]) && rn2(5) ) || ( is_eel(&mons[vampirepolypm]) && rn2(20) ) ) {
			vampirepolypm = rn2(NUMMONS);
		}
		if (!Unchanging) {
			polymon(vampirepolypm);
		}
 
	}

	if (book2->oartifact == ART_BOOK_OF_JEHOVA) {
		if ((u.ualign.type != A_LAWFUL) && (u.ualignbase[A_CURRENT] == u.ualignbase[A_ORIGINAL]) ) {
			You("have a strong feeling that %s is angry...", u_gname());
			pline("%s accepts your allegiance.", align_gname(A_LAWFUL));

			u.ualign.type = u.ualignbase[A_CURRENT] = A_LAWFUL;
			u.ublessed = 0;
			flags.botl = 1;

			You("have a sudden sense of a new direction.");
			/* Beware, Conversion is costly */
			change_luck(-3);
			u.ublesscnt += (ishaxor ? 150 : 300);
			adjalign((int)(u.ualignbase[A_ORIGINAL] * (u.alignlim / 2)));
		} else {
			u.usanity += 500;
			flags.botl = TRUE;
			You_feel("incapable of understanding what you just read!");
		}

	}

	if (book2->oartifact == ART_NOTEBOOK_OF_ERRANT_SPARKS) {
		u.uen += rn1(20,20);
		percentrestoremana(1);
		if (u.uen > u.uenmax) u.uen = u.uenmax;
		You_feel("full of mystic power!");

		getdir(NULL);
		buzz(25,2,u.ux,u.uy,u.dx,u.dy); /* lightning */
	}

	if (book2->oartifact == ART_BOOK_OF_SCIENTOLOGY) {
		if ((u.ualign.type != A_NEUTRAL) && (u.ualignbase[A_CURRENT] == u.ualignbase[A_ORIGINAL]) ) {
			You("have a strong feeling that %s is angry...", u_gname());
			pline("%s accepts your allegiance.", align_gname(A_NEUTRAL));

			u.ualign.type = u.ualignbase[A_CURRENT] = A_NEUTRAL;
			u.ublessed = 0;
			flags.botl = 1;

			You("have a sudden sense of a new direction.");
			/* Beware, Conversion is costly */
			change_luck(-3);
			u.ublesscnt += (ishaxor ? 150 : 300);
			adjalign((int)(u.ualignbase[A_ORIGINAL] * (u.alignlim / 2)));
		} else {
			switch (rnd(5)) {
				case 1:
					if (!FemtrapActiveRuth) pline("You feel that people are building toilets for you to use.");
					FemaleTrapYvonne |= FROMOUTSIDE;
					break;
				case 2:
					if (!FemtrapActiveRuth) pline("There's some karate women who want to demonstrate their combat capabilities to you.");
					FemaleTrapVictoria |= FROMOUTSIDE;
					break;
				case 3:
					if (!FemtrapActiveRuth) pline("Some unfair woman wants to crush you with her block-heeled combat boots!");
					FemaleTrapManuela |= FROMOUTSIDE;
					break;
				case 4:
					if (!FemtrapActiveRuth) pline("The girls are going to fart you in the face without emitting any kind of sound.");
					FemaleTrapJennifer |= FROMOUTSIDE;
					break;
				case 5:
					if (!FemtrapActiveRuth) pline("It seems that monsters may spotaneously turn into sexy women, as if there weren't enough of them running around the dungeon anyway.");
					FemaleTrapYasaman |= FROMOUTSIDE;
					break;
			}
		}

		if (invent) identify_pack(0, 0, 0);
	}

	if (book2->oartifact == ART_MUTAGEN_CONTAINER) {
		register struct monst *nexusmon, *nextmon;

		for(nexusmon = fmon; nexusmon; nexusmon = nextmon) {
			nextmon = nexusmon->nmon; /* trap might kill mon */
			if (DEADMONSTER(nexusmon)) continue;

			add_monster_egotype(nexusmon);
		}
		pline_The("mutagen container was distributed.");
	}

	if (book2->oartifact == ART_TSCHIINGDIAE) {
		int randomalignment = !rn2(3) ? A_CHAOTIC : rn2(2) ? A_NEUTRAL : A_LAWFUL;

		if (u.ualign.type != randomalignment) {
			u.ublessed = 0; /* lose your god's protection */
		}

		u.ualign.type = randomalignment;
		flags.botl = TRUE;

		Your("mind oscillates briefly.");
	}

	if (book2->oartifact == ART_POTATO___S_FORTUNE) {
		int randsp = 5;
		while (randsp > 0) {
			randsp--;
			(void) makemon(&mons[PM_ITEM_MASTER], 0, 0, NO_MM_FLAGS);
		}
		You_feel("that there might be treasure available...");
	}

	if (book2->oartifact == ART_FRA_NOAN) {
		bad_equipment(0);
	}

	if (book2->oartifact == ART_ATTACK_WITH_ALL_CANNON_PIP) {
		use_skill(P_ATTACK_SPELL, 25);
	}

	if (book2->oartifact == ART_MORGAN_S_STUDIES) {
		use_skill(P_OCCULT_SPELL, 35);
	}

	if (book2->oartifact == ART_EDDY_S_UNCONTROLLABILITY) {
		use_skill(P_CHAOS_SPELL, 50);
		badeffect();
	}

	if (book2->oartifact == ART_LAURA_S_BEDBOOK) {
		use_skill(P_ELEMENTAL_SPELL, 33);
	}

	if (book2->oartifact == ART_DARTH_MAUL_S_TEACHINGS) {
		use_skill(P_NIMAN, 50);
	}

	if (book2->oartifact == ART_DEEPWOOD_VENOM_SEEKOUT) {
		use_skill(P_VENOM, 50);
	}

	if (book2->oartifact == ART_SUE_THROWAWAY_COMMENTARY) {
		use_skill(P_BODY_SPELL, 60);
	}

	if (book2->oartifact == ART_MEDITATE__DUDE_) {
		use_skill(P_PROTECTION_SPELL, 75);
	}

	if (book2->oartifact == ART_WHEREVER_YOU_GO__THERE_YOU) {
		if (u.uenmax > 9) {
			u.uenmax -= 10;
			flags.botl = TRUE;
			You("bought some spell training at the cost of magical power.");
		} else if (u.alla > 100) {
			u.alla -= 100;
			You("bought some spell training at the cost of your soul.");
		} else {
			u.youaredead = 1;
			You("experience some horrible agony as your soul shatters.");
			killer = "reading a dangerous book";
			killer_format = KILLED_BY;
			done(DIED);
			/* lifesaved */
			u.youaredead = 0;
			u.alla = 1000; /* reset to starting amount */
		}
		use_skill(P_PROTECTION_SPELL, 10);
		use_skill(P_ATTACK_SPELL, 10);
		use_skill(P_BODY_SPELL, 10);
		use_skill(P_MATTER_SPELL, 10);
		use_skill(P_HEALING_SPELL, 10);
		use_skill(P_CHAOS_SPELL, 10);
		use_skill(P_OCCULT_SPELL, 10);
		use_skill(P_ELEMENTAL_SPELL, 10);
		use_skill(P_PROTECTION_SPELL, 10);
		use_skill(P_DIVINATION_SPELL, 10);

	}

	if (book2->oartifact == ART_APPRETEN_IMMEDIATELY_TERMI) {
		use_temporary_tech(T_CREATE_AMMO);
	}

	if (book2->oartifact == ART_SECRET_BOOK_OF_VENOM) {
		You("gain the secret knowledge of venom mixing!");
		learntech_or_leveltech(T_VENOM_MIXING, FROMOUTSIDE, 1);
	}

	if (book2->oartifact == ART_A_LITTLE_BITTLE_JUMEX) {
		u.homosexual = 1;
		You_feel("heterosexual.");
	}

	if (book2->oartifact == ART_BABYABO_PHOGO) {
		u.tempstdprotection += 10000;
		You("are protected from STDs for a while.");
	}

	if (book2->oartifact == ART_FLASHBACK_JIPPIEJEYO) {
		int i, j;
		pline("You learn the identity of some objects:");
		for (i = 0; i < 5; i++) {
			j = rn2(NUM_OBJECTS);
			while (objects[j].oc_prob < 1) j = rn2(NUM_OBJECTS);
			makeknown(j);
			pline("%s (%s).", obj_descr[j].oc_name, objtypenames[objects[j].oc_class]);
		}

	}

	if (book2->oartifact == ART_EXTRAORDINARY_METRONOME) {
		spell_metronome(1, 8);
	}

	if (book2->oartifact == ART_DRUM__THE_DRUM_) {
		You_feel("that monsters are aware of your presence."); /* aggravate monster */
		aggravate();

		register struct monst *mtmp;

		for(mtmp = fmon; mtmp; mtmp = mtmp->nmon) {
		    if (DEADMONSTER(mtmp)) continue;
		    if (distu(mtmp->mx,mtmp->my) > 100) continue;
		    if (!resist(mtmp, SCROLL_CLASS, 0, NOTELL)) {
			monflee(mtmp, rn1(10, 10), FALSE, FALSE, FALSE);
		    }
		}

	}

	if (book2->oartifact == ART_GOD_IN_THE_HEAVEN) {
		Your("prayer timeout is %d.", u.ublesscnt);
	}

	if (book2->oartifact == ART_METROTECHEN) {
		tech_metronome();
	}

	if (book2->oartifact == ART_GO_TO_THE_FAST_TRAIN) {
		TrapCreationProblem += rnz(12000);
		use_skill(P_SEARCHING, 100);
		You("train the searching skill, fast.");
	}

	if (book2->oartifact == ART_THERE_COME_THOSE_WUEU_WUEU) {

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		u.cnd_kopsummonamount++;
		int copcnt = rnd(monster_difficulty() ) + 1;
		if (rn2(5)) copcnt = (copcnt / (rnd(4) + 1)) + 1;
		if (Role_if(PM_CAMPERSTRIKER)) copcnt *= (rn2(5) ? 2 : rn2(5) ? 3 : 5);

		if (uarmh && itemhasappearance(uarmh, APP_ANTI_GOVERNMENT_HELMET) ) {
			copcnt = (copcnt / 2) + 1;
		}

		if (ublindf && ublindf->oartifact == ART_NO_CUPS) {
			copcnt = (copcnt / 2) + 1;
		}

		if (RngeAntiGovernment) {
			copcnt = (copcnt / 2) + 1;
		}

		int tryct = 0;
		int x, y;

		for (tryct = 0; tryct < 2000; tryct++) {
			x = rn1(COLNO-3,2);
			y = rn2(ROWNO);

			if (isok(x, y) && (levl[x][y].typ > DBWALL) && !(t_at(x, y)) ) {
				(void) maketrap(x, y, KOP_CUBE, 0, FALSE);
				break;
			}
		}

	      while(--copcnt >= 0) {
			if (xupstair) (void) makemon(mkclass(S_KOP,0), xupstair, yupstair, rn2(3) ? MM_ANGRY|MM_ADJACENTOK : MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
			else (void) makemon(mkclass(S_KOP,0), 0, 0, rn2(3) ? MM_ANGRY|MM_ADJACENTOK : MM_ANGRY|MM_ADJACENTOK|MM_FRENZIED);
		} /* while */

		u.aggravation = 0;
	}

	if (book2->oartifact == ART_I_HAVE_SPEP_TIME) {
		if (rn2(2)) You("have at least until turn %d before you need to have ascended.", u.ascensionfirsthint * 4);
		else You("don't have much more than %d turns before you need to have ascended.", u.ascensionsecondhint);
	}

	if (book2->oartifact == ART_ADRIANEN_WOMAN) {
		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}
		(void) makemon(specialtensmon(332), 0, 0, MM_ADJACENTOK); /* MS_SHOE */
		u.aggravation = 0;
	}

	if (book2->oartifact == ART_PROVINZKAJA_BITCHING) {
		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}
		(void) makemon(specialtensmon(332), u.ux, u.uy, MM_XFRENZIED|MM_ANGRY|MM_ADJACENTOK); /* MS_SHOE */
		u.aggravation = 0;
	}

	if (book2->oartifact == ART_HIPPOCRATES__AID) {
		u.uhpmax++;
		if (Upolyd) u.mhmax++;
		if (uinsymbiosis) {
			u.usymbiote.mhpmax++;
			maybe_evolve_symbiote();
			if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
		}
		u.uenmax++;
		flags.botl = TRUE;
		pline("Hippocrates decides to aid you.");
	}

	if (book2->oartifact == ART_GS_GF_XM_TANJA) {
		if (!(FemaleTrapTanja & INTRINSIC)) {

			FemaleTrapTanja |= FROMOUTSIDE;
			if (!FemtrapActiveRuth) pline("The girls learned takewondo, and will knock the crap out of you with their female sneakers.");

			adjattrib(A_CHA, rn1(5,5), FALSE, TRUE);

			if (!flags.female) use_skill(P_GUN_CONTROL, 50);

			You_feel("that the dude who made those XM sound files must have a girlfriend named Tanja.");
		}
	}

	if (book2->oartifact == ART_DEVILS_SOUND_LIKE_MOSQUITO) {
		int devilsummon = 4;

		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		while (devilsummon > 0) {
			devilsummon--;
			(void) makemon(mkclass(S_DEMON,0), u.ux, u.uy, MM_ADJACENTOK);
			(void) makemon(mkclass(S_ANT,0), u.ux, u.uy, MM_ADJACENTOK);
		}

		u.aggravation = 0;

		You("absolutely can't stand that high-pitched 'SSSSSSSSSS!' sound!");
	}

	if (book2->oartifact == ART_DAMDAMDAM_DAMDAMDAM_DAMDAM) {
		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		{
			int attempts = 0;
			register struct permonst *ptrZ;
newbossDAM:
			do {

				ptrZ = rndmonst();
				attempts++;
				if (attempts && (attempts % 10000 == 0)) u.mondiffhack++;
				if (!rn2(2000)) reset_rndmonst(NON_PM);

			} while ( (!ptrZ || (ptrZ && !(ptrZ->geno & G_UNIQ))) && attempts < 50000);

			if (ptrZ && ptrZ->geno & G_UNIQ) {
				(void) makemon(ptrZ, 0, 0, MM_ANGRY);
				pline("DAMDAMDAM DAMDAMDAM DAMDAMDAMDAMDAMDAM, *DOOH-DODOMM DODODODOOH-DODOMM! DOOH-DODOMM DODODODOOH-DODOMM!*");
			}
			else if (rn2(50)) {
				attempts = 0;
				goto newbossDAM;
			}

		}

		u.mondiffhack = 0;
		u.aggravation = 0;
	}

	if (book2->oartifact == ART_AMUSINGE_BOOZER) {
		make_confused(HConfusion + rn1(15, 15), FALSE);
		You_feel("intoxicated...");
	}

	if (book2->oartifact == ART_OH_WHAT_A_GOOD_SAME_ME_THR) {
		if (u.ualign.type == A_LAWFUL) {
			adjalign(10);
			u.alignlim += 1;
			You("feel pious.");
		}
		do_vicinity_map();
	}

	if (book2->oartifact == ART_KAY_OR_STAU_NE) {
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_highwayfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("You build a highway to the left!");
	}

	if (book2->oartifact == ART_RADIO_BLA_BLA) {
		u.suppress_dim += 2000;
		You_feel("resistant to dimness!");
	}

	if (book2->oartifact == ART_SUNLIGHT_ONLY_ONE_MORE) {
		int ulx, uly;
		for (ulx = 1; ulx < (COLNO); ulx++)
	        for (uly = 0; uly < (ROWNO); uly++) {
			levl[ulx][uly].lit = 1;
		}
		pline("Everything's lit up!");
	}

	if (book2->oartifact == ART_IS_THERE_A_GIRLCOTT_) {
		cure_feminism_traps();
		You("decide to girlcott the feminism traps, because a boycott would be ineffective.");
	}

	if (book2->oartifact == ART_TO_SHIT) {
		u.uhitincxtra -= 1;
		You_feel("very clumsy.");
		if (!rn2(3)) {
			u.udamincxtra += 1;
			You_feel("very strong!");
		}
	}

	if (book2->oartifact == ART_TOO_LATE_TO_PULL_A_CHILD) {
		change_luck(-1);
		You_feel("unlucky.");
	}

	if (book2->oartifact == ART_WARP_CORE_OPEN) {
		register struct monst *warpcoremon;
		warpcoremon = makemon(mkclass(S_VORTEX,0), u.ux, u.uy, MM_ADJACENTOK);
		if (warpcoremon) {
			tamedog(warpcoremon, (struct obj *) 0, FALSE);
			pline_The("warp core turned into a vortex!");
		}

	}

	if (book2->oartifact == ART_HUNDREDS_OF_WOODEN_PALISAD) {
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_farmfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("Farmland appears.");
	}

	if (book2->oartifact == ART_SUI_SUI) {
		u.antidreameater += 5000;
		You_feel("more protected from dream eater attacks.");
	}

	if (book2->oartifact == ART_TEAM_PRIDE) {
		register struct monst *pridemon;

		if (!uinsymbiosis) {

			pridemon = makemon(specialtensmon(274), u.ux, u.uy, MM_ADJACENTOK); /* AD_DFOO */

			if (pridemon) {
				turnmonintosymbiote(pridemon, FALSE); /* WARNING: pridemon is removed at this point */
			}
		}
	}

	if (book2->oartifact == ART_THAT_S_SO_MIRA) {
		int i, j, bd = 100;
		for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if (levl[u.ux + i][u.uy + j].typ == ROOM || levl[u.ux + i][u.uy + j].typ == CORR) {
				levl[u.ux + i][u.uy + j].typ = URINELAKE;
				blockorunblock_point(u.ux + i,u.uy + j);
				if (!(levl[u.ux + i][u.uy + j].wall_info & W_EASYGROWTH)) levl[u.ux + i][u.uy + j].wall_info |= W_HARDGROWTH;
				del_engr_at(u.ux + i, u.uy + j);
	
				newsym(u.ux + i,u.uy + j);
			}
		}
		pline("Ewwwwww, there's urine everywhere... that's so Mira...");
		u.miraspawncounter++;
		upnivel(FALSE);
		u.uhpmax++;
		if (Upolyd) u.mhmax++;
		if (uinsymbiosis) {
			u.usymbiote.mhpmax++;
			maybe_evolve_symbiote();
			if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
		}

		flags.botl = TRUE;
	}

	if (book2->oartifact == ART_KLARISTO_) {
		incr_itimeout(&HSee_invisible, rnz(5000));
		pline("You can see invisible things!");
	}

	if (book2->oartifact == ART_POSSIBLE_BIO_READING) {
		u.urealedibility += 10;
		pline("Possible bio reading.");
	}

	if (book2->oartifact == ART____MINUTES_WITHOUT_WLAN) {
		u.powerfailure += 1500;
		incr_itimeout(&HInvis, 1500);
		incr_itimeout(&HStealth, 1500);
		You("have to spend the next 15 minutes without WLAN.");
	}

	if (book2->oartifact == ART_WENDSDAY__WENDSDAY__WE) {
		if (week_day() == 3) {
			incr_itimeout(&HSwimming, 5000);
			u.tempwaterprotection += 5000;
			pline("It actually is Wendsday, err Wednesday, so you can swim unharmed for a long time!");
		} else {
			incr_itimeout(&HSwimming, 1000);
			u.tempwaterprotection += 1000;
			pline("You can swim unharmed for some time.");
		}
	}

	if (book2->oartifact == ART_SOUTH_WEST_MOSAMBIK) {
		int madepoolQ = 0;

		do_clear_areaX(u.ux, u.uy, rno(10), do_megafloodings, (void *)&madepoolQ);
		if (madepoolQ)
			pline("Oh %s, there's %s flooding on this sub level!", rn2(2) ? "damn" : "great", rn2(2) ? "a" : "another");

	}

	if (book2->oartifact == ART_ARMED_FORCES_ARM_REAL) {
		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}
		coord cc, dd;
		int cx,cy, randsp, i;

		cx = rn2(COLNO);
		cy = rn2(ROWNO);

		randsp = rn1(8,8);

		for (i = 0; i < randsp; i++) {
			if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

			(void) makemon(squadmon(), cx, cy, MM_ADJACENTOK|MM_ANGRY);

		}

		u.aggravation = 0;

		You_feel("that the army is chasing after you!");
	}

	if (book2->oartifact == ART_RARE_POKESPAWNAGE) {
		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}
		coord cc, dd;
		int cx,cy, randsp, i;

		cx = rn2(COLNO);
		cy = rn2(ROWNO);

		randsp = rn1(8,8);

		for (i = 0; i < randsp; i++) {
			if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

			(void) makemon(specialtensmon(!rn2(3) ? 195 : rn2(2) ? 243 : 299), cx, cy, MM_ADJACENTOK); /* AD_COLD, AD_FRZE or AD_ICEB */

		}

		u.aggravation = 0;

		You_feel("a rather cold aura...");

	}

	if (book2->oartifact == ART_HIGH_SCORING_TRICK) {
		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		coord cc, dd;
		int cx,cy, randsp, i;

		cx = rn2(COLNO);
		cy = rn2(ROWNO);

		randsp = rnd(15);

		for (i = 0; i < randsp; i++) {

			if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

			(void) makemon(insidemon(), cx, cy, MM_ADJACENTOK); /* M5_SPACEWARS */
		}
		pline("Arabella has joined the game. Moloch's minions get stronger.");

		u.aggravation = 0;

	}

	if (book2->oartifact == ART_HEAVY_THUNDERHALL) {
		incr_itimeout(&HWildWeatherEffect, rnz(10000));
		pline_The("weather starts changing randomly!");
	}

	if (book2->oartifact == ART_SOMMERDEN_WANT_TO_BE_USED_) {
		pline_The("sun comes out.");
		u.currentweather = WEATHER_SUNNY;
		tell_main_weather();
	}

	if (book2->oartifact == ART_RAINZONE) {
		pline("It starts to rain.");
		u.currentweather = WEATHER_RAIN;
		tell_main_weather();
	}

	if (book2->oartifact == ART_PAINKILLER_THING) {
		u.temppainresist += 10000;
		You_feel("immune to pain.");
	}

	if (book2->oartifact == ART_GARY_S_MEGA_CONTRO) {
		incr_itimeout(&HControlMagic, 5000);
		You("feel more capable of controlling your magic!");
	}

	if (book2->oartifact == ART_DANGER_BYGONE) {
		You_feel("out of the danger zone.");
		struct trap *ttmp;

		int i, j;

		for (i = -2; i <= 2; i++) for(j = -2; j <= 2; j++) {

			if ((ttmp = t_at(u.ux + i, u.uy + j)) != 0) {
			    if (ttmp->ttyp == MAGIC_PORTAL) continue;
			    if (ttmp->ttyp == ONE_WAY_PORTAL) continue;
				deltrap(ttmp);
			}

		}

		(void) doredraw();

	}

	if (book2->oartifact == ART_SHUFFLE_O_MATIC) {
		register struct monst *nexusmon, *nextmon;

		for(nexusmon = fmon; nexusmon; nexusmon = nextmon) {
			nextmon = nexusmon->nmon; /* trap might kill mon */
			if (DEADMONSTER(nexusmon)) continue;
			if (nexusmon == u.usteed) continue;

			if (nexusmon->mtrapped) {
			    /* no longer in previous trap (affects mintrap) */
			    nexusmon->mtrapped = 0;
			    fill_pit(nexusmon->mx, nexusmon->my);
			}
			wakeup(nexusmon); /* monster becomes hostile */
			rloc(nexusmon, FALSE);
		}

	}

	if (book2->oartifact == ART_REINFORCE___) {
		if (spellid(0) == NO_SPELL) {
			You("don't know any spells, and therefore you cannot add spell memory to them either.");
			return;
		}

		pline("Choose a spell to add spell memory.");
addxtragainSS:
		if (!addsomespellmemoryX()) {
			if (yn("Really exit with no spell selected?") == 'y')
				pline("You just wasted the opportunity to add memory to a spell.");
			else goto addxtragainSS;
		}

	}

	if (book2->oartifact == ART_LOCKEMOUT) {
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_lockfloodg, (void *)&madepoolQ);
		tele();
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_lockfloodg, (void *)&madepoolQ);
		You_feel("locked out.");
	}

	if (book2->oartifact == ART_VITABACK) {
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
		Glib = 0;
	}

	if (book2->oartifact == ART_SWEET_DREAMS_ARE_MADE_OF_S) {
		if(HSleep_resistance & INTRINSIC) {
			HSleep_resistance &= ~INTRINSIC;
		} 
		if(HSleep_resistance & TIMEOUT) {
			HSleep_resistance &= ~TIMEOUT;
		}
		You_feel("tired...");
		u.steeldreamcounter++;
		u.uprops[DEAC_SLEEP_RES].intrinsic += (u.steeldreamcounter * 10000);
		fall_asleep(-(u.steeldreamcounter * 10), TRUE);

		register struct obj *melating;
		pline("You may change the material of an item to steel.");
melatechoice:
		melating = getobj(allowall, "change the material of");
		if(!melating) {
			if (yn("Really exit with no object selected?") == 'y') {
				pline("You just wasted the opportunity to change an item's material.");
			}
			else goto melatechoice;
		} else if ((melating->otyp == GOLD_PIECE) || (melating->otyp == STRANGE_OBJECT) || (melating->otyp == AMULET_OF_YENDOR) || (melating->otyp == CANDELABRUM_OF_INVOCATION) || (melating->otyp == BELL_OF_OPENING) || (melating->otyp == SPE_BOOK_OF_THE_DEAD) || (objects[melating->otyp].oc_prob < 1)) {
			pline("The material of that item cannot be changed!");
		} else if (melating) {

			objects[melating->otyp].oc_material = MT_STEEL;
			pline_The("target item is made of steel now.");
		}
		You("fall asleep. Sweet dreams...");
	}

	if (book2->oartifact == ART_MAYONAISE_SHOULD_HAVE_BEEN) {
		pline("You lose  Wisdom");
		if (PlayerHearsSoundEffects) pline(issoviet ? "Pochemu vy ne sledite luchshe dlya vashikh atributov, tak ili inache?" : "Due-l-ue-l-ue-l!");
		ABASE(A_WIS) -= 1;
		AMAX(A_WIS) -= 1;
		flags.botl = 1;

		if(ABASE(A_WIS) < ATTRABSMIN(A_WIS)) {
			u.youaredead = 1;
			pline("Well, you really should have been more wise... your body shuts down forever.");
			killer = "not being wise enough";
			killer_format = KILLED_BY;
			done(DIED);
			/* lifesaved */
			u.youaredead = 0;
			if(ABASE(A_WIS) < ATTRABSMIN(A_WIS)) {
				ABASE(A_WIS) += 1;
				AMAX(A_WIS) += 1;
			}
		}

		if (!rn2(3)) {
			intrinsicgainorloss(1);
		} else if (!rn2(5)) {
			intrinsicgainorloss(2);
		}
	}

	if (book2->oartifact == ART_ENTROPY_S_LEDGER) {
		losexp("entropy ledger", TRUE, FALSE);
		u.temptriplemf += rnz(5000);
		You_feel("more capable of finding magic items!");
	}

	if (book2->oartifact == ART_BEAT_MY_LOVER) {
		register struct monst *nexusmon, *nextmon;

		for(nexusmon = fmon; nexusmon; nexusmon = nextmon) {
			nextmon = nexusmon->nmon; /* trap might kill mon */
			if (DEADMONSTER(nexusmon)) continue;

			if (!is_male(nexusmon->data)) continue;

			nexusmon->mhp -= u.ulevel;
			if (nexusmon->mhp <= 0) {
				xkilled(nexusmon, 0);
				pline("%s was beaten to death!", Monnam(nexusmon));
			}
			else {
				pline("%s was beaten up!", Monnam(nexusmon));
				wakeup(nexusmon); /* monster becomes hostile */
			}
		}
	}

	if (book2->oartifact == ART_DOLCEVAIN) {
		u.thornspell += rnz(1000);
		You("grow a thorny hide!");
	}

	if (book2->oartifact == ART_LEYDE_HIR_MI_TONIGHT) {
		if (Aggravate_monster) {
			u.aggravation = 1;
			reset_rndmonst(NON_PM);
		}

		register struct monst *leydemon;
		leydemon = makemon(courtmon(), u.ux, u.uy, MM_ADJACENTOK);
		if (leydemon) {
			if (is_female(leydemon->data)) {
				tamedog(leydemon, (struct obj *) 0, FALSE);
				pline("The leyde hirs yu tonight!");
			} else pline("Oh, looks like your plea was in vain.");
		}

		u.aggravation = 0;
	}

	if (book2->oartifact == ART_WHOA__THERE_ARE_LOUD_WANDS) {
		struct obj *uammo;
		uammo = mkobj(WAND_CLASS, FALSE, FALSE);
		if (uammo) {
			uammo->quan = 1;
			uammo->owt = weight(uammo);
			dropy(uammo);
			stackobj(uammo);
			pline("A wand has formed on the ground!");
		}
	}

	if (book2->oartifact == ART_LIKE_GEMOLT) {
		u.currentweather = WEATHER_OVERCAST;
		tell_main_weather();
	}

	if (book2->oartifact == ART_SUEYUE_SAA_A) {
		incr_itimeout(&HRegeneration, 2000);
		You("start regenerating.");
	}

	if (book2->oartifact == ART_NINETY_NATE_CHI) {
		adjattrib(A_INT, 1, FALSE, TRUE);
	}

	if (book2->oartifact == ART_SWEAN__YOU_KNOW) {
		adjattrib(A_CHA, 1, FALSE, TRUE);
	}

	if (book2->oartifact == ART_LESS_MUST_MUST) {
		incr_itimeout(&HFull_nutrient, 5000);
		Your("food consumption slows down.");
	}

	if (book2->oartifact == ART___EJOHM_V_Y_EJOHM_V_Y_EJOH) {
		pline((FunnyHallu || Confusion) ?	"Oops! Alarm:" : "Oops! Here is alarm!");
		aggravate();
	}

	if (book2->oartifact == ART_YOU_ARE_STOP_UNABLE) {
		intrinsicgainorloss(2); /* lose an intrinsic */
	}

	if (book2->oartifact == ART_FIDEDEDE_DE_DEDEDE_DEDEDE) {
		incr_itimeout(&HJumping, 1000);
		You("feel like Super Mario!");
	}

	if (book2->oartifact == ART_SCHUB_SCHUB) {
		int i, ii, lim;

		i = rn2(A_MAX);		/* start at a random point */
		for (ii = 0; ii < A_MAX; ii++) {
			lim = AMAX(i);
			if (ABASE(i) < lim) {
				ABASE(i)++;
				pline("Wow! This makes you feel good!");
				flags.botl = 1;
				break; /* only restore one --Amy */
			}
			if(++i >= A_MAX) i = 0;
		}

	}

	if (book2->oartifact == ART_W_W_W_W_W_W_W_W_W_W_W_) {
		u.tempwwwww += 25000;
	}

	if (book2->oartifact == ART_HARENWAH) {
		u.tempharenwah += 2000;
		You("feel that the animals will be peaceful.");
	}

	if (book2->oartifact == ART_NOPEFLUID) {
		u.tempnofluidators += 100;
		You("are safe from the fluidators for the time being.");
	}

	if (book2->oartifact == ART_MYHEART_MAGIGI) {
		incr_itimeout(&HManaleech, 2500);
		You("start leeching mana!");
	}

	if (book2->oartifact == ART_MANUAL_OF_THE_THOUSAND_HAN) {
		incr_itimeout(&HManaleech, 2000);
		MonsterSpeedBug += 2000;
		u.tempxtrapwregen += 2000;
		You("start leeching mana, and also regenerate mana more quickly!");
	}

	if (book2->oartifact == ART_DAILY_FRESHE_BREAD) {
		struct obj *uammo;
		uammo = mksobj(BREAD, TRUE, FALSE, FALSE);
		if (uammo) {
			uammo->quan = rnd(3);
			uammo->owt = weight(uammo);
			dropy(uammo);
			stackobj(uammo);
			pline("Some bread has formed on the ground!");
		}

	}

	if (book2->oartifact == ART_PART_PIZZA) {
		struct obj *uammo;
		uammo = mksobj(SLICE_OF_PIZZA, TRUE, FALSE, FALSE);
		if (uammo) {
			uammo->quan = rnd(8);
			uammo->owt = weight(uammo);
			dropy(uammo);
			stackobj(uammo);
			pline("Some slices of pizza have formed on the ground!");
		}

	}

	if (book2->oartifact == ART_GOLEMEQUIPMENT) {
		struct obj *uammo;
		uammo = mksobj(TORCH, TRUE, FALSE, FALSE);
		if (uammo) {
			uammo->quan = rnz(10);
			uammo->owt = weight(uammo);
			dropy(uammo);
			stackobj(uammo);
			pline("A torch has formed on the ground!");
		}

	}

	if (book2->oartifact == ART_NOW_LEVEL_UP_AT_ONCE_) {
		incr_itimeout(&HExpBoost, 1000);
		pline("Quickly! You'll get bonus experience!");
	}

	if (book2->oartifact == ART_YEARLY_WONDER) {
		incr_itimeout(&HWonderlegs, rnz(2000));
		Your("%s feel much better!", makeplural(body_part(LEG)) );
	}

	if (book2->oartifact == ART_LIKE_A_CANDLE_IN_THE_WIND) {
		struct obj *uammo;
		uammo = mksobj(TALLOW_CANDLE, TRUE, FALSE, FALSE);
		if (uammo) {
			uammo->quan = rnz(35);
			uammo->owt = weight(uammo);
			dropy(uammo);
			stackobj(uammo);
			pline("A stack of candles has formed on the ground!");
		}

	}

	if (book2->oartifact == ART_SMELL_THE_COFFEE) {
		struct obj *uammo;
		uammo = mksobj(POT_COFFEE, TRUE, FALSE, FALSE);
		if (uammo) {
			uammo->quan = 1;
			uammo->owt = weight(uammo);
			dropy(uammo);
			stackobj(uammo);
			pline("A mug of coffee has formed on the ground!");
		}

	}

	if (book2->oartifact == ART_SOCKETED_ANGULAR_SIGN) {
		struct obj *uammo;
		uammo = mksobj(DILITHIUM_CRYSTAL + rn2((JADE + 1) - DILITHIUM_CRYSTAL), TRUE, FALSE, FALSE);
		if (uammo) {
			uammo->quan = 1;
			uammo->owt = weight(uammo);
			dropy(uammo);
			stackobj(uammo);
			pline("A gem has formed on the ground!");
		}

	}


	if (book2->oartifact == ART_ICE_BELONGS_IN_THE_ICE) {

		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_icefloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("The landscape is winterized!");

		struct obj *uammo;
		uammo = mksobj(ICE_BLOCK, TRUE, FALSE, FALSE);
		if (uammo) {
			uammo->quan = rn1(25, 25);
			uammo->owt = weight(uammo);
			dropy(uammo);
			stackobj(uammo);
			pline("A stack of ice blocks has formed on the ground!");
		}

	}

	if (book2->oartifact == ART_ZI_U_U_U_U_U_UUUUU) {
		struct obj *uammo;
		uammo = mksobj(randartcrossbowbolt(), TRUE, FALSE, FALSE);
		if (uammo) {
			uammo->quan = rnz(50);
			uammo->owt = weight(uammo);
			dropy(uammo);
			stackobj(uammo);
			pline("A stack of crossbow ammo has formed on the ground!");
		}
	}

	if (book2->oartifact == ART_KING_IN_YELLOW) {
		int kiypm;
		kiypm = dprince(rn2((int)A_LAWFUL+2) - 1);
		if (kiypm >= PM_ORCUS && kiypm <= PM_DEMOGORGON) u.conclusiocount++;
		if (kiypm && (kiypm != NON_PM)) {
			(void) makemon(&mons[kiypm], u.ux, u.uy, MM_ANGRY|MM_FRENZIED);
			pline("An angry demon is summoned!");
		}
	}

	if (book2->oartifact == ART_IWA_ERWI) {
		int iwaerwi;
		if (rn2(2)) {
			if (objects[SPE_DEFUSING].oc_level < 8) objects[SPE_DEFUSING].oc_level++;
		} else {
			if (objects[SPE_DEFUSING].oc_level > 1) objects[SPE_DEFUSING].oc_level--;
		}

		for (iwaerwi = 0; iwaerwi < MAXSPELL && spellid(iwaerwi) != NO_SPELL; iwaerwi++) {
			if (spellid(iwaerwi) == SPE_DEFUSING) {
				spl_book[iwaerwi].sp_lev = objects[SPE_DEFUSING].oc_level;
			}
		}

		pline("The level of the defusing spell has changed!");
	}

	if (book2->oartifact == ART_HEALING_RAIN_OBTAINED) {
		wonderspell(SPE_HEALING_RAIN, 0);
	}

	if (book2->oartifact == ART_PRIMER_OF_SPARKS) {
		wonderspell(SPE_FLAME_SPHERE, 0);
		incr_itimeout(&HFire_resistance, 1000);
		You_feel("cooler for a while.");
	}

	if (book2->oartifact == ART_ELEMENTALIST_S_CODEX) {
		wonderspell(SPE_FIREBALL, 0);
		wonderspell(SPE_CONE_OF_COLD, 0);
	}

	if (book2->oartifact == ART_CHRONICLE_OF_THE_LAST_DAWN) {
		u.uhp = u.uhpmax;
		u.uen = u.uenmax;
		if (Upolyd) u.mh = u.mhmax;
		flags.botl = TRUE;
		u.boosttimer += 5000;
		u.combatcommand += 5000;
		You_feel("fully powered!");
	}

	if (book2->oartifact == ART_LIBRAM_OF_SPARKS) {
		wonderspell(SPE_VOLT_ROCK, 0);

		register struct obj *wandchargor;
		pline("You may improve the charge count of one of your wands.");

wandchargechoice:
		wandchargor = getobj(allowall, "boost the charge count");

		if(!wandchargor) {
			if (yn("Really exit with no object selected?") == 'y') {
				pline("You just wasted the opportunity to charge a wand.");
			}
			else goto wandchargechoice;
		} else if (wandchargor->oclass != WAND_CLASS) {
			pline("That isn't a wand, and therefore nothing happens!");
		} else if (wandchargor->otyp == WAN_WISHING || wandchargor->otyp == WAN_CHARGING || wandchargor->otyp == WAN_ACQUIREMENT || wandchargor->otyp == WAN_GAIN_LEVEL || wandchargor->otyp == WAN_INCREASE_MAX_HITPOINTS) {
			pline("Sadly, you decided to pick a wand that cannot be recharged indefinitely, even though the description of that spellbook explicitly said that you can't do that, and therefore the recharging fails. Maybe you should work on your real-life reading comprehension skills, because currently your character's is better than your own...");
		} else {
			wandchargor->spe++;
			pline_The("wand glows briefly.");
		}

	}

	if (book2->oartifact == ART_CODEX_OF_FEATHERED_WORDS) {
		incr_itimeout(&HLevitation, 100);
		HLevitation |= I_SPECIAL;
		You("float into the air!");
		heal_legs();
		if (u.inertia) {
			u.inertia -= 50;
			if (u.inertia < 1) {
				u.inertia = 0;
				You_feel("less slow.");
			}
		}
	}

	if (book2->oartifact == ART_TRICKSTER_S_MANUAL) {

		spell_metronome(1, 2); /* can only select level 1 or 2 spells */

		if (!rn2(20)) badeffect();

	}

	if (book2->oartifact == ART_CANDLELIGHT_CODEX) {
		register struct obj *pseudoobj;
		pseudoobj = mksobj(WAN_LIGHT, FALSE, 2, FALSE);
		wonderspell(SPE_LIGHT, 0);
		litroom(TRUE, pseudoobj);
		if (pseudoobj) obfree(pseudoobj, (struct obj *)0);	/* now, get rid of it */
		incr_itimeout(&HSight_bonus, 200);
		You("feel the area lighting up!");
	}

	if (book2->oartifact == ART_NOVICE_S_COMPANION) {
		u.uen += rn1(10,10);
		percentrestoremana(1);
		if (u.uen > u.uenmax) u.uen = u.uenmax;
		flags.botl = TRUE;
		You_feel("full of mystic power!");
		u.tempbooklenses += 1000;
	}

	if (book2->oartifact == ART_HEALER_S_HANDBOOK) {
		wonderspell(SPE_HEALING, 0);
		healup(rn1(10,10), 0, FALSE, FALSE);
		percentheal(2);
		incr_itimeout(&HPoison_resistance, 100);
		You_feel("healthy for the time being.");
	}

	if (book2->oartifact == ART_WEEPING_CODEX) {
		losestr(1, TRUE);
		healup(rn1(25,25), 0, FALSE, FALSE);
		percentheal(5);
		You_feel("healthier!");
		castspecificspell(SPE_BUBBLEBEAM);

	}

	if (book2->oartifact == ART_POCKET_HERBAL_GRIMOIRE) {
		healup(rn1(25,25), 0, TRUE, FALSE);
		percentheal(5);
		incr_itimeout(&HPoison_resistance, 500);
		You_feel("healthy for the time being.");
	}

	if (book2->oartifact == ART_TOME_OF_ENDLESS_HUNGER) {
		if (u.uhpmax < 11) {
			u.youaredead = 1;
			You("have been drained to the point of death...");
			killer = "endless hunger";
			killer_format = KILLED_BY;
			done(DIED);
			u.youaredead = 0;
		} else {
			u.uhpmax -= rnd(10);
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			flags.botl = TRUE;
		}
		u.tempundeaddbldmg += rnz(5000);
		wonderspell(SPE_DRAIN_LIFE, 0);
		wonderspell(SPE_COMMAND_UNDEAD, 0);
	}

	if (book2->oartifact == ART_JUST_AND_JOSEPH_LOOKING_FO) {
		if (u.ualign.type != A_LAWFUL) {
			u.alignlim -= 5;	
			adjalign(-20);
			You_feel("kinda naughty.");
		}
		god_gives_pet(A_LAWFUL);
	}

	if (book2->oartifact == ART_NECRONOMICON_OF_ABDUL_ALHA) {
		if (u.uhpmax < 11) {
			u.youaredead = 1;
			You("have been drained to the point of death...");
			killer = "endless hunger";
			killer_format = KILLED_BY;
			done(DIED);
			u.youaredead = 0;
		} else {
			u.uhpmax -= 10;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			flags.botl = TRUE;
		}

		register int randomspell;
		int attempts = 50000;

newabdulspell:
		randomspell = SPE_FORCE_BOLT + rn2((SPE_PSYBEAM + 1) - SPE_FORCE_BOLT);

		if (objects[randomspell].oc_level != 8) {
			if (attempts > 0) {
				attempts--;
				goto newabdulspell;
			}
		}

		wonderspell(randomspell, 0);
	}
	if (book2->oartifact == ART_LESSEE_DAT && Role_if(PM_ELECTRIC_MAGE)) {
		if (P_MAX_SKILL(P_ELEMENTAL_SPELL) == P_ISRESTRICTED) {
			unrestrict_weapon_skill(P_ELEMENTAL_SPELL);
			pline("You can now learn the elemental spells skill.");
		} else if (P_MAX_SKILL(P_ELEMENTAL_SPELL) == P_UNSKILLED) {
			unrestrict_weapon_skill(P_ELEMENTAL_SPELL);
			P_MAX_SKILL(P_ELEMENTAL_SPELL) = P_BASIC;
			pline("You can now learn the elemental spells skill.");
		} else if (rn2(2) && P_MAX_SKILL(P_ELEMENTAL_SPELL) == P_BASIC) {
			P_MAX_SKILL(P_ELEMENTAL_SPELL) = P_SKILLED;
			pline("Your knowledge of the elemental spells skill increases.");
		} else if (!rn2(4) && P_MAX_SKILL(P_ELEMENTAL_SPELL) == P_SKILLED) {
			P_MAX_SKILL(P_ELEMENTAL_SPELL) = P_EXPERT;
			pline("Your knowledge of the elemental spells skill increases.");
		} else if (!rn2(10) && P_MAX_SKILL(P_ELEMENTAL_SPELL) == P_EXPERT) {
			P_MAX_SKILL(P_ELEMENTAL_SPELL) = P_MASTER;
			pline("Your knowledge of the elemental spells skill increases.");
		} else if (!rn2(100) && P_MAX_SKILL(P_ELEMENTAL_SPELL) == P_MASTER) {
			P_MAX_SKILL(P_ELEMENTAL_SPELL) = P_GRAND_MASTER;
			pline("Your knowledge of the elemental spells skill increases.");
		} else if (!rn2(200) && P_MAX_SKILL(P_ELEMENTAL_SPELL) == P_GRAND_MASTER) {
			P_MAX_SKILL(P_ELEMENTAL_SPELL) = P_SUPREME_MASTER;
			pline("Your knowledge of the elemental spells skill increases.");
		}
	}

}

/* attempting to cast a forgotten spell will cause disorientation */
STATIC_OVL void
spell_backfire(spell)
int spell;
{
    long duration = (long)((spellev(spell) + 1) * 3);	 /* 6..24 */

    /* prior to 3.4.1, the only effect was confusion; it still predominates */
    if (!obsidianprotection()) switch (rn2(17)) {
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

/* cast a random spell, possibly one that you don't even have --Amy */
void
spell_metronome(minsplvl, maxsplvl)
int minsplvl, maxsplvl;
{
	struct obj *pseudo;
	int attempts = 50000;

	pseudo = mksobj(SPE_MAGIC_BOLT, FALSE, 2, FALSE);
metronomeroll:
	pseudo->otyp = randartspellbookX();

	if (!pseudo) {
		pline("The spell failed spontaneously!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return;
	}

	/* random spell must always be at least "minsplvl" and cannot be higher than "maxsplvl" --Amy */
	if (attempts > 0) {
		attempts--;
		if (objects[pseudo->otyp].oc_level < minsplvl) goto metronomeroll;
		if (objects[pseudo->otyp].oc_level > maxsplvl) goto metronomeroll;
	}

	pline("You cast the '%s' spell.", OBJ_NAME(objects[pseudo->otyp]));

metronomestart:
	if (!getdir((char *)0)) {
		if (yn("Do you really want to input no direction?") == 'y')
			pline_The("magical energy is released!");
		else {
			goto metronomestart;
		}
	}

	pseudo->blessed = pseudo->cursed = 0;
	pseudo->quan = 20L;			/* do not let useup get it */

	spelleffects(pseudo->otyp, FALSE, TRUE);

	if (pseudo) obfree(pseudo, (struct obj *)0);
}

/* cast a specific spell, regardless of whether you actually have it --Amy */
void
castspecificspell(spelltype)
int spelltype;
{
	struct obj *pseudo;
	int attempts = 50000;

	pseudo = mksobj(spelltype, FALSE, 2, FALSE);

	pseudo->otyp = spelltype;

	if (!pseudo) {
		pline("The spell failed spontaneously!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return;
	}

	pline("You cast the '%s' spell.", OBJ_NAME(objects[pseudo->otyp]));

specificspellstart:
	if (!getdir((char *)0)) {
		if (yn("Do you really want to input no direction?") == 'y')
			pline_The("magical energy is released!");
		else {
			goto specificspellstart;
		}
	}

	pseudo->blessed = pseudo->cursed = 0;
	pseudo->quan = 20L;			/* do not let useup get it */

	spelleffects(pseudo->otyp, FALSE, TRUE);

	if (pseudo) obfree(pseudo, (struct obj *)0);
}

int
spelleffects(spell, atme, metronomespell)
int spell;
boolean atme;
boolean metronomespell;
{
	int energy, damage, chance, n, intell;
	int hungr;
	int skill, role_skill;
	boolean confused = ((Confusion != 0) && !(Conf_resist && (StrongConf_resist ? rn2(3) : !rn2(3)) ) );
	struct obj *pseudo;
	struct obj *otmp;
	int confusionchance = 0;

	if (metronomespell) {
		skill = 0;
		role_skill = 0;
		goto metronomedoit;
	}

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

		/* overhaul by Amy: forgotten spells are no longer free, they cost Pw now and can fail or backfire
		 * higher-level spells cost less mana to cast, THIS IS NOT AN ERROR, it's because those will also generally
		 * have a higher fail rate, because I want there to be no easy answer for the question of "which is the best
		 * spell to turn into a forgotten spell?" - low-level ones will have a low fail rate but cost much Pw,
		 * high-level ones will have a high fail rate (unless you're a pretty good spellcaster) but cost little Pw */
		int forgottencost;
		forgottencost = ((9 - spellev(spell)) * 5);
		if (u.uhave.amulet && u.amuletcompletelyimbued && !u.freeplaymode) {
			You_feel("the amulet draining your energy away.");
			forgottencost += rnd(2*forgottencost);

			if (u.uen < forgottencost) {
				u.uen = 0;
				pline("You are exhausted, and fail to invoke the forgotten spell due to the amulet draining all your energy away.");
				return(1);
			}
		}
		if (u.uen < forgottencost) {
			You("don't have enough energy to cast that spell. The required amount was %d.", forgottencost);
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			return(0);
		}
		u.uen -= forgottencost;

		Your("knowledge of this spell is twisted.");

		if (rnd(100) > (percent_success(spell))) {
			pline("The attempt to invoke the forgotten spell failed.");
			if (!rn2(10)) {
				pline("In fact, you've screwed up badly enough for it to backfire...");
				badeffect();
			}
			return(1);
		}

		pline("It invokes nightmarish images in your mind...");
		u.cnd_forgottenspellcount++;
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
	energy = manacost(spellid(spell));
	if (SpellColorYellow) energy *= 2;
	if (SpellColorWhite) energy *= 4;

	if (WinceState) {
		energy *= 6;
		energy /= 5;
		energy++;
		if (StrongWinceState) {
			energy *= 6;
			energy /= 5;
			energy++;
		}
	}
	if (u.martialstyle == MARTIALSTYLE_MUAYTHAI) {
		energy *= 11;
		energy /= 10;
	}
	if (ACURR(A_WIS) == 2) {
		energy *= 11;
		energy /= 10;
	}
	if (ACURR(A_WIS) == 1) {
		energy *= 12;
		energy /= 10;
	}

	/* inertia control and spellbinder make spells a bit more expensive... --Amy */
	if (u.inertiacontrol) {
		energy *= 5;
		energy /= 4;
	}
	if (u.spellbinder) {
		energy *= 5;
		energy /= 4;
	}
	/* if you have both active, it's even more expensive */
	if (u.spellbinder && u.inertiacontrol) {
		energy *= 4;
		energy /= 3;
	}

	/* only being easier to cast is not good enough for the "special spell", since you can't have a failure rate
	 * lower than 0%. Reduce cost of casting the special spell to 80%! --Amy */
	if (spellid(spell) == urole.spelspec) { if (rn2(10)) energy += 1; energy *= 4; energy /= 5; }
	if (spellid(spell) == u.superspecialspell) { if (rn2(10)) energy += 1; energy *= 4; energy /= 5; }

	if (tech_inuse(T_SPELL_SPAM) && rn2(10)) {
		energy += 1; energy *= 9; energy /= 10;
	}

	/* slight mana cost decrease if you're very skilled, to make skill matter more --Amy */
	if (role_skill == P_SKILLED) { if (rn2(10)) energy += 1; energy *= 19; energy /= 20;}
	if (role_skill == P_EXPERT) { if (rn2(10)) energy += 1; energy *= 18; energy /= 20;}
	if (role_skill == P_MASTER) { if (rn2(10)) energy += 1; energy *= 17; energy /= 20;}
	if (role_skill == P_GRAND_MASTER) { if (rn2(10)) energy += 1; energy *= 16; energy /= 20;}
	if (role_skill == P_SUPREME_MASTER) { if (rn2(10)) energy += 1; energy *= 15; energy /= 20;}

	if (Role_if(PM_MAHOU_SHOUJO) && energy > 1) {  /* Casting any sort of magic uses half power for them */
		if (rn2(10)) energy += 1;
		energy /= 2;
	}

	if (powerfulimplants() && uimplant && uimplant->oartifact == ART_FOCUS_GEM_OF_KAEDRA_MOONVE && energy > 1) {
		if (rn2(10)) energy += 1;
		energy *= 4;
		energy /= 5;
	}

	if (u.tempspellcost90 && energy > 1) {
		if (rn2(10)) energy += 1;
		energy *= 9;
		energy /= 10;
	}

	if (uarm && uarm->oartifact == ART_HLOVAL_S_PROBLEM && energy > 1) {
		if (rn2(10)) energy += 1;
		energy *= 3;
		energy /= 4;
	}

	if (ublindf && ublindf->oartifact == ART_MEANINGFUL_CHALLENGE && energy > 1) {
		if (rn2(10)) energy += 1;
		energy *= 9;
		energy /= 10;
	}

	if (uarmh && uarmh->oartifact == ART_FULLY_WORKING_AT_HALF_MAXI) {
		if (rn2(10)) energy += 1;
		energy /= 2;
	}

	if (uwep && uwep->oartifact == ART_MANA_METER_BOOSTER) {
		if (rn2(10)) energy += 1;
		energy *= 9;
		energy /= 10;
	}

	if (uarmc && uarmc->oartifact == ART_ZIRATHA_S_CASTING_LEGEND) {
		if (rn2(10)) energy += 1;
		energy *= 9;
		energy /= 10;
	}

	if (uarmg && itemhasappearance(uarmg, APP_UNCANNY_GLOVES)) {
		energy *= 11;
		energy /= 10;
	}

	if (uleft && uleft->oartifact == ART_HENRIETTA_S_MAGICAL_AID) {
		if (rn2(10)) energy += 1;
		energy *= 4;
		energy /= 5;
	}

	if (Race_if(PM_BACTERIA)) {
		if (rn2(10)) energy += 1;
		energy *= 4;
		energy /= 5;
	}

	if (Upolyd && dmgtype(youmonst.data, AD_SPEL) ) {
		if (rn2(10)) energy += 1;
		energy *= 19;
		energy /= 20;
	}
	if (Upolyd && dmgtype(youmonst.data, AD_CLRC) ) {
		if (rn2(10)) energy += 1;
		energy *= 19;
		energy /= 20;
	}
	if (Upolyd && dmgtype(youmonst.data, AD_CAST) ) {
		if (rn2(10)) energy += 1;
		energy *= 9;
		energy /= 10;
	}

	if (uarmc && uarmc->oartifact == ART_GAGARIN_S_TRANSLATOR) {
		if (rn2(10)) energy += 1;
		energy *= 9;
		energy /= 10;
	}

	if (uright && uright->oartifact == ART_HENRIETTA_S_MAGICAL_AID) {
		if (rn2(10)) energy += 1;
		energy *= 4;
		energy /= 5;
	}
	if (Role_if(PM_ELEMENTALIST) && skill == P_ELEMENTAL_SPELL) {
		if (rn2(10)) energy += 1;
		energy *= 3;
		energy /= 4;
	}

	if ((uarmg && itemhasappearance(uarmg, APP_OCCULTISM_GLOVES)) && skill == P_OCCULT_SPELL) {
		if (rn2(10)) energy += 1;
		energy *= 4;
		energy /= 5;
	}

	if (uarmc && uarmc->oartifact == ART_INA_S_APPRENTICESHIP && skill == P_HEALING_SPELL) {
		if (rn2(10)) energy += 1;
		energy *= 4;
		energy /= 5;
	}

	if (Race_if(PM_MANSTER) && energy > 1) {
		if (rn2(10)) energy += 1;
		energy *= 2;
		energy /= 3;
	}

	if (powerfulimplants() && uimplant && uimplant->oartifact == ART_DOMPFINATION) {
		if (rn2(10)) energy += 1;
		energy *= 9;
		energy /= 10;
	}

	if (Role_if(PM_MAHOU_SHOUJO) && (energy > 1) && uarmc && itemhasappearance(uarmc, APP_WEEB_CLOAK) ) {
		if (rn2(10)) energy += 1;
		energy *= 9;
		energy /= 10;
	}

	if (uwep && uwep->oartifact == ART_WUFFIAN_RADIO && (spellid(spell) == SPE_LIGHT_AREA) ) {
		if (rn2(10)) energy += 1;
		energy *= 2;
		energy /= 3;
	}

	if (uarm && uarm->oartifact == ART_HELEN_S_REVEAL && (spellid(spell) == SPE_CHARM_MONSTER) ) {
		if (rn2(10)) energy += 1;
		energy /= 2;
	}

	/* Fail safe. Spellcasting should never become too inexpensive. --Amy */
	if (energy < 2) energy = rn2(10) ? 2 : 1;

	if (u.uhunger <= 10 && spellid(spell) != SPE_DETECT_FOOD && spellid(spell) != SPE_SATISFY_HUNGER && spellid(spell) != SPE_KEEP_SATIATION) {
		You("are too hungry to cast that spell.");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return(0);
	} else if (ACURR(A_STR) < 4 && !(Role_if(PM_CELLAR_CHILD) && uwep && (weapon_type(uwep) == P_QUARTERSTAFF)) )  {
		You("lack the strength to cast spells.");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return(0);
	} else if(rn2(4) && check_capacity(
		"Your concentration falters while carrying so much stuff.")) { /* 25% chance to be able to cast when overtaxed; lose a turn if it fails --Amy */
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return (1);
	} else if (!freehandX() && !(uwep && uwep->oartifact == ART_MAGE_STAFF_OF_ETERNITY) && !(Role_if(PM_CELLAR_CHILD) && uwep && (weapon_type(uwep) == P_QUARTERSTAFF)) ) {
		Your("arms are not free to cast!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return (0);
	} else if (u.spellcooldown > 0) {
		pline("Spell cooldown. Wait %d more turns.", u.spellcooldown);
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return (0);
	} else if ((Muteness || u.uprops[MUTENESS].extrinsic || have_mutenessstone() || (uarmh && uarmh->oartifact == ART_CLELIA_S_TONGUE_BREAKER) ) && rn2(10)) {
		pline("You're muted, and fail to cast the spell!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		u.uen -= rnd(20); /* arbitrary; you're supposed to still be able to cast a little sometimes --Amy */
		if (u.uen < 0) u.uen = 0;
		return (0);
	} else if (tech_inuse(T_SILENT_OCEAN)) {
		pline("The silent ocean prevents you from spellcasting.");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return (0);
	} else if (isevilvariant && (Strangled || is_silent(youmonst.data) || !has_head(youmonst.data) || youmonst.data->msound == MS_BUZZ || youmonst.data->msound == MS_BURBLE || youmonst.data->msound == MS_GURGLE || youmonst.data->msound == MS_MUTE)) {
		pline("You're unable to chant any magical formula!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return (0);
	}

	/* Casting any sort of magic as a mahou shoujo or naga does not cause hunger */
	/* Amy edit: but if you're satiated, you always use the standard amount of nutrition. That way, hungerless casting
	 * does not rob you of the ability to get out of satiated status by repeatedly casting spells. */

		if ((!Role_if(PM_MAHOU_SHOUJO) && !Race_if(PM_HUMANLIKE_NAGA) && (spellid(spell) != SPE_DETECT_FOOD) && (spellid(spell) != SPE_SATISFY_HUNGER) && (spellid(spell) != SPE_KEEP_SATIATION) ) || u.uhunger > 2500 ) {
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

		/* if you're supposed to get hungry, then it shouldn't be reduced to zero --Amy */
		if (hungr < 1) hungr = 1;
	}
	else
		hungr = 0;
			/* don't put player (quite) into fainting from
			 * casting a spell, particularly since they might
			 * not even be hungry at the beginning; however,
			 * this is low enough that they must eat before
			 * casting anything else except detect food
			 */

	if (u.uhave.amulet && u.amuletcompletelyimbued && !u.freeplaymode) {
		/* casting while you have the fully imbued amulet always causes extra hunger no matter what --Amy
		 * but a non-imbued one doesn't increase hunger cost */
		hungr += rnd(2*energy);
	}

			if (hungr > u.uhunger-3)
				hungr = u.uhunger-3;
	if (energy > u.uen)  {
		if (SpellColorPlatinum) {
			You("overcast the spell. Magic reaction hurts you!");
			goto castanyway;
		}

		if (role_skill >= P_SKILLED || Race_if(PM_BACTERIA)) You("don't have enough energy to cast that spell.");
		else You("don't have enough energy to cast that spell. The required amount was %d.",energy);
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		/* WAC/ALI Experts can override with HP/hunger loss */
		if ((role_skill >= P_SKILLED || Race_if(PM_BACTERIA)) && (u.uhpmax > (energy / 5)) && (yn("Continue? Doing so may damage your maximum health.") == 'y')) {
			energy -= u.uen;
			hungr += energy * 2;
			if (hungr > u.uhunger - 1)
				hungr = u.uhunger - 1;

			if (energy > 4 && (!Race_if(PM_BACTERIA) || !rn2(2))) {
			/* otherwise, skilled godmode at 0% fail equals instawin. --Amy */
				pline("Your maximum health was reduced by %d.", energy / 5);
				u.uhpmax -= (energy / 5);
				if (u.uhpmax < 1) {
					u.uhpmax = 1;
					u.youaredead = 1;
					done(DIED);
					u.youaredead = 0;
				}
				u.uhp -= (energy / 5);
				if (u.uhp < 1) {
					u.youaredead = 1;
					done(DIED);
					u.youaredead = 0;
				}
			} else if (energy < 5 && (!Race_if(PM_BACTERIA) || !rn2(2))) {
				pline("Your maximum health was reduced by 1.");
				u.uhpmax -= 1;
				if (u.uhpmax < 1) {
					u.uhpmax = 1;
					u.youaredead = 1;
					done(DIED);
					u.youaredead = 0;
				}
				u.uhp -= 1;
				if (u.uhp < 1) {
					u.youaredead = 1;
					done(DIED);
					u.youaredead = 0;
				}
			}
			flags.botl = TRUE;

			losehp(energy,"spellcasting exhaustion", KILLED_BY);
			if (role_skill < P_EXPERT) exercise(A_WIS, FALSE);
			energy = u.uen;
		} else {
			if (role_skill >= P_SKILLED || Race_if(PM_BACTERIA)) pline("The required amount was %d.",energy);
			return 0;
		}
	}

castanyway:

	/* come on, you should be able to cast using less nutrition if your skill is higher! --Amy */

	if ((hungr > 0) && (u.uhunger <= 2500)) { /* But only if you're not satiated (see above) */

		if (!PlayerCannotUseSkills) {
			if ( role_skill == P_BASIC) {hungr *= 85; hungr /= 100;}
			if ( role_skill == P_SKILLED) {hungr *= 70; hungr /= 100;}
			if ( role_skill == P_EXPERT) {hungr *= 55; hungr /= 100;}
			if ( role_skill == P_MASTER) {hungr *= 40; hungr /= 100;}
			if ( role_skill == P_GRAND_MASTER) {hungr *= 25; hungr /= 100;}
			if ( role_skill == P_SUPREME_MASTER) {hungr *= 10; hungr /= 100;}
		}

		/* casting it often (and thereby keeping it in memory) should also reduce hunger... */
		if ( spellknow(spell) >= 10000) {hungr *= 9; hungr /= 10;}
		if ( spellknow(spell) >= 20000) {hungr *= 9; hungr /= 10;}
		if ( spellknow(spell) >= 30000) {hungr *= 9; hungr /= 10;}
		if ( spellknow(spell) >= 40000) {hungr *= 9; hungr /= 10;}
		if ( spellknow(spell) >= 50000) {hungr *= 9; hungr /= 10;}
		if ( spellknow(spell) >= 60000) {hungr *= 9; hungr /= 10;}
		if ( spellknow(spell) >= 70000) {hungr *= 9; hungr /= 10;}

		if (uarmh && uarmh->oartifact == ART_APOTHEOSIS_VEIL) hungr /= 2;

		/* higher XL should help, reducing it to 40% of the normal value at max XL --Amy */
		if (u.ulevel > 10) {
			hungr *= (100 - ((u.ulevel - 10) * 3));
			hungr /= 100;
		}

		if (boost_power_value()) {
			hungr *= (100 - (boost_power_value() * 3) );
			hungr /= 100;
		}

		if (hungr < 1) hungr = 1;
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

		if (!obsidianprotection()) switch (rn2(11)) {
		    case 0: make_sick(Sick ? Sick/2L + 1L : (long)rn1(ACURR(A_CON),20),
				"red spell sickness", TRUE, SICK_NONVOMITABLE);
			    break;
		    case 1: make_blinded(Blinded + lcount, TRUE);
			    break;
		    case 2: if (!Confusion)
				You("suddenly feel %s.", FunnyHallu ? "trippy" : "confused");
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
		    case 7: (void) adjattrib(rn2(A_MAX), -1, FALSE, TRUE);
			    break;
		    case 8: (void) make_hallucinated(HHallucination + lcount, TRUE, 0L);
			    break;
		    case 9: make_feared(HFeared + lcount, TRUE);
			    break;
		    case 10: make_dimmed(HDimmed + lcount, TRUE);
			    break;
		}

	}

	if (SpellColorBrown && !(t_at(u.ux, u.uy)) ) {
		register struct trap *shittrap;
		shittrap = maketrap(u.ux, u.uy, SHIT_TRAP, 0, FALSE);
		if (shittrap && !(shittrap->hiddentrap)) {
			shittrap->tseen = 1;
		}
	}

	if (SpellCooldownBug || u.uprops[SPELL_COOLDOWN_BUG].extrinsic || have_spellcooldownstone() || (ublindf && ublindf->oartifact == ART_ARABELLA_S_SEE_NO_EVIL_MON) ) {
		if (!u.spellcooldown) {
			u.spellcooldown = SpellCooldownXtra ? rnz(100) : rnz(10);
			Your("spellcasting has been put on a timer of %d turns.", u.spellcooldown);
		}
	}

	if (SpellColorPink) {
		pline("%s", fauxmessage());
		u.cnd_plineamount++;
	}

	if (SpellColorViolet) pushplayer(TRUE);

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

	if (isevilvariant && youmonst.data->msound == MS_JAPANESE) {
		if (strstri(spellname(spell), "r") != 0) {

			register int confusedcost;

			pline("You can't spell the letter 'r', and therefore fail to cast the spell correctly.");
			u.cnd_spellfailcount++;
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			if (!rn2(100)) { /* evil patch idea by Amy: failure effect */
				pline("In fact, you cast the spell incorrectly in a way that causes bad stuff to happen...");
				badeffect();
			}
			if (SpellColorSilver) u.seesilverspell = 0;

			if (FailuresAreCostly) confusedcost = energy;
			else confusedcost = ((energy * 50 / ((role_skill == P_SUPREME_MASTER) ? 240 : (role_skill == P_GRAND_MASTER) ? 220 : (role_skill == P_MASTER) ? 200 : (role_skill == P_EXPERT) ? 180 : (role_skill == P_SKILLED) ? 160 : (role_skill == P_BASIC) ? 140 : 120)) + 1);

			u.uen -= confusedcost;

			if (SpellColorOrange) losehp(confusedcost, "casting an orange spell while confused", KILLED_BY);

			flags.botl = 1;

			return (1);
		}
	}

	if ( (confused && spellid(spell) != SPE_CURE_CONFUSION && spellid(spell) != SPE_CURE_RANDOM_STATUS && (confusionchance < rnd(100)) && (!StrongConf_resist || !rn2(3)) && rn2(Conf_resist ? 5 : 10) ) || (rnd(100) > chance)) {

		register int confusedcost;

		if (!issoviet) pline("You fail to cast the spell correctly.");
		else pline("HA HA HA HA HA, tip bloka l'da sdelal vy ne zaklinaniye!");
		u.cnd_spellfailcount++;
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		if (!rn2(100)) { /* evil patch idea by Amy: failure effect */
			pline("In fact, you cast the spell incorrectly in a way that causes bad stuff to happen...");
			badeffect();
		}

		if (SpellColorSilver) u.seesilverspell = 0;

#ifdef ALLEG_FX
                if (iflags.usealleg) alleg_aura(u.ux, u.uy, P_ATTACK_SPELL-1);
#endif

		/* Higher spellcasting skills mean failure takes less mana. --Amy */

		if (FailuresAreCostly) confusedcost = energy;
		else confusedcost = ((energy * 50 / ((role_skill == P_SUPREME_MASTER) ? 240 : (role_skill == P_GRAND_MASTER) ? 220 : (role_skill == P_MASTER) ? 200 : (role_skill == P_EXPERT) ? 180 : (role_skill == P_SKILLED) ? 160 : (role_skill == P_BASIC) ? 140 : 120)) + 1);

		u.uen -= confusedcost;

		if (SpellColorOrange) losehp(confusedcost, "casting an orange spell while confused", KILLED_BY);

		flags.botl = 1;
		return(1);
	}

	if (u.tremblingamount) {
		int tremblechance = (u.tremblingamount * 5 / 2);
		if (rn2(100) < rnd(tremblechance)) {

			int confusedcost;

			You("screw up while casting the spell...");
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			u.cnd_spellfailcount++;

			if (!rn2(10)) {
				pline("In fact, you screwed up so badly that bad stuff happens...");
				badeffect();
			}

			if (SpellColorSilver) u.seesilverspell = 0;

			if (FailuresAreCostly) confusedcost = energy;
			else confusedcost = ((energy * 50 / ((role_skill == P_SUPREME_MASTER) ? 240 : (role_skill == P_GRAND_MASTER) ? 220 : (role_skill == P_MASTER) ? 200 : (role_skill == P_EXPERT) ? 180 : (role_skill == P_SKILLED) ? 160 : (role_skill == P_BASIC) ? 140 : 120)) + 1);

			u.uen -= confusedcost;

			if (SpellColorOrange) losehp(confusedcost, "casting an orange spell while trembling", KILLED_BY);

			flags.botl = 1;
			return(1);
		}

	}

	/* Players could cheat if they had just barely enough mana for casting a spell without the increased drain.
	 * They'd just need to keep trying until the extra mana costs are randomly very low.
	 * Prevent players from abusing this by calculating the extra drain _after_ the other checks. --Amy */
	if (u.uhave.amulet && u.amuletcompletelyimbued && !u.freeplaymode) {
		You_feel("the amulet draining your energy away.");
		energy += rnd(2*energy);
	}

	u.uen -= energy;

	if (SpellColorOrange) losehp(energy, "casting an orange spell with too little health", KILLED_BY);

	if (SpellColorSilver) u.seesilverspell = 1;

	/* And if the amulet drained it below zero, set it to zero and just make the spell fail now. */
	if (u.uhave.amulet && !u.freeplaymode && u.amuletcompletelyimbued && u.uen < 0) {
		pline("You are exhausted, and fail to cast the spell due to the amulet draining all your energy away.");
		u.cnd_spellfailcount++;
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		u.uen = 0;
		if (SpellColorSilver) u.seesilverspell = 0;
		return(1);
	}

	u.cnd_spellcastcount++;

	if (uarmg && uarmg->oartifact == ART_WHOOSHZHOOSH) {
		incr_itimeout(&HFast, rn1(15, 5));
	}

	u.umemorizationturns += spellev(spell);
	if (u.umemorizationturns >= 100) {
		u.umemorizationturns -= 100;
		use_skill(P_MEMORIZATION, 1);
	}

	/* if your INT is low, have a very slight chance of increasing it --Amy */
	if (ABASE(A_INT) < 10) {
		int intellchance = 500;
		switch (ABASE(A_INT)) {
			case 1: intellchance = 300; break;
			case 2: intellchance = 400; break;
			case 3: intellchance = 500; break;
			case 4: intellchance = 600; break;
			case 5: intellchance = 700; break;
			case 6: intellchance = 800; break;
			case 7: intellchance = 900; break;
			case 8: intellchance = 1000; break;
			case 9: intellchance = 1000; break;
			default: intellchance = 1000; break;
		}

		/* make it depend on spell level... */
		intellchance *= 16;
		intellchance /= (8 + spellev(spell));

		if (!rn2(intellchance)) (void) adjattrib(A_INT, 1, FALSE, TRUE);
	}

	if (uarmc && uarmc->oartifact == ART_SOVNGARDE_TUM_DA_SAL && (spellev(spell) > 4) && !rn2(50) ) {
		int sovspheres = rnd(8);
		while (sovspheres > 0) {
			sovspheres--;
			(void) make_helper(PM_FREEZING_SPHERE, u.ux, u.uy); /* stronger than the summoned version created by the freeze sphere --Amy */
		}
		pline("A bunch of freezing spheres is summoned!");
	}

	if (uwep && is_lightsaber(uwep) && (uwep->lamplit || Role_if(PM_SHADOW_JEDI)) ) {
		u.unimanturns++;

		if (spellev(spell) > rn2(8)) {
			mightbooststat(A_INT);
		}

		if (u.unimanturns >= 3) {
			u.unimanturns = 0;
			use_skill(P_NIMAN, 1);
		}

		if (uwep && tech_inuse(T_ENERGY_TRANSFER)) {
			uwep->age += energy;
			if (uwep->otyp == ORANGE_LIGHTSABER) uwep->age += (energy * rnd(2));
			if (uwep->oartifact == ART_DESANN_S_WRATH) uwep->age += (energy * rnd(2));
			pline("Your lightsaber is charged a bit.");
		}

	}
	if (uwep && uwep->oartifact == ART_NEVERMAN) {
		u.unimanturns++;

		if (spellev(spell) > rn2(8)) {
			mightbooststat(A_INT);
		}

		if (u.unimanturns >= 3) {
			u.unimanturns = 0;
			use_skill(P_NIMAN, 1);
		}
	}

	if (Role_if(PM_MAHOU_SHOUJO)) { /* Casting any sort of magic causes all monsters on a level to 
      become alert of your location, due to mahou shoujo always announcing their attacks. */

		wake_nearby();
		verbalize("%s!",spellname(spell) );

	}

	if (uarmh && uarmh->oartifact == ART_ANTON_S_CROWN_OF_ECHOES && !rn2(100)) {
		wake_nearby();
		pline_The("echoes of your spellcasting chant are resounding in the dungeon.");
	}

	if (Race_if(PM_RELEASIER)) {
		verbalize("%s", !rn2(3) ? "Kjear! Hahahahaha!" : !rn2(2) ? "Aa-hu!" : "Hiarwigo!");
	}

	if (SpellColorBrightCyan) {
		u.aggravation = 1;
		u.lamefarmer = 1;
		adjalign(-5);
		reset_rndmonst(NON_PM);
		(void) makemon((struct permonst *)0, 0, 0, MM_ANGRY|MM_FRENZIED);
		u.aggravation = 0;
		u.lamefarmer = 0;

	}

	/* you can't hold perfectly still while performing the casting motion; occasionally scuff engraving --Amy
	 * but less than if you tried to use melee attacks */
	if (!rn2(5)) u_wipe_engr(1);

	flags.botl = 1;
	exercise(A_WIS, TRUE);

	/* Erasmus' gauntlets cause you to cast a random spell on occasion; don't run this code if we're already processing a random spell --Amy
	 * this has to be done after we've established that you successfully cast a spell, so you can't just attempt a 100% fail spell a gazillion times
	 * until you randomly hit the 1% chance of getting a random spell, because if such an exploit exists, some player WILL abuse it... */
	if (uarmg && uarmg->oartifact == ART_ERASMUS__SCRIPTED_GAUNTLET && !rn2(100)) {
		pline_The("magical energy goes out of control...");
		spell_metronome(1, 8);
		return(1);
	}

	/* pseudo is a temporary "false" object containing the spell stats. */
metronomedoit:
	pseudo = (metronomespell ? mksobj(spell, FALSE, 2, FALSE) : mksobj(spellid(spell), FALSE, 2, FALSE) );
	if (!pseudo) {
		pline("The spell failed spontaneously!");
		if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return(1);
	}
	if (pseudo->otyp == GOLD_PIECE) pseudo->otyp = (metronomespell ? spell : spellid(spell)); /* minimalist fix */
	pseudo->blessed = pseudo->cursed = 0;
	pseudo->quan = 20L;			/* do not let useup get it */

	/* WAC -- If skilled enough,  will act like a blessed version */
	if (role_skill >= P_SKILLED && issoviet) { /* made it depend on skill level --Amy */
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
	case SPE_MAGIC_BOLT:
	case SPE_WATER_BOLT:
	case SPE_MULTIBEAM:
	case SPE_CALL_THE_ELEMENTS:
	case SPE_MANA_BOLT:
	case SPE_ULTRA_P:
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
	case SPE_GIANT_FOOT:
	case SPE_BUBBLING_HOLE:
	case SPE_GEYSER:
	case SPE_NERVE_POISON:
	case SPE_BLOOD_STREAM:
	case SPE_SHINING_WAVE:
	case SPE_ARMOR_SMASH:
	case SPE_STRANGLING:
	case SPE_PARTICLE_CANNON:
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
	case SPE_LIGHT_AREA:
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
	case SPE_CONVERGE_BREATH:
	case SPE_ELEMENTAL_BEAM:
	case SPE_NATURE_BEAM:
	case SPE_PETRIFY:
	case SPE_WIND:
	case SPE_FIRE_BOLT:
	case SPE_DEFENSIVE_FIREBALL:
	case SPE_HYPER_BEAM:
	case SPE_PARALYSIS:

		if (pseudo->otyp == SPE_ULTRA_P) {

			if(!Levitation) {
				HLevitation = 1;
				float_up();
				HLevitation = 0;
			}

			incr_itimeout(&HLevitation, rn1(30, 10));
			flags.botl = TRUE;
			spoteffects(FALSE);	/* for sinks */
		}

		if (pseudo->otyp == SPE_DEFENSIVE_FIREBALL) {
			boolean fireballfail = FALSE;
			int i, j, bd = 1;
			struct monst *mtmp;
			for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
				if (!isok(u.ux + i, u.uy + j)) continue;
				if ((mtmp = m_at(u.ux + i, u.uy + j)) != 0) {
					if (!mtmp->mpeaceful && !mtmp->mtame) {
						fireballfail = TRUE;
					}
				}
			}

			if (fireballfail) {
				You("are interrupted!");
				break;
			}
		}

		if (pseudo->otyp == SPE_BATTERING_RAM) {
			if (u.battertimer) {
				You("need to wait %d more turns to ready the ram. Battering ram can be used again at turn %ld.", u.battertimer, (moves + u.battertimer));
				break;
			} else {
				u.battertimer = 2;
			}
		}

		if (pseudo->otyp == SPE_PARTICLE_CANNON) {
			if (u.gaugetimer) {
				You("need to wait %d more turns to refill your gauge. The particle cannon can be used again at turn %ld.", u.gaugetimer, (moves + u.gaugetimer));
				break;
			} else {
				u.gaugetimer = 50;
				nomul(-6, "shooting the particle cannon", FALSE);
			}
		}

		if (pseudo->otyp == SPE_CONVERGE_BREATH) {
			if (u.gaugetimer) {
				You("need to wait %d more turns to refill your gauge. Converge breath can be used again at turn %ld.", u.gaugetimer, (moves + u.gaugetimer));
				break;
			} else {
				u.gaugetimer = 50;
			}
		}

		if (practicantterror && pseudo && pseudo->otyp == SPE_FINGER_OF_DEATH && !u.pract_fodzap) {
			pline("%s thunders: 'Well wait, I'll shut down your evil spellcasting. Also, for actually casting an unforgivable curse, you have to pay 25000 zorkmids. If you keep misbehaving like that I'll show you some unforgivable curses, I tell you...'", noroelaname());
			Muteness += rnz(5000);
			fineforpracticant(25000, 0, 0);
			u.pract_fodzap = TRUE;
		}

		if (pseudo->otyp == SPE_BLOOD_STREAM) {

			if (Upolyd && u.mh < 5) {
				losehp(10000, "forcibly bleeding out", KILLED_BY);
			} else if (!Upolyd && u.uhp < 5) {
				losehp(10000, "forcibly bleeding out", KILLED_BY);
			}
			if (rn2(2)) {
				if (Upolyd) u.mh -= ((u.mh / 5) + 1);
				else u.uhp -= ((u.uhp / 5) + 1);
			} else {
				if (Upolyd) {
					u.mh -= ((u.mhmax / 5) + 1);
					if (u.mh < 0) losehp(10000, "forcibly bleeding out", KILLED_BY);
				} else {
					u.uhp -= ((u.uhpmax / 5) + 1);
					if (u.uhp < 0) losehp(10000, "forcibly bleeding out", KILLED_BY);
				}
			}
		}

		if (!(objects[pseudo->otyp].oc_dir == NODIR)) {
			if (atme) u.dx = u.dy = u.dz = 0;
			else
magicalenergychoice:
			if (!getdir((char *)0)) {
			    /* getdir cancelled, re-use previous direction */

				/* Amy edit: this is absolute bullshit behavior, because it's very easy to mistype. It should
				 * NEVER direct the zap at yourself unless you specifically told it so, but the vanilla behavior
				 * means that knowing finger of death means death to interface screw can happen at any time.
				 * I've made magic resistance work too, see zap.c, but those that don't have magic resistance
				 * shall not be screwed over either. Screw you, programmers. And while you're at it,
				 * read up on game design and specifically interface design! :P --Amy */

				if (yn("Do you really want to input no direction?") == 'y')
					pline_The("magical energy is released!");
				else {
					goto magicalenergychoice;
				}

			    if (u.dx == 0 && u.dy == 0) {
				You("would have hit yourself, but to reduce YASD potential the blast goes in a random direction.");
				u.dz = 0;
				confdir();
			    }
			}
			if(!u.dx && !u.dy && !u.dz) {
			    if ((damage = zapyourself(pseudo, TRUE, FALSE)) != 0) {
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
			pushplayer(TRUE);
			pline("The winds hurt you!");
			losehp(rnd(10), "winds", KILLED_BY);
			if (In_sokoban(&u.uz) && !playercancheatinsoko()) {
				change_luck(-1);
				pline("You cheater!");
				if (evilfriday) u.ugangr++;
			}
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
		if (rn2(5) || !issoviet) pseudo->blessed = 0;
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

		if (!rn2(10)) {
			pline("The spell backlashes!");
			badeffect();
		}

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
		if (rn2(5) || !issoviet) pseudo->blessed = 0;
		(void) peffects(pseudo);
		break;
	case SPE_CURE_BLINDNESS:
		if (HeavyBlind) break;
		if (Blinded > (rn1(Role_if(PM_HEALER) ? 200 : 100, 20))) {
			int effreduction = rnd(Blinded / 2);
			if (effreduction > 0) {
				u.ucreamed -= effreduction;
				if ((int)(u.ucreamed) < 0) u.ucreamed = 0; /* fail safe */
				Blinded -= effreduction;
				Your("blindness counter is reduced.");
			}
			if (!Role_if(PM_HEALER) && !rn2(500)) {
				pline("The spell backlashes!");
				badeffect();
			}
		} else {
			healup(0, 0, FALSE, TRUE);
		}
		break;
	case SPE_AMNESIA:
		You_feel("dizzy!");
		forget(1 + rn2(5), FALSE);
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

	case SPE_ORE_MINING:

		{
			int i, j;
			boolean alreadydone = 0;
			for (i = -1; i <= 1; i++) for(j = -1; j <= 1; j++) {
				if (!isok(u.ux + i, u.uy + j)) continue;
				if (alreadydone) continue;
				if (levl[u.ux + i][u.uy + j].typ == STALACTITE) {
					levl[u.ux + i][u.uy + j].typ = CORR;

					if (!rn2(3)) { /* 80% chance of glass, 20% of precious gems */
						if (rn2(5)) mksobj_at(rnd_class(JADE+1, LUCKSTONE-1), u.ux + i, u.uy + j, TRUE, FALSE, FALSE);
						else mksobj_at(rnd_class(DILITHIUM_CRYSTAL, JADE), u.ux + i, u.uy + j, TRUE, FALSE, FALSE);
						pline("A stalactite turns into gems!");
						alreadydone = TRUE;

						if (practicantterror && !u.pract_oremining) {
							pline("%s booms: 'Didn't you hear? Mining my ore is forbidden! That's 1000 zorkmids, and you still have one minute to get away from my ore, got it?'", noroelaname());
							fineforpracticant(1000, 0, 0);
							u.pract_oremining = TRUE;
						}

						break;
					} else {
						pline("A stalactite shatters!");
						alreadydone = TRUE;
						break;
					}
					newsym(u.ux + i, u.uy + j);

					alreadydone = TRUE; /* why the HELL does the break statement not go out of the loop */
					break; /* only raze one stalactite if there are several --Amy */
				}

			}

		}
manloop:

		if (!(InterfaceScrewed || u.uprops[INTERFACE_SCREW].extrinsic || have_interfacescrewstone())) (void)doredraw();

		break;

	case SPE_BOILER_KABOOM:

		{
			int i, j;
			for (i = -1; i <= 1; i++) for(j = -1; j <= 1; j++) {

				if (!isok(u.ux + i, u.uy + j)) continue;
				if (levl[u.ux + i][u.uy + j].typ == RAINCLOUD) {

					levl[u.ux + i][u.uy + j].typ = CORR;
					blockorunblock_point(u.ux + i,u.uy + j);
					explode(u.ux + i, u.uy + j, ZT_FIRE, rnd(boosted_ulevel(1) * 3), WAND_CLASS, EXPL_FIERY);

				}

			}

		}

		if (!(InterfaceScrewed || u.uprops[INTERFACE_SCREW].extrinsic || have_interfacescrewstone())) (void)doredraw();

		break;

	case SPE_DEFOG:

		{
			boolean defogged = 0;
			int i, j;
			for (i = -1; i <= 1; i++) for(j = -1; j <= 1; j++) {

				if (!isok(u.ux + i, u.uy + j)) continue;
				if (levl[u.ux + i][u.uy + j].typ == NETHERMIST) {

					levl[u.ux + i][u.uy + j].typ = CORR;
					adjalign(-10);
					u.alignlim--;
					defogged = 1;

				}

			}

			if (defogged) pline("It is not foggy any longer.");
			else {
				pline("Nothing happens.");
				if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
					pline("Oh wait, actually something bad happens...");
					badeffect();
				}
			}

		}

		if (!(InterfaceScrewed || u.uprops[INTERFACE_SCREW].extrinsic || have_interfacescrewstone())) (void)doredraw();

		break;

	case SPE_SWAP_POSITION:

		u.swappositioncount = 4;
		pline("The next monster you move into will be displaced.");

		break;

	case SPE_SHUFFLE_MONSTER:

		{
			int hasshuffled = 0;

			register struct monst *nexusmon, *nextmon;

			for(nexusmon = fmon; nexusmon; nexusmon = nextmon) {
			    nextmon = nexusmon->nmon; /* trap might kill mon */
			    if (DEADMONSTER(nexusmon)) continue;
			    if (u.usteed && nexusmon == u.usteed) continue;
	
			    if (!monnear(nexusmon, u.ux, u.uy)) continue;
				if (nexusmon->mtrapped) {
				    /* no longer in previous trap (affects mintrap) */
				    nexusmon->mtrapped = 0;
				    fill_pit(nexusmon->mx, nexusmon->my);
				}

				if (pushmonster(nexusmon)) hasshuffled++;
				if (DEADMONSTER(nexusmon)) continue; /* just in case a trap killed it */
				if (pushmonster(nexusmon)) hasshuffled++;

			}

			if (hasshuffled && !(InterfaceScrewed || u.uprops[INTERFACE_SCREW].extrinsic || have_interfacescrewstone())) doredraw();

			if (!hasshuffled) pline("No monsters were shuffled!");
			else if (hasshuffled <= 2) pline("A monster was shuffled!");
			else pline("Several monsters were shuffled!");

		}

		break;

	case SPE_PET_SYRINGE:

		{

			register struct monst *nexusmon, *nextmon;
			const char *verb;
			int healamount;

			for(nexusmon = fmon; nexusmon; nexusmon = nextmon) {
			    nextmon = nexusmon->nmon; /* trap might kill mon */
			    if (DEADMONSTER(nexusmon)) continue;

			    if (!monnear(nexusmon, u.ux, u.uy)) continue;
			    if (!nexusmon->mtame) continue;

				healamount = d(15, 10 + spell_damage_bonus(SPE_PET_SYRINGE) );
				nexusmon->mhp += healamount;
				percentheal_mon(nexusmon, 1);

				if (nexusmon->mhp > nexusmon->mhpmax) nexusmon->mhp = nexusmon->mhpmax;

				if (nexusmon->bleedout && nexusmon->bleedout <= healamount) {
					nexusmon->bleedout = 0;
					pline("%s's bleeding stops.", Monnam(nexusmon));
				} else if (nexusmon->bleedout) {
					nexusmon->bleedout -= healamount;
					if (nexusmon->bleedout < 0) nexusmon->bleedout = 0; /* should never happen */
					pline("%s's bleeding diminishes.", Monnam(nexusmon));
				}

				abuse_dog(nexusmon);

				pline("%s received the syringe and is healthy again.", Monnam(nexusmon));
				verb = growl_sound(nexusmon);
				pline("%s %s.", Monnam(nexusmon), vtense((char *)0, verb));

			}

		}

		break;

	case SPE_BUC_KNOWLEDGE:

		if (CannotSelectItemsInPrompts) break;
		pline("Choose an item for BUC identification.");
bucchoice:
		otmp = getobj(allnoncount, "know the BUC of");
		if (!otmp) {
			if (yn("Really exit with no object selected?") == 'y')
				pline("You just wasted the opportunity to determine an item's BUC.");
			else goto bucchoice;
			pline("You decide not to determine an item's BUC after all.");
			break;
		}
		if (otmp) {
			int increasechance = 4 + u.bucskill;

			if (!otmp->bknown && ((rnd(increasechance) < 6) || Race_if(PM_KNOWLEDGABLE) ) ) {
				u.bucskill++;
				if (u.bucskill > 250) u.bucskill = 250;
			}

			otmp->bknown = TRUE;
			if (otmp->blessed || otmp->cursed) pline("Your %s flashes %s.", doname(otmp), hcolor(otmp->blessed ? NH_AMBER : NH_BLACK));
		}

		break;

	case SPE_PREACHING:

		You("preach some religious sermon.");
		adjalign(5);

		break;

	case SPE_RESIST_PARALYSIS:

		if(!(HDiscount_action & INTRINSIC)) {
			You("gain paralysis resistance!");
			incr_itimeout(&HDiscount_action, HDiscount_action ? (rnd(10) + spell_damage_bonus(SPE_RESIST_PARALYSIS)) : (rn1(100, 50) + spell_damage_bonus(SPE_RESIST_PARALYSIS)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}

		break;

	case SPE_KEEP_SATIATION:

		if(!(HFull_nutrient & INTRINSIC)) {
			Your("nutrition consumption slows down!");
			incr_itimeout(&HFull_nutrient, HFull_nutrient ? (rnd(10) + spell_damage_bonus(SPE_KEEP_SATIATION)) : (rn1(100, 50) + spell_damage_bonus(SPE_KEEP_SATIATION)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}

		break;

	case SPE_LIGHT:
		if (isok(u.ux, u.uy)) {
			levl[u.ux][u.uy].lit = TRUE;
			pline("A light shines on your location.");
		}
		if (isok(u.ux, u.uy + 1)) {
			levl[u.ux][u.uy + 1].lit = TRUE;
		}
		if (isok(u.ux, u.uy - 1)) {
			levl[u.ux][u.uy - 1].lit = TRUE;
		}
		if (isok(u.ux + 1, u.uy)) {
			levl[u.ux + 1][u.uy].lit = TRUE;
		}
		if (isok(u.ux - 1, u.uy)) {
			levl[u.ux - 1][u.uy].lit = TRUE;
		}
		break;

	case SPE_TECH_BOOST:

		if(!(HTechnicality & INTRINSIC)) {
			Your("techniques become stronger!");
			incr_itimeout(&HTechnicality, HTechnicality ? (rnd(10) + spell_damage_bonus(SPE_TECH_BOOST)) : (rn1(100, 50) + spell_damage_bonus(SPE_TECH_BOOST)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}

		break;

	case SPE_MAGIC_CONTROL:

		if(!(HControlMagic & INTRINSIC)) {
			You("feel more capable of controlling your magic!");
			incr_itimeout(&HControlMagic, HControlMagic ? (rnd(10) + spell_damage_bonus(SPE_MAGIC_CONTROL)) : (rn1(100, 50) + spell_damage_bonus(SPE_MAGIC_CONTROL)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}

		break;

	case SPE_ASTRAL_VIEW:

		if(!(HAstral_vision & INTRINSIC)) {
			You("can see through walls!");
			incr_itimeout(&HAstral_vision, HAstral_vision ? 1 : (5 + spell_damage_bonus(SPE_ASTRAL_VIEW) ) );
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}

		break;

	case SPE_CAROTINE_INJECTION:

		if (u.uhp < 6) {
			You("don't have enough health to cast this!");
			break;
		}
		if (Upolyd && u.mh < 6) {
			You("don't have enough health to cast this!");
			break;
		}

		pline("Ow!");
		losehp(5, "carotine injection", KILLED_BY);

		if(!(HBlind_resistance & INTRINSIC)) {
			You("feel able to see freely!");
			incr_itimeout(&HBlind_resistance, HBlind_resistance ? (rnd(10) + spell_damage_bonus(SPE_CAROTINE_INJECTION)) : (rnd(100) + spell_damage_bonus(SPE_CAROTINE_INJECTION)*20));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}

		break;

	case SPE_DOWNER_TRIP:

		u.uprops[DEAC_FEAR_RES].intrinsic += rnz(250);
		make_feared(HFeared + rnz(250), TRUE);

		if(!(HHalluc_resistance & INTRINSIC)) {
			You("feel like you're on a bad trip that lacks fleecy colors!");
			incr_itimeout(&HHalluc_resistance, HHalluc_resistance ? (rnd(100) + spell_damage_bonus(SPE_DOWNER_TRIP)) : (rnd(200) + spell_damage_bonus(SPE_DOWNER_TRIP)*50));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}

		break;

	case SPE_CONTINGENCY:

		u.contingencyturns = 50 + (spell_damage_bonus(SPE_CONTINGENCY) * 3);
		You("sign up a contract with the reaper.");

		break;

	case SPE_RANDOM_DETECTION:

		switch (rnd(4)) {
			case 1:
				trap_detectX((struct obj *)0);
				break;
			case 2:
				You("fail to detect anything.");
				break;
			case 3:
				object_detect(pseudo, 0, FALSE);
				break;
			case 4:
				monster_detect(pseudo, 0);
				break;
		}

		exercise(A_WIS, TRUE);

		if (!rn2(5)) {
			pline("The spell backlashes!");
			badeffect();
		}

		break;

	case SPE_AULE_SMITHING:

		if (CannotSelectItemsInPrompts) break;
		pline("Choose an item for erosionproofing.");
aulechoice:
		otmp = getobj(allnoncount, "fooproof");
		if (!otmp) {
			if (yn("Really exit with no object selected?") == 'y')
				pline("You just wasted the opportunity to fooproof an item.");
			else goto aulechoice;
			pline("You decide not to erosionproof any item after all.");
			break;
		}
		if (otmp) {
			otmp->oerodeproof = TRUE;
			pline("Success! Your item is erosionproof now.");
			if (otmp && objects[(otmp)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(otmp)) {
				if (!otmp->cursed) bless(otmp);
				else uncurse(otmp, FALSE);
			}
		}

		break;

	case SPE_HORSE_HOP:
		u.horsehopturns = 50 + rnd(50 + (spell_damage_bonus(SPE_HORSE_HOP) * 5) );
		pline("You can jump while riding!");

		break;

	case SPE_LINE_LOSS:

		{

			register struct monst *nexusmon, *nextmon;

			for(nexusmon = fmon; nexusmon; nexusmon = nextmon) {
			    nextmon = nexusmon->nmon; /* trap might kill mon */
			    if (DEADMONSTER(nexusmon)) continue;

			    if (!sensemon(nexusmon) && !canseemon(nexusmon)) continue;

				nexusmon->mhp--;
				if (nexusmon->mhp <= 0) {
					xkilled(nexusmon, 0);
					pline("%s lost the last line and dies!", Monnam(nexusmon));
				}
				else {
					pline("%s loses a line!", Monnam(nexusmon));
					wakeup(nexusmon); /* monster becomes hostile */
				}

			}

		}

		break;

	case SPE_TACTICAL_NUKE:

		if (Upolyd) losehp((u.mhmax / 10) + 1, "tactical nuke", KILLED_BY_AN);
		else losehp((u.uhpmax / 10) + 1, "tactical nuke", KILLED_BY_AN);
		pline("Rrrrroommmmmm - the nuke hits you and all monsters in view.");

		{

			register struct monst *nexusmon, *nextmon;

			for(nexusmon = fmon; nexusmon; nexusmon = nextmon) {
			    nextmon = nexusmon->nmon; /* trap might kill mon */
			    if (DEADMONSTER(nexusmon)) continue;

			    if (!sensemon(nexusmon) && !canseemon(nexusmon)) continue;

				nexusmon->mhp -= ((nexusmon->mhp / 10) + 1);
				if (nexusmon->mhp <= 0) {
					xkilled(nexusmon, 0);
					pline("%s dies to the nuke!", Monnam(nexusmon));
				}
				else {
					pline("%s is hurt by the nuke!", Monnam(nexusmon));
					wakeup(nexusmon); /* monster becomes hostile */
				}

			}

		}

		break;

	case SPE_RAGNAROK:

		if (u.ragnarokspelltimeout) {
			pline("Nothing happens.");
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
			break;
		}
		ragnarok(TRUE);
		if (evilfriday) evilragnarok(TRUE,level_difficulty());
		u.ragnarokspelltimeout += 1000; /* can't use it again for a while */

		break;

	case SPE_IMPACT_GUNFIRE:

		if (u.gaugetimer) {
			You("need to wait %d more turns to refill your gauge. Impact Gunfire can be used again at turn %ld.", u.gaugetimer, (moves + u.gaugetimer));
			break;
		} else {
			u.gaugetimer = 50;
		}

		{
			register struct obj *opbullet;
			int opbonus = 0;
			int opdamage = 0;
			int ctx, cty;
			int i;
			ctx = u.ux, cty = u.uy;

			coord cc;
			struct monst *psychmonst;

			opbullet = carrying(PISTOL_BULLET);
			if (!opbullet) opbullet = carrying(SILVER_PISTOL_BULLET);
			if (!opbullet) opbullet = carrying(LEAD_PISTOL_BULLET);
			if (!opbullet) opbullet = carrying(ANTIMATTER_PISTOL_BULLET);
			if (!opbullet) {
				pline("There are no pistol bullets, and therefore you can't shoot!");
				break;
			}

			opdamage = d(8, 12) + (spell_damage_bonus(SPE_IMPACT_GUNFIRE) * 6);

			if (opbullet) {
				if (opbullet->spe > 0) opbonus = opbullet->spe;

				if (opbullet->quan > 1) {
					opbullet->quan--;
					opbullet->owt = weight(opbullet);
				}
				else useup(opbullet);

			}

			if (opdamage > 1) opdamage = rnd(opdamage);
			opdamage += (opbonus * rnd(25));

		    	if (!getdir((char *)0)) return(0);
			if (!u.dx && !u.dy) {
				You("can't direct that at yourself.");
				break;
			}

			for(i = 0; i < 8; i++) {
				if (!isok(ctx + u.dx, cty + u.dy)) break;
				if (levl[ctx + u.dx][cty + u.dy].typ < POOL) break;

				ctx += u.dx;
				cty += u.dy;

				psychmonst = m_at(ctx, cty);

				if (psychmonst) {
					pline("Pouchschieau! Your projectile blasts %s!", mon_nam(psychmonst));
					if (noncorporeal(psychmonst->data)) { /* ghosts are hard to hit... */
						if (opdamage > 1) opdamage /= 2;
						pline("%s resists the attack!", Monnam(psychmonst));
						/* but isn't immune --Amy */
					}
					psychmonst->mhp -= opdamage;
					if (psychmonst->mhp < 1) {
						pline("%s is blown to bits.", Monnam(psychmonst));
						xkilled(psychmonst,0);
					} else {
						wakeup(psychmonst); /* make them hostile */
						if (psychmonst->mcanmove) {
							psychmonst->mcanmove = 0;
							psychmonst->mfrozen = 2;
							pline("%s is paralyzed.", Monnam(psychmonst));
						}
					}
					break;
				}

			}

		}

		break;

	case SPE_ONE_POINT_SHOOT:

		if (u.gaugetimer) {
			You("need to wait %d more turns to refill your gauge. One Point Shoot can be used again at turn %ld.", u.gaugetimer, (moves + u.gaugetimer));
			break;
		} else {
			u.gaugetimer = 50;
		}

		{
			register struct obj *opbullet;
			int opbonus = 0;
			int opdamage = 0;

			coord cc;
			struct monst *psychmonst;

			opbullet = carrying(PISTOL_BULLET);
			if (!opbullet) opbullet = carrying(SILVER_PISTOL_BULLET);
			if (!opbullet) opbullet = carrying(LEAD_PISTOL_BULLET);
			if (!opbullet) opbullet = carrying(ANTIMATTER_PISTOL_BULLET);
			if (!opbullet) {
				pline("There are no pistol bullets, and therefore you can't shoot!");
				break;
			}

			opdamage = d(6, 12) + (spell_damage_bonus(SPE_ONE_POINT_SHOOT) * 5);

			pline("Select the target monster");
			cc.x = u.ux;
			cc.y = u.uy;
			getpos(&cc, TRUE, "the spot to shoot");
			if (cc.x == -10) break; /* user pressed esc */
			psychmonst = m_at(cc.x, cc.y);

			if (!psychmonst || (!canseemon(psychmonst) && !canspotmon(psychmonst))) {
				You("don't see a monster there!");
				break;
			}

			/* there should be a valid target now, so use up a bullet */
			if (opbullet) {
				if (opbullet->spe > 0) opbonus = opbullet->spe;

				if (opbullet->quan > 1) {
					opbullet->quan--;
					opbullet->owt = weight(opbullet);
				}
				else useup(opbullet);

			}

			if (opdamage > 1) opdamage = rnd(opdamage);
			opdamage += (opbonus * rnd(20));

			if (psychmonst) {

				pline("PEW PEW PEW! %s is shot by your gun!", Monnam(psychmonst));
				if (noncorporeal(psychmonst->data)) { /* ghosts are hard to hit... */
					if (opdamage > 1) opdamage /= 2;
					pline("%s resists the attack!", Monnam(psychmonst));
					/* but isn't immune --Amy */
				}
				psychmonst->mhp -= opdamage;
				if (psychmonst->mhp < 1) {
					pline("%s resonates and breaks up.", Monnam(psychmonst));
					xkilled(psychmonst,0);
				} else wakeup(psychmonst); /* make them hostile */

			}

		}

		break;

	case SPE_RETURN:

		if (u.returntimer) {
			u.returntimer = 0;
			pline_The("air around you gradually loses power.");
			break; /* be lenient, damage Pw only if you're not already returning --Amy */
		}

		u.uenmax -= rnd(3);
		if (u.uenmax < 0) u.uenmax = 0;
		if (u.uen > u.uenmax) u.uen = u.uenmax;

		setupreturn(0);

		break;

	case SPE_GROUND_STOMP:

		if (Confusion) {
			You("are unable to use ground stomp while confused.");
			break;
		}

		You("smash something the ground."); /* This is NOT a misspelling!!! --Amy */

		{

			register struct monst *nexusmon, *nextmon;

			for(nexusmon = fmon; nexusmon; nexusmon = nextmon) {
			    nextmon = nexusmon->nmon; /* trap might kill mon */
			    if (DEADMONSTER(nexusmon)) continue;
			    if (!monnear(nexusmon, u.ux, u.uy)) continue;

			    if (!sensemon(nexusmon) && !canseemon(nexusmon)) continue;

				nexusmon->mhp -= (d(10, 10 + (spell_damage_bonus(SPE_GROUND_STOMP) * 5) ) + rnd(boosted_ulevel(1) * 5));
				if (nexusmon->mhp <= 0) {
					xkilled(nexusmon, 0);
					pline("%s is crushed flat!", Monnam(nexusmon));
				}
				else {
					pline("%s is hit by debris!", Monnam(nexusmon));
					wakeup(nexusmon); /* monster becomes hostile */
				}

			}

		}

		make_confused(HConfusion + rn1(50,75), FALSE);
		set_itimeout(&HeavyConfusion, HConfusion);
		u.uprops[DEAC_CONF_RES].intrinsic += rn1(50,75);

		break;

	case SPE_DIRECTIVE:

		{

			int successrate = 0;

			if (!(PlayerCannotUseSkills) && P_SKILL(P_OCCULT_SPELL) >= P_UNSKILLED) {

				switch (P_SKILL(P_OCCULT_SPELL)) {
					case P_BASIC: successrate = 25; break;
					case P_SKILLED: successrate = 40; break;
					case P_EXPERT: successrate = 50; break;
					case P_MASTER: successrate = 60; break;
					case P_GRAND_MASTER: successrate = 80; break;
					case P_SUPREME_MASTER: successrate = 100; break;
					default: successrate = 10; break;
				}

			}

			if (!successrate) {
				pline("Unfortunately, no one seems to follow any directives you're giving.");
				break;
			}

			if (!PlayerCannotUseSkills && P_SKILL(P_RIDING) >= P_SKILLED && rnd(75) <= successrate) {

			pline("Currently your steed has %d%% chance of being targetted by monsters.", u.steedhitchance);
			if (yn("Change it?") == 'y') {

				int lowerbound, higherbound;
				lowerbound = 25;
				higherbound = 25;

				switch (P_SKILL(P_RIDING)) {
					case P_SKILLED:
						lowerbound = 20;
						higherbound = 33;
						break;
					case P_EXPERT:
						lowerbound = 10;
						higherbound = 50;
						break;
					case P_MASTER:
						lowerbound = 5;
						higherbound = 75;
						break;
					case P_GRAND_MASTER:
						lowerbound = 3;
						higherbound = 90;
						break;
					case P_SUPREME_MASTER:
						lowerbound = 1;
						higherbound = 100;
						break;
					default:
						lowerbound = 25;
						higherbound = 25;
						break;
				}

				pline("You can set the chance to values between %d%% and %d%% (inclusive).", lowerbound, higherbound);
				if (lowerbound <= 1 && yn("Set the chance to 1%%?") == 'y') {
					u.steedhitchance = 1;
					pline("The chance that attacks target your steed is 1%% now.");
				} else if (lowerbound <= 3 && yn("Set the chance to 3%%?") == 'y') {
					u.steedhitchance = 3;
					pline("The chance that attacks target your steed is 3%% now.");
				} else if (lowerbound <= 5 && yn("Set the chance to 5%%?") == 'y') {
					u.steedhitchance = 5;
					pline("The chance that attacks target your steed is 5%% now.");
				} else if (lowerbound <= 10 && yn("Set the chance to 10%%?") == 'y') {
					u.steedhitchance = 10;
					pline("The chance that attacks target your steed is 10%% now.");
				} else if (lowerbound <= 20 && yn("Set the chance to 20%%?") == 'y') {
					u.steedhitchance = 20;
					pline("The chance that attacks target your steed is 20%% now.");
				} else if (yn("Set the chance to 25%%?") == 'y') {
					u.steedhitchance = 25;
					pline("The chance that attacks target your steed is 25%% now.");
				} else if (higherbound >= 33 && yn("Set the chance to 33%%?") == 'y') {
					u.steedhitchance = 33;
					pline("The chance that attacks target your steed is 33%% now.");
				} else if (higherbound >= 50 && yn("Set the chance to 50%%?") == 'y') {
					u.steedhitchance = 50;
					pline("The chance that attacks target your steed is 50%% now.");
				} else if (higherbound >= 75 && yn("Set the chance to 75%%?") == 'y') {
					u.steedhitchance = 75;
					pline("The chance that attacks target your steed is 75%% now.");
				} else if (higherbound >= 90 && yn("Set the chance to 90%%?") == 'y') {
					u.steedhitchance = 90;
					pline("The chance that attacks target your steed is 90%% now.");
				} else if (higherbound >= 100 && yn("Set the chance to 100%%?") == 'y') {
					u.steedhitchance = 100;
					pline("The chance that attacks target your steed is 100%% now.");
				} else pline("The chance that attacks target your steed remains %d%%.", u.steedhitchance);

			}

			}

			if (rnd(20) <= successrate) {

			pline("Currently your pets can%s pick up items.", u.petcollectitems ? "" : "'t");
			if (yn("Change it?") == 'y') {
				if (u.petcollectitems) u.petcollectitems = 0;
				else u.petcollectitems = 1;
				pline("Your pets can%s pick up items now.", u.petcollectitems ? "" : "'t");
			}

			}

			if (rnd(45) <= successrate) {

			pline("Currently your pets can%s attack%s monsters.", (u.petattackenemies > 0) ? "" : "'t", u.petattackenemies == 2 ? " both hostile and peaceful" : u.petattackenemies == 1 ? " only hostile" : u.petattackenemies == -1 ? " or be attacked by" : "");
			if (yn("Change it?") == 'y') {
				pline("You got the following options: make the pet attack everything, make it attack only hostile monsters, or prevent it from attacking anything (with the choice that it may or may not still be attacked by hostiles).");
				if (yn("Do you want your pets to attack everything?") == 'y') {
					u.petattackenemies = 2;
					pline("Your pets can attack all monsters now.");
				} else if (yn("Do you want your pets to only attack hostile creatures?") == 'y') {
					u.petattackenemies = 1;
					pline("Your pets can attack hostile monsters now, but will leave peaceful ones alone.");
				} else if (yn("Do you want your pets to not attack any monsters, but still be attacked by them?") == 'y') {
					u.petattackenemies = 0;
					pline("Your pets can't attack monsters now.");
				} else if (yn("Do you want your pets to not attack any monsters, and also not be attacked by them anymore?") == 'y') {
					u.petattackenemies = -1;
					pline("Your pets can't attack monsters now and also won't be attacked by monsters.");
				}
			}

			}

			if (rnd(75) <= successrate) {

			pline("Currently your pets can%s eat food off the floor.", u.petcaneat ? "" : "'t");
			if (yn("Change it?") == 'y') {
				if (u.petcaneat) u.petcaneat = 0;
				else u.petcaneat = 1;
				pline("Your pets can%s eat food off the floor now.", u.petcaneat ? "" : "'t");
			}

			}

			if (rnd(120) <= successrate) {

			pline("Currently your pets can%s try to follow you.", u.petcanfollow ? "" : "'t");
			if (yn("Change it?") == 'y') {
				if (u.petcanfollow) u.petcanfollow = 0;
				else u.petcanfollow = 1;
				pline("Your pets can%s try to follow you now.", u.petcanfollow ? "" : "'t");
			}

			}

		}

		break;

	case SPE_POWDER_SPRAY:

		You("spray the powder.");

		{

			register struct monst *nexusmon, *nextmon;

			for(nexusmon = fmon; nexusmon; nexusmon = nextmon) {
			    nextmon = nexusmon->nmon; /* trap might kill mon */
			    if (DEADMONSTER(nexusmon)) continue;

			    if (!monnear(nexusmon, u.ux, u.uy)) continue;
			    if (resist(nexusmon, SPBOOK_CLASS, 0, NOTELL)) continue;
			    if (resists_poison(nexusmon) && !player_will_pierce_resistance()) continue;

				nexusmon->mhp -= rn1(10, 6);
				if (!rn2(500) && !resists_poison(nexusmon)) {
					pline("The poison was deadly...");
					nexusmon->mhp = -1;
					xkilled(nexusmon, 0);
				} else if (nexusmon->mhp <= 0) {
					xkilled(nexusmon, 0);
					pline("%s is poisoned to death!", Monnam(nexusmon));
				}
				else {
					pline("%s is poisoned!", Monnam(nexusmon));
					wakeup(nexusmon); /* monster becomes hostile */
				}
			}

		}

		break;

	case SPE_FIREWORKS:

		throwstorm(pseudo, 2 + spell_damage_bonus(SPE_FIREWORKS), 2, 2);

		break;

	case SPE_AIMBOT_LIGHTNING:

		{
			coord cc;
			int dirx, diry;

			pline("Select the target square");
			cc.x = u.ux;
			cc.y = u.uy;
			getpos(&cc, TRUE, "the spot to aim lightning from");
			if (cc.x == -10) break; /* user pressed esc */

			if (!couldsee(cc.x, cc.y)) {
				You("can't see the location, and therefore nothing happens!");
				break;
			}

			dirx = rn2(3) - 1;
			diry = rn2(3) - 1;
			if (dirx == 0 && diry == 0) { /* fail safe; yeah I know this biases it towards diagonal :P --Amy */
				dirx = rn2(2) ? 1 : -1;
				diry = rn2(2) ? 1 : -1;
			}
			if (dirx != 0 || diry != 0) {
				buzz(15, 8 + (spell_damage_bonus(SPE_AIMBOT_LIGHTNING) * rnd(2)), cc.x, cc.y, dirx, diry);
			}

		}

		break;

	case SPE_ENHANCE_BREATH:

		u.breathenhancetimer = 100 + (spell_damage_bonus(SPE_ENHANCE_BREATH) * 10);
		Your("breath is magically enhanced!");

		break;

	case SPE_GOUGE_DICK:

		if (!u.uswallow) {
			pline("This spell has no effect if you're not engulfed.");
			break;
		}
		pline("You ram into %s with your %spenis.", mon_nam(u.ustuck), flags.female ? "nonexistant " : "");
		if (u.ustuck->mpeaceful && !(u.ustuck->mtame)) u.ustuck->mpeaceful = 0; /* monster becomes hostile */
		u.ustuck->mhp -= d(flags.female ? 2 : 5, 5 + spell_damage_bonus(SPE_GOUGE_DICK) + rno(boosted_ulevel(1)));
		u.ustuck->mcanmove = 0;
		u.ustuck->mfrozen = rn1(5,5);
		u.ustuck->mstrategy &= ~STRAT_WAITFORU;

		if (u.ustuck->mhp <= 0) {
			xkilled(u.ustuck, 0);
			pline("The monster that engulfed you is stabbed to death!");
		}
		else pline("%s is severely hurt!", Monnam(u.ustuck));

		break;

	case SPE_BODYFLUID_STRENGTHENING:

		u.bodyfluideffect = 10 + spell_damage_bonus(SPE_BODYFLUID_STRENGTHENING);
		Your("body is covered with protective acid!");

		break;

	case SPE_PURIFICATION:

		{
			int i, j;
			for (i = -1; i <= 1; i++) for(j = -1; j <= 1; j++) {
				if (!isok(u.ux + i, u.uy + j)) continue;
				if (i == 0 && j == 0) continue; /* don't target the square you're on --Amy */
				if (levl[u.ux + i][u.uy + j].typ == STYXRIVER) {
					levl[u.ux + i][u.uy + j].typ = MOAT;
					Norep("A styx river turns into a moat!");
				}
			}
		}

		if (!(InterfaceScrewed || u.uprops[INTERFACE_SCREW].extrinsic || have_interfacescrewstone())) (void)doredraw();

		break;

	case SPE_ADD_SPELL_MEMORY:

		if (spellid(0) == NO_SPELL)  { /* should never happen, but I put it here just in case --Amy */
			You("don't know any spells, and therefore you cannot add spell memory to them either.");
			break;
		}

		pline("Choose a spell to add spell memory.");
addspmagain:
		if (!addsomespellmemory()) {
			if (yn("Really exit with no spell selected?") == 'y')
				pline("You just wasted the opportunity to add memory to a spell.");
			else goto addspmagain;
		}

		break;

	case SPE_NEXUSPORT:

		switch (rnd(7)) {

			case 1:
			case 2:
			case 3:
				pline("You are teleported by nexus forces!");
				teleX();
				break;
			case 4:
			case 5:
				pline("You are pushed around by nexus forces!");
				phase_door(0);
				break;
			case 6:

				if (!playerlevelportdisabled() ) {
					make_stunned(HStun + 2, FALSE); /* to suppress teleport control that you might have */

					if (!u.levelporting) {
						u.levelporting = 1;
						nomul(-2, "being levelported", FALSE); /* because it's not called until you get another turn... */
					}
				}
				break;
			case 7:
				{
					nexus_swap();

					if (!rn2(3)) {

						int reducedstat = rn2(A_MAX);
						if(ABASE(reducedstat) <= ATTRABSMIN(reducedstat)) {
							pline("Your health was damaged!");
							u.uhpmax -= rnd(5);
							if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
							if (u.uhp < 1) {
								u.youaredead = 1;
								killer = "nexus scrambling";
								killer_format = KILLED_BY;
								done(DIED);
								u.youaredead = 0;
							}

						} else {
							ABASE(reducedstat) -= 1;
							AMAX(reducedstat) -= 1;
							flags.botl = 1;
							pline("Your attributes were damaged!");
						}
					}
				}
				break;

		}

		if (!rn2(5)) {
			pline("The spell backfires!");
			badeffect();
		}

		break;

	case SPE_ANTI_TELEPORTATION:

		u.antitelespelltimeout = rnd(100 + (spell_damage_bonus(SPE_ANTI_TELEPORTATION) * 10));
		You("erect an anti-teleportation barrier!");

		break;

	case SPE_FUMBLING:
		if (!Fumbling) pline("You start fumbling.");
		HFumbling = FROMOUTSIDE | rnd(5);
		set_itimeout(&HFumbling, 2);
		u.fumbleduration += rnz(1000);
		break;
	case SPE_REMOVE_BLESSING:
		{

		    if (FunnyHallu)
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

			if (obj->oclass == ARMOR_CLASS && !obj->known && (obj->shirtmessage % 3 == 0) ) {
				obj->known = TRUE;
				break;
			}

		    }
		}

		break;

	case SPE_METAL_GUARD:

		if (u.metalguard) pline("%s", nothing_happens);
		else {
			u.metalguard = TRUE;
			You("activate your metal guard!");
		}

		break;

	case SPE_MAGIC_WHISTLING:

	{
		register struct monst *whismtmp, *whisnextmon;

		You("try to whistle your pets here...");

		for(whismtmp = fmon; whismtmp; whismtmp = whisnextmon) {
		    whisnextmon = whismtmp->nmon; /* trap might kill mon */
		    if (DEADMONSTER(whismtmp)) continue;
		    if (evilfriday && level.flags.noteleport) continue;
		    if (whismtmp == u.usteed) continue;
		    if (whismtmp->mtame) {
			if (whismtmp->mtrapped) {
			    /* no longer in previous trap (affects mintrap) */
			    whismtmp->mtrapped = 0;
			    fill_pit(whismtmp->mx, whismtmp->my);
			}
			mnexto(whismtmp);
			if (mintrap(whismtmp) == 2) change_luck(-1);
		    }
		}
	}
		break;

	case SPE_GAIN_SPACT:

		if (u.uhpmax < 101) {
			pline("You don't have enough health to control the powers of this spell!");
			break;
		}
		if (Upolyd && u.mhmax < 101) {
			pline("You don't have enough health to control the powers of this spell!");
			break;
		}

		u.uhpmax -= rnd(100);
		if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		if (Upolyd) {
			u.mhmax -= rnd(100);
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		}

		{
			int wondertech = rnd(MAXTECH-1);
		    	learntech_or_leveltech(wondertech, FROMOUTSIDE, 1);
			You("learn how to perform a new technique!");

		}

		break;

	case SPE_ATTUNE_MAGIC:

			if (rn2(3)) {
				pline("Your mana increases.");
				u.uenmax++;
			} else switch (rnd(30)) {

				case 1:
					HTeleport_control += 2;
					tele();
					break;
				case 2:
					{

					register struct obj *acqo;

					acqo = mkobj_at(SPBOOK_CLASS, u.ux, u.uy, FALSE, FALSE);
					if (acqo) {
						acqo->bknown = acqo->known = TRUE;
						pline("A book appeared at your %s!", makeplural(body_part(FOOT)));
					} else {
						pline("Nothing happens...");
						if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
							pline("Oh wait, actually something bad happens...");
							badeffect();
						}
					  }
					}
					break;
				case 3:
					(void) monster_detect((struct obj *)0, 0);
					exercise(A_WIS, TRUE);
					break;
				case 4:
					trap_detect((struct obj *)0);
					break;
				case 5:
					object_detect((struct obj *)0, 0, FALSE);
					break;
				case 6:
					{
					boolean havegifts = u.ugifts;

					if (!havegifts) u.ugifts++;

					register struct obj *acqo;

					acqo = mk_artifact((struct obj *)0, !rn2(3) ? A_CHAOTIC : rn2(2) ? A_NEUTRAL : A_LAWFUL, TRUE);
					if (acqo) {
					    dropy(acqo);
						int acqoskill = get_obj_skill(acqo, TRUE);

						if (P_MAX_SKILL(acqoskill) == P_ISRESTRICTED) {
						    unrestrict_weapon_skill(acqoskill);
						} else if (P_MAX_SKILL(acqoskill) == P_UNSKILLED) {
							unrestrict_weapon_skill(acqoskill);
							P_MAX_SKILL(acqoskill) = P_BASIC;
						} else if (rn2(2) && P_MAX_SKILL(acqoskill) == P_BASIC) {
							P_MAX_SKILL(acqoskill) = P_SKILLED;
						} else if (!rn2(4) && P_MAX_SKILL(acqoskill) == P_SKILLED) {
							P_MAX_SKILL(acqoskill) = P_EXPERT;
						} else if (!rn2(10) && P_MAX_SKILL(acqoskill) == P_EXPERT) {
							P_MAX_SKILL(acqoskill) = P_MASTER;
						} else if (!rn2(100) && P_MAX_SKILL(acqoskill) == P_MASTER) {
							P_MAX_SKILL(acqoskill) = P_GRAND_MASTER;
						} else if (!rn2(200) && P_MAX_SKILL(acqoskill) == P_GRAND_MASTER) {
							P_MAX_SKILL(acqoskill) = P_SUPREME_MASTER;
						}
						if (Race_if(PM_RUSMOT)) {
							if (P_MAX_SKILL(acqoskill) == P_ISRESTRICTED) {
							    unrestrict_weapon_skill(acqoskill);
							} else if (P_MAX_SKILL(acqoskill) == P_UNSKILLED) {
								unrestrict_weapon_skill(acqoskill);
								P_MAX_SKILL(acqoskill) = P_BASIC;
							} else if (rn2(2) && P_MAX_SKILL(acqoskill) == P_BASIC) {
								P_MAX_SKILL(acqoskill) = P_SKILLED;
							} else if (!rn2(4) && P_MAX_SKILL(acqoskill) == P_SKILLED) {
								P_MAX_SKILL(acqoskill) = P_EXPERT;
							} else if (!rn2(10) && P_MAX_SKILL(acqoskill) == P_EXPERT) {
								P_MAX_SKILL(acqoskill) = P_MASTER;
							} else if (!rn2(100) && P_MAX_SKILL(acqoskill) == P_MASTER) {
								P_MAX_SKILL(acqoskill) = P_GRAND_MASTER;
							} else if (!rn2(200) && P_MAX_SKILL(acqoskill) == P_GRAND_MASTER) {
								P_MAX_SKILL(acqoskill) = P_SUPREME_MASTER;
							}
						}

					    discover_artifact(acqo->oartifact);

						if (!havegifts) u.ugifts--;
						pline("An artifact appeared beneath you!");

					}	

					else pline("Opportunity knocked, but nobody was home.  Bummer.");

					}

					break;
				case 7:
					pline("The RNG decides to curse-weld an item to you.");
					bad_artifact_xtra();
					break;
				case 8:
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

					}

					break;
				case 9:
					pline("Your body suddenly becomes all stiff!");
					if (isstunfish) nomul(-rnz(15), "paralyzed by a pentagram", TRUE);
					else nomul(-rnd(15), "paralyzed by a pentagram", TRUE);
					break;
				case 10:

					pline("The dungeon is getting more chaotic!");
					{
					int madepoolPEP = 0;
					do_clear_areaX(u.ux, u.uy, 12, do_terrainfloodg, (void *)&madepoolPEP);
					}

					break;
				case 11:
					You_feel("powered up!");
					u.uenmax += rnd(5);
					u.uen = u.uenmax;
					break;
				case 12:
					pline("Suddenly, you gain a new companion!");
					(void) make_familiar((struct obj *)0, u.ux, u.uy, FALSE, FALSE);
					break;
				case 13:
					{

					if (Aggravate_monster) {
						u.aggravation = 1;
						reset_rndmonst(NON_PM);
					}

					    coord dd;
					    coord cc;
					    int cx,cy;
						int i;
						int randsp, randmnst, randmnsx;
						struct permonst *randmonstforspawn;
						int monstercolor;

				      cx = rn2(COLNO);
				      cy = rn2(ROWNO);

					if (!rn2(4)) {

					randsp = rno(14);
					if (!rn2(10)) randsp += rno(2);
					if (!rn2(100)) randsp += rno(5);
					if (!rn2(1000)) randsp += rno(10);
					if (!rn2(10000)) randsp += rno(20);
					randmnst = (rn2(187) + 1);
					randmnsx = (rn2(100) + 1);

					pline("You suddenly feel a surge of tension!");

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

					else if (!rn2(3)) {

					randsp = rno(14);
					if (!rn2(10)) randsp += rno(2);
					if (!rn2(100)) randsp += rno(5);
					if (!rn2(1000)) randsp += rno(10);
					if (!rn2(10000)) randsp += rno(20);
					randmonstforspawn = rndmonst();

					You_feel("the arrival of monsters!");

					for (i = 0; i < randsp; i++) {

						if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

						(void) makemon(randmonstforspawn, cx, cy, MM_ADJACENTOK);
					}

					}

					else if (!rn2(2)) {

					randsp = rno(14);
					if (!rn2(10)) randsp += rno(2);
					if (!rn2(100)) randsp += rno(5);
					if (!rn2(1000)) randsp += rno(10);
					if (!rn2(10000)) randsp += rno(20);
					monstercolor = rnd(15);

					You_feel("a colorful sensation!");

					for (i = 0; i < randsp; i++) {

						if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

						(void) makemon(colormon(monstercolor), cx, cy, MM_ADJACENTOK);
					}

					}

					else {

					randsp = rno(14);
					if (!rn2(10)) randsp += rno(2);
					if (!rn2(100)) randsp += rno(5);
					if (!rn2(1000)) randsp += rno(10);
					if (!rn2(10000)) randsp += rno(20);
					monstercolor = rnd(434);

					You_feel("that a group has arrived!");

					for (i = 0; i < randsp; i++) {

						if (!enexto(&cc, u.ux, u.uy, (struct permonst *)0) ) continue;

						(void) makemon(specialtensmon(monstercolor), cx, cy, MM_ADJACENTOK);
					}

					}

					u.aggravation = 0;

					}
					break;
				case 14:

					if (u.uhunger < 1500) {
						pline("Your %s fills.", body_part(STOMACH));
						u.uhunger = 1500;
						u.uhs = 1; /* NOT_HUNGRY */
						flags.botl = 1;
					} else {
						pline("Nothing happens...");
						if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
							pline("Oh wait, actually something bad happens...");
							badeffect();
						}
					}
					break;
				case 15:
					if (u.ualign.record < -1) {
						adjalign(-(u.ualign.record / 2));
						You_feel("partially absolved.");
					} else {
						u.alignlim++;
						adjalign(10);
						You_feel("appropriately %s.", align_str(u.ualign.type));
					}
					break;
				case 16:
					{

					int nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
					if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
					int blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
					if (!blackngdur ) blackngdur = 500; /* fail safe */

					pline("Your mana increases.");
					u.uenmax++;
					/* nasty trap effect - no extra message because, well, nastiness! --Amy */
					randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), (blackngdur - (monster_difficulty() * 3)));

					}
					break;
				case 17:
					{
					int i = rn2(A_MAX);
					adjattrib(i, 1, 0, TRUE);
					adjattrib(i, 1, 0, TRUE);
					adjattrib(i, 1, 0, TRUE);
					adjattrib(i, 1, 0, TRUE);
					adjattrib(i, 1, 0, TRUE);
					}
					break;
				case 18:
					Your("intrinsics change.");
					intrinsicgainorloss(0);
					break;
				case 19:
					{
					struct obj *pseudogram;
					pseudogram = mksobj(SCR_ITEM_GENOCIDE, FALSE, 2, FALSE);
					if (!pseudogram) {
						pline("Nothing happens...");
						if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
							pline("Oh wait, actually something bad happens...");
							badeffect();
						}
						break;
					}
					if (pseudogram->otyp == GOLD_PIECE) pseudogram->otyp = SCR_ITEM_GENOCIDE;
					(void) seffects(pseudogram);
					obfree(pseudogram, (struct obj *)0);	/* now, get rid of it */

					}

					break;
				case 20:
					doubleskilltraining();
					break;
				case 21:
					if (!(HAggravate_monster & INTRINSIC) && !(HAggravate_monster & TIMEOUT)) {

						int maxtrainingamount = 0;
						int skillnumber = 0;
						int actualskillselection = 0;
						int amountofpossibleskills = 1;
						int i;

						for (i = 0; i < P_NUM_SKILLS; i++) {
							if (P_SKILL(i) != P_ISRESTRICTED) continue;

							if (P_ADVANCE(i) > 0 && P_ADVANCE(i) >= maxtrainingamount) {
								if (P_ADVANCE(i) > maxtrainingamount) {
									amountofpossibleskills = 1;
									skillnumber = i;
									maxtrainingamount = P_ADVANCE(i);
								} else if (!rn2(amountofpossibleskills + 1)) {
									amountofpossibleskills++;
									skillnumber = i;
								} else {
									amountofpossibleskills++;
								}
							}
						}

						if (skillnumber > 0 && maxtrainingamount > 0) {
							unrestrict_weapon_skill(skillnumber);

							register int maxcap = P_BASIC;
							if (!rn2(2)) {
								maxcap = P_SKILLED;
								if (!rn2(2)) {
									maxcap = P_EXPERT;
									if (maxtrainingamount >= 20 && !rn2(2)) {
										maxcap = P_MASTER;
										if (maxtrainingamount >= 160 && !rn2(2)) {
											maxcap = P_GRAND_MASTER;
											if (maxtrainingamount >= 540 && !rn2(2)) {
												maxcap = P_SUPREME_MASTER;
											}
										}
									}
								}
							}

							P_MAX_SKILL(skillnumber) = maxcap;
							pline("You can now learn the %s skill, with a new cap of %s.", wpskillname(skillnumber), maxcap == P_SUPREME_MASTER ? "supreme master" : maxcap == P_GRAND_MASTER ? "grand master" : maxcap == P_MASTER ? "master" : maxcap == P_EXPERT ? "expert" : maxcap == P_SKILLED ? "skilled" : "basic");
						} else {
							pline("Nothing happens...");
							if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
								pline("Oh wait, actually something bad happens...");
								badeffect();
							}
						}

					}

					if (HAggravate_monster & INTRINSIC) {
						HAggravate_monster &= ~INTRINSIC;
						You_feel("more acceptable!");
					}
					if (HAggravate_monster & TIMEOUT) {
						HAggravate_monster &= ~TIMEOUT;
						You_feel("more acceptable!");
					}
					break;
				case 22:
					{
						u.aggravation = 1;
						reset_rndmonst(NON_PM);
						int attempts = 0;
						register struct permonst *ptrZ;
newbossPENT:
					do {

						ptrZ = rndmonst();
						attempts++;
						if (attempts && (attempts % 10000 == 0)) u.mondiffhack++;
						if (!rn2(2000)) reset_rndmonst(NON_PM);

					} while ( (!ptrZ || (ptrZ && !(ptrZ->geno & G_UNIQ))) && attempts < 50000);

					if (ptrZ && ptrZ->geno & G_UNIQ) {
						if (wizard) pline("monster generation: %s", ptrZ->mname);
						(void) makemon(ptrZ, u.ux, u.uy, MM_ANGRY);
					}
					else if (rn2(50)) {
						attempts = 0;
						goto newbossPENT;
					}

					u.mondiffhack = 0;

					if (!rn2(10) ) {
						attempts = 0;
						goto newbossPENT;
					}
					pline("Boss monsters appear from nowhere!");

					}
					u.aggravation = 0;

					break;
				case 23:
					if (!rn2(6400)) {
						ragnarok(TRUE);
						if (evilfriday) evilragnarok(TRUE,level_difficulty());

					}

					u.aggravation = 1;
					u.heavyaggravation = 1;
					DifficultyIncreased += 1;
					HighlevelStatus += 1;
					EntireLevelMode += 1;

					(void) makemon(mkclass(S_NEMESE,0), u.ux, u.uy, MM_ANGRY|MM_FRENZIED);

					u.aggravation = 0;
					u.heavyaggravation = 0;

					break;
				case 24:
					wonderspell(-1, 0);
					break;
				case 25:

					{
					int tryct = 0;
					int x, y;
					boolean canbeinawall = FALSE;
					if (!rn2(Passes_walls ? 5 : 25)) canbeinawall = TRUE;
					register struct trap *ttmp;
					for (tryct = 0; tryct < 2000; tryct++) {
						x = rn1(COLNO-3,2);
						y = rn2(ROWNO);

						if (isok(x, y) && ((levl[x][y].typ > DBWALL) || canbeinawall) && !(t_at(x, y)) ) {
								ttmp = maketrap(x, y, rndtrap(), 0, TRUE);
							if (ttmp) {
								ttmp->tseen = 0;
								ttmp->hiddentrap = 1;
							}
							if (!rn2(5)) break;
						}
					}

					You_feel("in grave danger...");
					}
					break;
				case 26:
					badeffect();
					break;
				case 27:
					if (!uinsymbiosis) {
						getrandomsymbiote(FALSE, FALSE);
						pline("Suddenly you have a symbiote!");
					} else {
						u.usymbiote.mhpmax += rnd(10);
						maybe_evolve_symbiote();
						if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
						flags.botl = TRUE;
						Your("symbiote seems much stronger now.");
					}
					break;
				case 28:
					decontaminate(100);
					You_feel("decontaminated.");
					break;
				case 29:
					pline("Wow!  This makes you feel good!");
					{
					int i, ii, lim;
					i = rn2(A_MAX);
					for (ii = 0; ii < A_MAX; ii++) {
						lim = AMAX(i);
						if (ABASE(i) < lim) {
							ABASE(i) = lim;
							flags.botl = 1;
							break;
						}
						if(++i >= A_MAX) i = 0;
					}

					}
					break;
				case 30:
					boostknownskillcap();
					break;
				default:
					impossible("undefined pentagram effect");
					break;

			}


		if (!rn2(7)) badeffect();

		break;

	case SPE_THRONE_GAMBLE:

	    if (rnd(6) > 4)  {
		switch (rnd(32))  {
		    case 1:
			(void) adjattrib(rn2(A_MAX), -rno(5), FALSE, TRUE);
			losehp(rnd(10), "cursed throne", KILLED_BY_AN);
			break;
		    case 2:
			(void) adjattrib(rn2(A_MAX), 1, FALSE, TRUE);
			break;
		    case 3:
			pline("A%s electric shock shoots through your body!",
			      (Shock_resistance) ? "n" : " massive");
			if (!ShockImmunity) losehp(StrongShock_resistance ? rnd(2) : Shock_resistance ? rnd(6) : rnd(30),
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
			if (uinsymbiosis) {
				u.usymbiote.mhpmax += 4;
				maybe_evolve_symbiote();
				if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
			}
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
			    identify_pack(rn2(5), 0, 0);
			}
			break;
		    case 13:
			Your("mind turns into a pretzel!");
			make_confused(HConfusion + rn1(7,16),FALSE);
			break;
		    case 14:
			You("are granted some new skills!"); /* new effect that unrestricts skills --Amy */
			unrestrictskillchoice();

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
			if (CannotSelectItemsInPrompts) break;
			pline("You may fully identify an object!");
			register struct obj *idobj;

secureidchoice:
			idobj = getobj(allnoncount, "secure identify");

			if (!idobj) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to secure identify your objects.");
				else goto secureidchoice;
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
		    case 21:
			{
				int nastytrapdur = (Role_if(PM_GRADUATE) ? 6 : Role_if(PM_GEEK) ? 12 : 24);
				if (!nastytrapdur) nastytrapdur = 24; /* fail safe */
				int blackngdur = (Role_if(PM_GRADUATE) ? 2000 : Role_if(PM_GEEK) ? 1000 : 500);
				if (!blackngdur ) blackngdur = 500; /* fail safe */
				randomnastytrapeffect(rnz(nastytrapdur * (monster_difficulty() + 1)), (blackngdur - (monster_difficulty() * 3)));
				You_feel("uncomfortable.");
			}
			break;
		    case 22:
			morehungry(500);
			pline("Whoops... suddenly you feel hungry.");
			break;
		    case 23:
			pline("Suddenly you feel a healing touch!");
			reducesanity(100);
			break;
		    case 24:
			poisoned("throne", rn2(6) /* A_STR ... A_CHA*/, "poisoned throne", 30);
			break;
		    case 25:
			{
				int thronegold = rnd(200);
				u.ugold += thronegold;
				pline("Some coins come loose! You pick up %d zorkmids.", thronegold);
			}
			break;
		    case 26:
			{

				if (Aggravate_monster) {
					u.aggravation = 1;
					reset_rndmonst(NON_PM);
				}

				pline("A voice echoes:");
				verbalize("Thou hath been summoned to appear before royalty, %s!", playeraliasname);
				(void) makemon(specialtensmon(rn2(2) ? 105 : 106), u.ux, u.uy, MM_ANGRY); /* M2_LORD, M2_PRINCE */

				u.aggravation = 0;

			}
			break;
		    case 27:
			badeffect();
			break;
		    case 28:
			u.uhp++;
			u.uhpmax++;
			if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
			You_feel("a health boost!");
			break;
		    case 29:
			pline("A pretty ethereal woman appears and offers: 'For only 10000 zorkmids, I will give you a very rare trinket!");
			if (u.ugold < 10000) {
				pline("But you don't have enough money! Frustrated, she places a terrible curse on you and disappears.");
				randomfeminismtrap(rnz( (level_difficulty() + 2) * rnd(50)));
			}
			else {
				char femhandlebuf[BUFSZ];
				getlin ("Do you want to buy her goods? [y/yes/no]",femhandlebuf);
				(void) lcase (femhandlebuf);
				if (!(strcmp (femhandlebuf, "yes")) || !(strcmp (femhandlebuf, "y")) || !(strcmp (femhandlebuf, "ye")) || !(strcmp (femhandlebuf, "ys"))) {
					u.ugold -= 10000;
					register struct obj *acqo;
					acqo = mksobj(makegreatitem(), TRUE, TRUE, FALSE);
					if (acqo) {
						dropy(acqo);
						verbalize("Thanks a lot! You'll find your prize on the ground.");
					} else {
						verbalize("Oh sorry, I must have misplaced it. Here you have your money back. Maybe next time I'll have something for you.");
						u.ugold += 10000;
					}
				} else {
					verbalize("You will regret that decision!");
					randomfeminismtrap(rnz( (level_difficulty() + 2) * rnd(50)));

				}
			}
			break;
		    case 30:
			pline("A shady merchant appears and offers: 'Sale! Sale! I'm selling you this useful item for 2000 zorkmids!");
			if (u.ugold < 2000) {
				pline("But you don't have enough money! The merchant disappears.");
			}
			else {
				char femhandlebuf[BUFSZ];
				getlin ("Do you want to buy his item? [y/yes/no]",femhandlebuf);
				(void) lcase (femhandlebuf);
				if (!(strcmp (femhandlebuf, "yes")) || !(strcmp (femhandlebuf, "y")) || !(strcmp (femhandlebuf, "ye")) || !(strcmp (femhandlebuf, "ys"))) {
					u.ugold -= 2000;
					register struct obj *acqo;
					acqo = mksobj(usefulitem(), TRUE, TRUE, FALSE);
					if (acqo) {
						dropy(acqo);
						verbalize("Thank you! I've dropped the item at your feet.");
					} else {
						verbalize("Nyah-nyah, thanks for the money, sucker!");
					}
				} else {
					verbalize("Are you sure? Well, it's your decision. I'll find someone else to sell it to, then.");
				}
			}
			break;
		    case 31:
			{
				struct obj *stupidstone;
				stupidstone = mksobj_at(rnd_class(RIGHT_MOUSE_BUTTON_STONE,NASTY_STONE), u.ux, u.uy, TRUE, FALSE, FALSE);
				if (stupidstone) {
					stupidstone->quan = 1L;
					stupidstone->owt = weight(stupidstone);
					if (!Blind) stupidstone->dknown = 1;
					if (stupidstone) {
						pline("%s lands in your knapsack!", Doname2(stupidstone));
						(void) pickup_object(stupidstone, 1L, TRUE, TRUE);
					}
				}
			}
			break;
		    case 32:
			(void) mksobj_at(rnd_class(DILITHIUM_CRYSTAL, ROCK), u.ux, u.uy, TRUE, TRUE, FALSE);
			pline("Some stones come loose!");
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

		if (!rn2(7)) badeffect();

		break;

	case SPE_COMBAT_COMMAND:

		verbalize(!rn2(3) ? "Bohhh!" : rn2(2) ? "Wouuh!" : "Bohhohh!");
		if (u.combatcommand) u.combatcommand += rnd(10);
		else u.combatcommand = (rn1(50, 50) + spell_damage_bonus(SPE_COMBAT_COMMAND) * 5 );

		break;

	case SPE_DEFUSING:

		if(!(HDefusing & INTRINSIC)) {
			You("can disarm traps more effectively!");
			incr_itimeout(&HDefusing, HDefusing ? (rnd(10) + spell_damage_bonus(SPE_DEFUSING)) : (rn1(100, 50) + spell_damage_bonus(SPE_DEFUSING)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}

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

		u.magicshield += (u.magicshield ? rnd(10) : 40 + (spell_damage_bonus(SPE_MAGIC_SHIELD) * 10) );
		pline("You activate your magic shield!");

		break;

	case SPE_BERSERK:

		if (u.berserktime) {
			pline("Bad idea - you are already berserk! Things turn into an unrecognizable blur.");
			make_hallucinated(HHallucination + rnd(50) + HStun + HConfusion, FALSE, 0L);
			set_itimeout(&HeavyHallu, HHallucination);
			break;
		}

		u.berserktime = 7 + spell_damage_bonus(SPE_BERSERK);
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

					rainedmon->mhp -= rnd(20 + (spell_damage_bonus(SPE_RAIN_CLOUD) * 3) );
					pline("%s is damaged by the raindrops!", Monnam(rainedmon));
					if (rainedmon->mhp <= 0) {
						pline("%s is killed!", Monnam(rainedmon));
						xkilled(rainedmon, 0);
					}

				}
			}
		    }
		}

		if (!inertiaprotection()) {
			u.uprops[DEAC_FAST].intrinsic += (( rnd(10) + rnd(monster_difficulty() + 1) ) * 3);
			pline(u.inertia ? "You slow down even more due to the rain." : "You slow down greatly due to the rain.");
			u.inertia += (rnd(10) + rnd(monster_difficulty() + 1));
		}

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
					if (rainedmon->mpeaceful && !(rainedmon->mtame)) {
						rainedmon->mpeaceful = 0;
						pline("%s gets angry.", Monnam(rainedmon));
					}
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
			    if (nexusmon == u.usteed) continue;
			    if (resist(nexusmon, SPBOOK_CLASS, 0, NOTELL)) continue;

			    if (!monnear(nexusmon, u.ux, u.uy)) continue;
				if (nexusmon->mtrapped) {
				    /* no longer in previous trap (affects mintrap) */
				    nexusmon->mtrapped = 0;
				    fill_pit(nexusmon->mx, nexusmon->my);
				}
				pline("%s is beamed away!", Monnam(nexusmon));
				wakeup(nexusmon); /* monster becomes hostile */
				rloc(nexusmon, FALSE);
			}
		}

		if (!rn2(10)) {
			register int statloss = rn2(A_MAX);

			ABASE(statloss) -= rnd(2);
			if (ABASE(statloss) < ATTRABSMIN(statloss)) ABASE(statloss) = ATTRABSMIN(statloss);
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
				if (levl[nexusmon->mx][nexusmon->my].typ >= STONE && levl[nexusmon->mx][nexusmon->my].typ <= ROCKWALL) {
					if ((levl[nexusmon->mx][nexusmon->my].wall_info & W_NONDIGGABLE) == 0 && !(*in_rooms(nexusmon->mx,nexusmon->my,SHOPBASE))) {
						levl[nexusmon->mx][nexusmon->my].typ = CORR;
						blockorunblock_point(nexusmon->mx,nexusmon->my);
						if (!(levl[nexusmon->mx][nexusmon->my].wall_info & W_HARDGROWTH)) levl[nexusmon->mx][nexusmon->my].wall_info |= W_EASYGROWTH;
						newsym(nexusmon->mx,nexusmon->my);

					}
					pline("%s wails out in pain!", Monnam(nexusmon));
					nexusmon->mhp -= rnz(nexusmon->mhp / 2);
					if (nexusmon->mhp <= 0) {
						xkilled(nexusmon, 0);
						pline("%s is killed!", Monnam(nexusmon));
					} else wakeup(nexusmon); /* monster becomes hostile */
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
		u.enchantspell = (5 + spell_damage_bonus(SPE_ENCHANT));
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

			    if (frostmon->mpeaceful && !frostmon->mtame) {
				pline("%s gets angry.", Monnam(frostmon) );
				frostmon->mpeaceful = 0; /* monster becomes hostile */
			    }

			    if (!resists_elec(frostmon) && !(dmgtype(frostmon->data, AD_PLYS)) && !resist(frostmon, SPBOOK_CLASS, 0, NOTELL) && !rn2(10)) {

				if (canseemon(frostmon) ) {
					pline("%s is paralyzed! It might be unable to move!", Monnam(frostmon) );
				}
				frostmon->mcanmove = 0;
				frostmon->mfrozen = rnd(5);
				frostmon->mstrategy &= ~STRAT_WAITFORU;

			    }

			    if (!resists_elec(frostmon)) {

				frostmon->mhp -= rnd(10 + (spell_damage_bonus(SPE_THUNDER_WAVE) * 2) );
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
					wakeup(mtmp);
					mtmp->mhp -= rnd(40 + (spell_damage_bonus(SPE_POWER_FAILURE) * 10) );
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
				newsym(u.ux + i,u.uy + j);
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
			    if (rn2(2)) continue;
				frostmon->mfrozen = 0;
				frostmon->msleeping = 0;
				frostmon->masleep = 0;
				frostmon->mcanmove = 1;
				frostmon->mflee = 0;
				frostmon->mcansee = 1;
				frostmon->mblinded = 0;
				frostmon->mstun = 0;
				frostmon->mconf = 0;
				pline("%s is cured.", Monnam(frostmon));
			}

			if (u.usteed && rn2(2)) {
				u.usteed->mfrozen = 0;
				u.usteed->msleeping = 0;
				u.usteed->masleep = 0;
				u.usteed->mcanmove = 1;
				u.usteed->mflee = 0;
				u.usteed->mcansee = 1;
				u.usteed->mblinded = 0;
				u.usteed->mstun = 0;
				u.usteed->mconf = 0;
				pline("%s is cured.", Monnam(u.usteed));
			}

		}

		break;

	case SPE_THORNS:

		pline("You throw up a thorny %s!", body_part(BODY_SKIN));
		u.thornspell = 10 + (spell_damage_bonus(SPE_THORNS) * 2);
		/* casting it repeatedly will not give you a longer duration --Amy */

		break;

	case SPE_MANA_BATTERY:
		{

		    register struct obj *mbobj, *mbobj2;
		    for (mbobj = invent; mbobj; mbobj = mbobj2) {
		      mbobj2 = mbobj->nobj;

			if (mbobj->oclass == WAND_CLASS) {
				if (mbobj->spe > 1) {
					u.uen += (mbobj->spe * 10);
					percentrestoremana(1);
				}
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

			/* make good shit, after all you pay with max health for this spell */
			u.aggravation = 1;
			if (!rn2(3)) u.heavyaggravation = 1;
			if (!rn2(20)) DifficultyIncreased += 1;
			if (!rn2(20)) HighlevelStatus += 1;

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
			    } else if ((rnd(30 - ACURR(A_CHA))) < 4) {
				elemental = tamedog(elemental, (struct obj *) 0, FALSE);
				if (elemental) You("dominate %s!", mon_nam(elemental));
			    }

			}
			u.aggravation = 0;
			u.heavyaggravation = 0;
			if (DifficultyIncreased > 0) DifficultyIncreased--;
			if (HighlevelStatus > 0) HighlevelStatus--;

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
			if (spell_damage_bonus(SPE_HYPERSPACE_SUMMON) > 0) ammount += spell_damage_bonus(SPE_HYPERSPACE_SUMMON);

			/* make good shit, after all you pay with max health and pw for this spell */
			u.aggravation = 1;
			if (!rn2(2)) u.heavyaggravation = 1;
			if (!rn2(7)) DifficultyIncreased += 1;
			if (!rn2(7)) HighlevelStatus += 1;

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
				    } else if ((rnd(30 - ACURR(A_CHA))) < 4) {
					vortex = tamedog(vortex, (struct obj *) 0, FALSE);
					if (vortex) You("dominate %s!", mon_nam(vortex));
				    } else if ((rnd(30 - ACURR(A_CHA))) < 4) {
					vortex = tamedog(vortex, (struct obj *) 0, FALSE);
					if (vortex) You("dominate %s!", mon_nam(vortex));
				    }
	
				}
				ammount--;
			}

			u.aggravation = 0;
			u.heavyaggravation = 0;
			if (DifficultyIncreased > 0) DifficultyIncreased--;
			if (HighlevelStatus > 0) HighlevelStatus--;

		}

		break;

	case SPE_KNOW_ENCHANTMENT:
		{

		    pline("You detect the enchantment status of your inventory!");

		    register struct obj *obj, *obj2;
		    for (obj = invent; obj; obj = obj2) {
		      obj2 = obj->nobj;

			if (obj->shirtmessage % 4 == 0 && !obj->known) {
				obj->known = TRUE;
				break;
			}

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
		youmonst.mappearance = FunnyHallu ? ORANGE : GOLD_PIECE;
		newsym(u.ux,u.uy);

		break;

	case SPE_WHISPERS_FROM_BEYOND:

		ABASE(A_INT) -= rnd(2);
		if (ABASE(A_INT) < ATTRABSMIN(A_INT)) {
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

			ABASE(A_INT) = ATTRABSMIN(A_INT);
			You_feel("like a scarecrow.");
		}
		AMAX(A_INT) = ABASE(A_INT);

		ABASE(A_WIS) -= rnd(2);
		if (ABASE(A_WIS) < ATTRABSMIN(A_WIS)) {

			u.youaredead = 1;
			You("turn into an unthinkable vegetable and die.");
			killer = "being turned into a vegetable";
			killer_format = KILLED_BY;
			done(DIED);
			u.youaredead = 0;

			/* player still alive somehow? well then, you can go on playing */

			ABASE(A_WIS) = ATTRABSMIN(A_WIS);
		}
		AMAX(A_WIS) = ABASE(A_WIS);

		increasesanity(rnd(1000));

		/* It is not a mistake that INT loss kills you twice while WIS loss only does so once. --Amy */

		/* now identify the stuff because otherwise players could just hangup cheat past the bad effects... gah... */
		identify_pack(0, 0, 0);
		identify_pack(0, 0, 0);
		if (rn2(2)) identify_pack(0, 0, 0);
		if (!rn2(5)) identify_pack(0, 0, 0);
whisperchoice:
		if (!CannotSelectItemsInPrompts) {
			otmp = getobj(allnoncount, "secure identify");

			if (!otmp) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to secure identify your objects.");
				else goto whisperchoice;
				pline("A feeling of loss comes over you.");
				break;
			}
			if (otmp) {
				makeknown(otmp->otyp);
				if (otmp->oartifact) discover_artifact((int)otmp->oartifact);
				otmp->known = otmp->dknown = otmp->bknown = otmp->rknown = 1;
				if (otmp->otyp == EGG && otmp->corpsenm != NON_PM)
				learn_egg_type(otmp->corpsenm);
				prinv((char *)0, otmp, 0L);
			}
		}

		/* this spell is so costly to cast and so high level, I decide to give it some additional effect --Amy */
		incr_itimeout(&HKeen_memory, HKeen_memory ? (rnd(50) + spell_damage_bonus(SPE_WHISPERS_FROM_BEYOND)*2) : (rn1(250, 250) + spell_damage_bonus(SPE_WHISPERS_FROM_BEYOND)*20));

		break;

	case SPE_STASIS:
		u.stasistime = rnd(100);
		nomul(-(u.stasistime), "frozen in stasis", FALSE);
		pline("You freeze completely.");

		break;

	case SPE_CRYOGENICS:

		cryogenics();

		break;

	case SPE_TERROR:

		if (!rn2(20)) {

			pline("The spell effect backlashes!");

		    if (!obsidianprotection()) switch (rn2(17)) {
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
			wakeup(fleemon); /* monster becomes hostile */
			if (!resist(fleemon, SPBOOK_CLASS, 0, NOTELL))
				monflee(fleemon, rnd(50), FALSE, FALSE, FALSE);
		}
		You_hear("horrified screaming close by.");
	    }

		break;

	case SPE_SYMHEAL:
		if (uinsymbiosis) {
			int healamount;
			healamount = (rnd(10) + 4 + (spell_damage_bonus(SPE_SYMHEAL) * 2) + rndrnz(boosted_ulevel(1)));
			if (healamount > 1) healamount /= 2;
			Your("symbiote seems healthier!");
			u.usymbiote.mhp += healamount;
			if (u.usymbiote.mhp > u.usymbiote.mhpmax) u.usymbiote.mhp = u.usymbiote.mhpmax;
		}

		break;

	case SPE_PHASE_DOOR:
		phase_door(0);
		if (!rn2(20)) {
			pline("The spell backlashes!");
			badeffect();
		}

		break;

	case SPE_TELEPORT_SELF:
		tele();
		if (!rn2(10)) {
			pline("The spell backlashes!");
			badeffect();
		}

		break;

	case SPE_MEDIUM_HEALING:
	{
		int healamount = d(2, 10) + 5 + (!rn2(3) ? 0 : rndrnz(boosted_ulevel(1)) );
		if (healamount > 1 && !rn2(2)) healamount /= 2;

		healup(healamount, 0, FALSE, FALSE);
		percentheal(rno(2));

		You_feel("healthier!");

		if (u.usteed) {
			u.usteed->mhp += rnd(12);
			percentheal_mon(u.usteed, rno(2));
			if (u.usteed->mhp > u.usteed->mhpmax) u.usteed->mhp = u.usteed->mhpmax;
			pline("%s looks healthier.", Monnam(u.usteed));

		}

	}

		break;

	case SPE_STRONG_HEALING:
	{
		int healamount = d(5, 10) + 10 + (rn2(3) ? 0 : rnz(boosted_ulevel(2)) );
		if (healamount > 1 && !rn2(2)) healamount /= 2;

		healup(healamount, 0, FALSE, FALSE);
		percentheal(rno(4));

		You_feel("much more healthy!");

		if (u.usteed) {
			u.usteed->mhp += d(5, 5);
			percentheal_mon(u.usteed, rno(4));
			if (u.usteed->mhp > u.usteed->mhpmax) u.usteed->mhp = u.usteed->mhpmax;
			pline("%s looks healthier.", Monnam(u.usteed));

		}

	}

		break;

	case SPE_SUPER_HEALING:
	{
		int healamount = d(13, 10) + 13 + (rn2(2) ? 0 : rnz(boosted_ulevel(4)) );
		if (healamount > 1 && !rn2(2)) healamount /= 2;

		healup(healamount, 0, FALSE, FALSE);
		percentheal(rno(7));

		You_feel("healthy again!");

		if (u.usteed) {
			u.usteed->mhp += d(5, 13);
			percentheal_mon(u.usteed, rno(7));
			if (u.usteed->mhp > u.usteed->mhpmax) u.usteed->mhp = u.usteed->mhpmax;
			pline("%s looks healthier.", Monnam(u.usteed));

		}

	}

		break;

	case SPE_HEALING_RAIN:

	{
		struct monst *hrmon;

		int healamount = d(2, 10) + 5 + (!rn2(3) ? 0 : rndrnz(boosted_ulevel(1)) );
		if (healamount > 1 && !rn2(2)) healamount /= 2;

		healup(healamount, 0, FALSE, FALSE);
		percentheal(rno(2));

		You_feel("healthier!");

		if (u.usteed) {
			u.usteed->mhp += rnd(12);
			percentheal_mon(u.usteed, rno(2));
			if (u.usteed->mhp > u.usteed->mhpmax) u.usteed->mhp = u.usteed->mhpmax;
			pline("%s looks healthier.", Monnam(u.usteed));

		}

		for (hrmon = fmon; hrmon; hrmon = hrmon->nmon) {
			if (DEADMONSTER(hrmon)) continue;
			if (distu(hrmon->mx,hrmon->my) > 9) continue;
			if (!hrmon->mtame) continue;

			hrmon->mhp += rnd(12);
			percentheal_mon(hrmon, rno(2));
			if (hrmon->mhp > hrmon->mhpmax) hrmon->mhp = hrmon->mhpmax;
			pline("%s looks healthier.", Monnam(hrmon));

		}

	}

		break;

	case SPE_RELOCATION:
		if (u.uhave.amulet && !u.freeplaymode && u.amuletcompletelyimbued) {
			pline("The amulet prevents you from using that spell.");
			break;
		}
		if (In_endgame(&u.uz)) {
			pline("That sort of magic doesn't work on the Planes.");
			break;
		}

		badeffect();
		badeffect();
		badeffect();
		badeffect();
		badeffect();
		u.uenmax -= 5;
		if (u.uenmax < 0) u.uenmax = 0;
		if (u.uen > u.uenmax) u.uen = u.uenmax;

	      (void) safe_teleds_normalterrain(FALSE);

		break;

	case SPE_BURROW:
		u.burrowed = 100;
		u.utrap = 100;
		u.utraptype = TT_INFLOOR;
		pline("You are burrowed into the floor, and gain armor class.");

		break;

	case SPE_SWITCHEROO:

		{
		int NastinessProblemSave = NastinessProblem;

		pline("Click! The red status light goes out while the green light starts shining brightly!");

		cure_nasty_traps();

		NastinessProblem = NastinessProblemSave;

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

		}

	case SPE_GAIN_CORRUPTION:

		pline("Okay, if that's really what you want... you feel corrupted.");
		if (FunnyHallu) pline("At least this isn't ADOM, where having too many corruptions would instakill you!");

		getnastytrapintrinsic();

		break;

	case SPE_COMMAND_DEMON:

		{
		    int soundresist = 0;
		    int i, j, bd = 1;
		    struct monst *mtmp;

		    for(i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if ((mtmp = m_at(u.ux + i, u.uy + j)) != 0 && (is_demon(mtmp->data))) {

				soundresist = 0;
				switch (monstersoundtype(mtmp)) {
					default: break;
					case MS_FART_NORMAL: soundresist = 2; break;
					case MS_FART_QUIET: soundresist = 2; break;
					case MS_FART_LOUD: soundresist = 3; break;
					case MS_CONVERT: soundresist = 3; break;
					case MS_STENCH: soundresist = 5; break;
					case MS_HCALIEN: soundresist = 10; break;
				}

			    if (!rn2(2) && (!soundresist || (!rn2(soundresist)) ) && !resist(mtmp, SPBOOK_CLASS, 0, NOTELL) && !resist(mtmp, SPBOOK_CLASS, 0, NOTELL)) {
				(void) tamedog(mtmp, (struct obj *) 0, FALSE);
				u.uenmax--;
				if (u.uenmax < 0) u.uenmax = 0;
				if (u.uen > u.uenmax) u.uen = u.uenmax;
				flags.botl = TRUE;
			    }
			    else if (!rn2(15) && !((rnd(30 - ACURR(A_CHA))) < 4) && !mtmp->mfrenzied && !mtmp->mtame) {
				pline("Instead of being tamed, %s enters a state of frenzy!", mon_nam(mtmp));
				mtmp->mpeaceful = 0;
				mtmp->mfrenzied = 1;
			    }
			}
		    }
		}

		if (!rn2(10)) {
			pline("The spell backfires!");
			badeffect();
		}

		break;

	case SPE_SELFDESTRUCT:

		{
		    int i, j, bd = 3;
		    struct monst *mtmp;

			u.youaredead = 1;

			pline("%s used SELFDESTRUCT!", playeraliasname);

		    for(i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if ((mtmp = m_at(u.ux + i, u.uy + j)) != 0) {
				mtmp->mhp -= rnd(u.uhpmax * 5);
				pline("%s is hit by the explosion!", Monnam(mtmp));
				if (mtmp->mhp <= 0) {
					pline("%s is killed!", Monnam(mtmp));
					xkilled(mtmp, 0);
				} else wakeup(mtmp); /* monster becomes hostile */
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
			pline("%s used EXPLOSION!", playeraliasname);

		    for(i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
			if (!isok(u.ux + i, u.uy + j)) continue;
			if ((mtmp = m_at(u.ux + i, u.uy + j)) != 0) {
				mtmp->mhp -= rnd(u.uhpmax * 10);
				pline("%s is hit by the explosion!", Monnam(mtmp));
				if (mtmp->mhp <= 0) {
					pline("%s is killed!", Monnam(mtmp));
					xkilled(mtmp, 0);
				} else wakeup(mtmp); /* monster becomes hostile */
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
		do_earthquake((GushLevel - 1) / 3 + 1);
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
				u.cnd_lycanthropecount++;
			}

		}

		break;

	case SPE_BUC_RANDOMIZATION:

	    {	register struct obj *objC;
		    if (FunnyHallu)
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
			     is_feminismstone(objC) ||
			     (objC->otyp == LEATHER_LEASH && objC->leashmon) || (objC->otyp == ARMORED_LEASH && objC->leashmon) || (objC->otyp == INKA_LEASH && objC->leashmon) || (objC->otyp == ADAMANT_LEASH && objC->leashmon) ) && !stack_too_big(objC) && !rn2(5) ) {
			    	blessorcurse(objC, 2);
			}
		}

		update_inventory();

		break;
	    }

	case SPE_MESSAGE:

		pline("%s", fauxmessage());
		u.cnd_plineamount++;
		if (!rn2(3)) {
			pline("%s", fauxmessage());
			u.cnd_plineamount++;
		}

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
			struct trap *ttmp;

		    int i, j;

		      for (i = -1; i <= 1; i++) for(j = -1; j <= 1; j++) {

				if ((ttmp = t_at(u.ux + i, u.uy + j)) != 0) {
				    if (ttmp->ttyp == MAGIC_PORTAL) continue;
				    if (ttmp->ttyp == ONE_WAY_PORTAL) continue;
					deltrap(ttmp);
				}

			}
		}

		pline("You are hit by the magical reaction from casting this very powerful spell.");
		u.uenmax -= rnd(5);
		badeffect();
		(void) doredraw();

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

		if (ABASE(A_STR) < 6 || ABASE(A_DEX) < 6 || ABASE(A_INT) < 6 || ABASE(A_WIS) < 6 || ABASE(A_CON) < 6 || ABASE(A_CHA) < 6) {
			pline("You don't have the stats required to power this mighty spell!");
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
		if (ABASE(A_STR) < ATTRABSMIN(A_STR)) ABASE(A_STR) = ATTRABSMIN(A_STR);
		AMAX(A_STR) = ABASE(A_STR);

		ABASE(A_DEX) -= rnd(5);
		if (ABASE(A_DEX) < ATTRABSMIN(A_DEX)) ABASE(A_DEX) = ATTRABSMIN(A_DEX);
		AMAX(A_DEX) = ABASE(A_DEX);

		ABASE(A_INT) -= rnd(5);
		if (ABASE(A_INT) < ATTRABSMIN(A_INT)) ABASE(A_INT) = ATTRABSMIN(A_INT);
		AMAX(A_INT) = ABASE(A_INT);

		ABASE(A_WIS) -= rnd(5);
		if (ABASE(A_WIS) < ATTRABSMIN(A_WIS)) ABASE(A_WIS) = ATTRABSMIN(A_WIS);
		AMAX(A_WIS) = ABASE(A_WIS);

		ABASE(A_CON) -= rnd(5);
		if (ABASE(A_CON) < ATTRABSMIN(A_CON)) ABASE(A_CON) = ATTRABSMIN(A_CON);
		AMAX(A_CON) = ABASE(A_CON);

		ABASE(A_CHA) -= rnd(5);
		if (ABASE(A_CHA) < ATTRABSMIN(A_CHA)) ABASE(A_CHA) = ATTRABSMIN(A_CHA);
		AMAX(A_CHA) = ABASE(A_CHA);

		makewish(TRUE);

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

		if (ABASE(A_STR) < 4 || ABASE(A_DEX) < 4 || ABASE(A_INT) < 4 || ABASE(A_WIS) < 4 || ABASE(A_CON) < 4 || ABASE(A_CHA) < 4) {
			pline("You don't have the stats required to power this mighty spell!");
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
		if (ABASE(A_STR) < ATTRABSMIN(A_STR)) ABASE(A_STR) = ATTRABSMIN(A_STR);
		AMAX(A_STR) = ABASE(A_STR);
		}

		if (!rn2(2)) {
		ABASE(A_DEX) -= 1;
		if (ABASE(A_DEX) < ATTRABSMIN(A_DEX)) ABASE(A_DEX) = ATTRABSMIN(A_DEX);
		AMAX(A_DEX) = ABASE(A_DEX);
		}

		if (!rn2(2)) {
		ABASE(A_INT) -= 1;
		if (ABASE(A_INT) < ATTRABSMIN(A_INT)) ABASE(A_INT) = ATTRABSMIN(A_INT);
		AMAX(A_INT) = ABASE(A_INT);
		}

		if (!rn2(2)) {
		ABASE(A_WIS) -= 1;
		if (ABASE(A_WIS) < ATTRABSMIN(A_WIS)) ABASE(A_WIS) = ATTRABSMIN(A_WIS);
		AMAX(A_WIS) = ABASE(A_WIS);
		}

		if (!rn2(2)) {
		ABASE(A_CON) -= 1;
		if (ABASE(A_CON) < ATTRABSMIN(A_CON)) ABASE(A_CON) = ATTRABSMIN(A_CON);
		AMAX(A_CON) = ABASE(A_CON);
		}

		if (!rn2(2)) {
		ABASE(A_CHA) -= 1;
		if (ABASE(A_CHA) < ATTRABSMIN(A_CHA)) ABASE(A_CHA) = ATTRABSMIN(A_CHA);
		AMAX(A_CHA) = ABASE(A_CHA);
		}

		acquireitem();

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

			mtmp->mhp += rnd(50);
			percentheal_mon(mtmp, rno(3));
			if (mtmp->mhp > mtmp->mhpmax) {
			    mtmp->mhp = mtmp->mhpmax;
			}
			if (mtmp->bleedout) {
				mtmp->bleedout -= rnd(50);
				if (mtmp->bleedout < 0) mtmp->bleedout = 0;
			}
			pline("%s is healed.", Monnam(mtmp));

		    }

		    int healamount = (d(6,8) + rndrnz(boosted_ulevel(1)) );
		    if (healamount > 1) healamount /= 2;

		    healup(healamount,0,0,0);
		    percentheal(rno(3));
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

		if ((Sick || Slimed) && !Role_if(PM_HEALER)) {
			if (u.uenmax > 0) {
				u.uenmax--;
				if (u.uen > u.uenmax) u.uen = u.uenmax;
			} else {
				You("don't have enough power for this spell.");
				break;
			}
		}

		if (Sick) You("are no longer ill.");
		if (Slimed) {
		    pline_The("slime disappears!");
		    Slimed = 0;
		 /* flags.botl = 1; -- healup() handles this */
		}
		healup(0, 0, TRUE, FALSE);
		break;
	case SPE_CURE_HALLUCINATION:
		if (HeavyHallu) break;
		if (HHallucination > (rn1(Role_if(PM_HEALER) ? 200 : 100, 20))) {
			int effreduction = rnd(HHallucination / 2);
			if (effreduction > 0) {
				HHallucination -= effreduction;
				Your("hallucination counter is reduced.");
			}
			if (!Role_if(PM_HEALER) && !rn2(500)) {
				pline("The spell backlashes!");
				badeffect();
			}
		} else {
			make_hallucinated(0L,TRUE,0L);
		}
		break;
	case SPE_CURE_CONFUSION:
		if (HeavyConfusion) break;
		if (HConfusion > (rn1(Role_if(PM_HEALER) ? 200 : 100, 20))) {
			int effreduction = rnd(HConfusion / 2);
			if (effreduction > 0) {
				HConfusion -= effreduction;
				Your("confusion counter is reduced.");
			}
			if (!Role_if(PM_HEALER) && !rn2(500)) {
				pline("The spell backlashes!");
				badeffect();
			}
		} else {
			make_confused(0L,TRUE);
		}
		break;
	case SPE_CURE_STUN:
		if (HeavyStunned) break;
		if (HStun > (rn1(Role_if(PM_HEALER) ? 200 : 100, 20))) {
			int effreduction = rnd(HStun / 2);
			if (effreduction > 0) {
				HStun -= effreduction;
				Your("stun counter is reduced.");
			}
			if (!Role_if(PM_HEALER) && !rn2(500)) {
				pline("The spell backlashes!");
				badeffect();
			}
		} else {
			make_stunned(0L,TRUE);
		}
		break;
	case SPE_CURE_DIM:
		if (HeavyDimmed) break;
		if (HDimmed > (rn1(Role_if(PM_HEALER) ? 200 : 100, 20))) {
			int effreduction = rnd(HDimmed / 2);
			if (effreduction > 0) {
				HDimmed -= effreduction;
				Your("dimness counter is reduced.");
			}
			if (!Role_if(PM_HEALER) && !rn2(500)) {
				pline("The spell backlashes!");
				badeffect();
			}
		} else {
			make_dimmed(0L,TRUE);
		}
		break;
	case SPE_CURE_BURN:
		if (HeavyBurned) break;
		if (HBurned > (rn1(Role_if(PM_HEALER) ? 200 : 100, 20))) {
			int effreduction = rnd(HBurned / 2);
			if (effreduction > 0) {
				HBurned -= effreduction;
				Your("burn counter is reduced.");
			}
			if (!Role_if(PM_HEALER) && !rn2(500)) {
				pline("The spell backlashes!");
				badeffect();
			}
		} else {
			make_burned(0L,TRUE);
		}
		break;
	case SPE_CURE_FREEZE:
		if (HeavyFrozen) break;
		if (HFrozen > (rn1(Role_if(PM_HEALER) ? 200 : 100, 20))) {
			int effreduction = rnd(HFrozen / 2);
			if (effreduction > 0) {
				HFrozen -= effreduction;
				Your("freeze counter is reduced.");
			}
			if (!Role_if(PM_HEALER) && !rn2(500)) {
				pline("The spell backlashes!");
				badeffect();
			}
		} else {
			make_frozen(0L,TRUE);
		}
		break;
	case SPE_CURE_NUMBNESS:
		if (HeavyNumbed) break;
		if (HNumbed > (rn1(Role_if(PM_HEALER) ? 200 : 100, 20))) {
			int effreduction = rnd(HNumbed / 2);
			if (effreduction > 0) {
				HNumbed -= effreduction;
				Your("numbness counter is reduced.");
			}
			if (!Role_if(PM_HEALER) && !rn2(500)) {
				pline("The spell backlashes!");
				badeffect();
			}
		} else {
			make_numbed(0L,TRUE);
		}
		break;
	case SPE_CURE_RANDOM_STATUS:
		switch (rnd(10)) {
			case 1:
				if ((Sick || Slimed) && !Role_if(PM_HEALER) && !rn2(5)) {
					if (u.uenmax > 0) {
						u.uenmax--;
						if (u.uen > u.uenmax) u.uen = u.uenmax;
					} else {
						You("don't have enough power for this spell.");
						break;
					}
				}

				if (Sick) You("are no longer ill.");
				if (Slimed) {
				    pline_The("slime disappears!");
				    Slimed = 0;
				}
				healup(0, 0, TRUE, FALSE);
				break;
			case 2:
				if (HeavyHallu) break;
				if (HHallucination > (rn1(Role_if(PM_HEALER) ? 600 : 300, 20))) {
					int effreduction = rnd(HHallucination / 2);
					if (effreduction > 0) {
						HHallucination -= effreduction;
						Your("hallucination counter is reduced.");
					}
				} else {
					make_hallucinated(0L,TRUE,0L);
				}
				break;
			case 3:
				if (HeavyConfusion) break;
				if (HConfusion > (rn1(Role_if(PM_HEALER) ? 600 : 300, 20))) {
					int effreduction = rnd(HConfusion / 2);
					if (effreduction > 0) {
						HConfusion -= effreduction;
						Your("confusion counter is reduced.");
					}
				} else {
					make_confused(0L,TRUE);
				}
				break;
			case 4:
				if (HeavyStunned) break;
				if (HStun > (rn1(Role_if(PM_HEALER) ? 600 : 300, 20))) {
					int effreduction = rnd(HStun / 2);
					if (effreduction > 0) {
						HStun -= effreduction;
						Your("stun counter is reduced.");
					}
				} else {
					make_stunned(0L,TRUE);
				}
				break;
			case 5:
				if (HeavyBurned) break;
				if (HBurned > (rn1(Role_if(PM_HEALER) ? 600 : 300, 20))) {
					int effreduction = rnd(HBurned / 2);
					if (effreduction > 0) {
						HBurned -= effreduction;
						Your("burn counter is reduced.");
					}
				} else {
					make_burned(0L,TRUE);
				}
				break;
			case 6:
				if (HeavyFrozen) break;
				if (HFrozen > (rn1(Role_if(PM_HEALER) ? 600 : 300, 20))) {
					int effreduction = rnd(HFrozen / 2);
					if (effreduction > 0) {
						HFrozen -= effreduction;
						Your("freeze counter is reduced.");
					}
				} else {
					make_frozen(0L,TRUE);
				}
				break;
			case 7:
				if (HeavyNumbed) break;
				if (HNumbed > (rn1(Role_if(PM_HEALER) ? 600 : 300, 20))) {
					int effreduction = rnd(HNumbed / 2);
					if (effreduction > 0) {
						HNumbed -= effreduction;
						Your("numbness counter is reduced.");
					}
				} else {
					make_numbed(0L,TRUE);
				}
				break;
			case 8:
				if (HeavyBlind) break;
				if (Blinded > (rn1(Role_if(PM_HEALER) ? 600 : 300, 20))) {
					int effreduction = rnd(Blinded / 2);
					if (effreduction > 0) {
						u.ucreamed -= effreduction;
						if ((int)(u.ucreamed) < 0) u.ucreamed = 0; /* fail safe */
						Blinded -= effreduction;
						Your("blindness counter is reduced.");
					}
				} else {
						make_blinded(0L,FALSE);
				}
				break;
			case 9:
				if (HeavyFeared) break;
				if (HFeared > (rn1(Role_if(PM_HEALER) ? 600 : 300, 20))) {
					int effreduction = rnd(HFeared / 2);
					if (effreduction > 0) {
						HFeared -= effreduction;
						Your("fear counter is reduced.");
					}
				} else {
					make_feared(0L, TRUE);
				}
				break;
			case 10:
				if (HeavyDimmed) break;
				if (HDimmed > (rn1(Role_if(PM_HEALER) ? 600 : 300, 20))) {
					int effreduction = rnd(HDimmed / 2);
					if (effreduction > 0) {
						HDimmed -= effreduction;
						Your("dimness counter is reduced.");
					}
				} else {
					make_dimmed(0L,TRUE);
				}
				break;
		}
		break;

	case SPE_STINKING_CLOUD:
	      {  coord cc;
		pline("Where do you want to center the cloud?");
		cc.x = u.ux;
		cc.y = u.uy;
		if (getpos(&cc, TRUE, "the desired position") < 0) {
		    pline("%s", Never_mind);
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually I do mind...");
				badeffect();
			}
		    break;
		}
		if (!cansee(cc.x, cc.y) || distu(cc.x, cc.y) >= 32) {
		    You("smell rotten eggs.");
		    break;
		}
		(void) create_gas_cloud(cc.x, cc.y, 2, 5);
		break;
		}
		break;
	case SPE_FIXING:
		You_feel("revitalized.");

		if ((Sick || Slimed) && !Role_if(PM_HEALER)) {
			if (u.uenmax > 0) {
				u.uenmax--;
				if (u.uen > u.uenmax) u.uen = u.uenmax;
			} else {
				You("don't have enough power for this spell.");
				break;
			}
		}

		if (Stoned) {
			fix_petrification();

			if (!rn2(2)) {
				if (u.uhpmax < 2) {
					u.youaredead = 1;
					killer = "dissolving completely";
					killer_format = KILLED_BY;
					done(DIED);
					u.youaredead = 0;
				} else {
					u.uhpmax--;
					if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
					flags.botl = TRUE;
					if (Upolyd) {
						if (u.mhmax > 1) u.mhmax--;
						if (u.mh > u.mhmax) u.mh = u.mhmax;
					}
				}
				Your("health was damaged.");
			} else {
				u.negativeprotection += rnd(3);
				MCReduction += rn1(2500, 2500);
				Your("body was damaged.");
			}
		}

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
		explode(u.ux, u.uy, ZT_SPELL(ZT_FIRE), 3 + (spell_damage_bonus(SPE_FIRE) / 2), SCROLL_CLASS, EXPL_FIERY);

		break;

	case SPE_RUSSIAN_ROULETTE:

		if (rn2(10)) {
			pline("Click!");
			badeffect();

		} else if (rn2(10)) {
			pline("Click! You feel vitalized.");
			u.uhpmax++;
			if (Upolyd) u.mhmax++;
			if (uinsymbiosis) {
				u.usymbiote.mhpmax++;
				maybe_evolve_symbiote();
				if (u.usymbiote.mhpmax > 500) u.usymbiote.mhpmax = 500;
			}

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

		/* bullshit downside by Amy - we don't want petrification to be THAT trivial to cure... */

		if (!rn2(2)) {
			if (u.uhpmax < 2) {
				u.youaredead = 1;
				killer = "dissolving completely";
				killer_format = KILLED_BY;
				done(DIED);
				u.youaredead = 0;
			} else {
				u.uhpmax--;
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				flags.botl = TRUE;
				if (Upolyd) {
					if (u.mhmax > 1) u.mhmax--;
					if (u.mh > u.mhmax) u.mh = u.mhmax;
				}
			}
			Your("health was damaged.");
		} else {
			u.negativeprotection += rnd(3);
			MCReduction += rn1(2500, 2500);
			Your("body was damaged.");
		}

		if (Acid_resistance && (StrongAcid_resistance || AcidImmunity || rn2(10)) ) {
			pline("This tastes %s.", FunnyHallu ? "tangy" : "sour");
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

		if (!obsidianprotection()) switch (rn2(17)) {
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
		else TimeStopped += rnd(3 + spell_damage_bonus(SPE_TIME_STOP) );
		break;
	case SPE_LEVELPORT:
	      if (!playerlevelportdisabled()) {
			level_tele();
			pline("From your strain of casting such a powerful spell, the magical energy backlashes on you.");
			badeffect();
		}
		else pline("Hmm... that level teleport spell didn't do anything.");

		break;
	case SPE_WARPING:
		if (((u.uhave.amulet) && !u.freeplaymode) || CannotTeleport || (u.usteed && mon_has_amulet(u.usteed))) { pline("You shudder for a moment."); break;}

		if (playerlevelportdisabled()) { 
			pline("You're unable to warp!");
			break;
		}

		banishplayer();

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
			register struct trap *ttmp;
			boolean canbeinawall = FALSE;
			if (!rn2(Passes_walls ? 5 : 25)) canbeinawall = TRUE;

		      for (i = -bd; i <= bd; i++) for(j = -bd; j <= bd; j++) {
				if (!isok(u.ux + i, u.uy + j)) continue;
				if ((levl[u.ux + i][u.uy + j].typ <= DBWALL) && !canbeinawall) continue;
				if (t_at(u.ux + i, u.uy + j)) continue;

			      rtrap = rndtrap();

				ttmp = maketrap(u.ux + i, u.uy + j, rtrap, 100, FALSE);
				if (ttmp && !rn2(10)) ttmp->hiddentrap = TRUE;
			}
		}

		if (rn2(10)) {
			makerandomtrap(FALSE);
			if (!rn2(2)) makerandomtrap(FALSE);
			if (!rn2(4)) makerandomtrap(FALSE);
			if (!rn2(8)) makerandomtrap(FALSE);
			if (!rn2(16)) makerandomtrap(FALSE);
			if (!rn2(32)) makerandomtrap(FALSE);
			if (!rn2(64)) makerandomtrap(FALSE);
			if (!rn2(128)) makerandomtrap(FALSE);
			if (!rn2(256)) makerandomtrap(FALSE);
		} else {
			makeinvisotrap();
			if (!rn2(2)) makeinvisotrap();
			if (!rn2(4)) makeinvisotrap();
			if (!rn2(8)) makeinvisotrap();
			if (!rn2(16)) makeinvisotrap();
			if (!rn2(32)) makeinvisotrap();
			if (!rn2(64)) makeinvisotrap();
			if (!rn2(128)) makeinvisotrap();
			if (!rn2(256)) makeinvisotrap();
		}
		badeffect();

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
		if (!Blind && !u.usleep) Your("%s", vision_clears);
		break;

	case SPE_AIR_CURRENT:
		if (FunnyHallu)
			You_hear("air current noises, and a remark by Amy about how sexy they are.");
		else
			You_hear("air current noises.");

		pushplayer(TRUE);
		if (In_sokoban(&u.uz) && !playercancheatinsoko()) {
			change_luck(-1);
			pline("You cheater!");
			if (evilfriday) u.ugangr++;
		}

		break;

	case SPE_DASHING:

dashingchoice:
		if (!getdir((char *)0)) {
			if (yn("Do you really want to input no direction?") == 'y')
				break;
			else {
				goto dashingchoice;
			}
			break;
		}
		if (!u.dx && !u.dy) {
			You("stretch.");
			break;
		}

		spell_dash();

		break;

	case SPE_MELTDOWN:
		You("melt!");
		u.uhpmax -= rnd(3);
		if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		if (u.uhp < 1) {
			u.youaredead = 1;
			killer = "melting down";
			killer_format = KILLED_BY;
			done(DIED);
			u.youaredead = 0;
		}
		if (Upolyd) {
			u.mhmax -= rnd(3);
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		}

		u.uenmax -= rnd(3);
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
		if (rn2(2)) {
			u.uenmax -= rnd(3);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}
		break;

	case SPE_LAVA:
		{
		int madepool = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_lavafloodg, (void *)&madepool);
		if (madepool) pline("Lava pools are created!");
		if (rn2(2)) {
			u.uenmax -= rnd(3);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}

		break;

	case SPE_IRON_PRISON:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_barfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("Iron bars appear from thin air!");
		if (rn2(2)) {
			u.uenmax -= rnd(4);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}

		break;

	case SPE_CLOUDS:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_cloudfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("Clouds everywhere!");
		if (!rn2(3)) {
			u.uenmax -= 1;
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}

		break;

	case SPE_ICE:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_icefloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("The landscape is winterized!");
		if (rn2(2)) {
			u.uenmax -= rnd(2);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}

		break;

	case SPE_LOCKOUT:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_lockfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("The area is walled off!");
		if (rn2(2)) {
			u.uenmax -= rnd(5);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}

		break;

	case SPE_GROW_TREES:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_treefloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("Trees start to grow rapidly!");
		if (rn2(2)) {
			u.uenmax -= rnd(4);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}

		break;

	case SPE_GRAVE:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_gravefloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("Hans Walt erects grave walls!");
		if (rn2(2)) {
			u.uenmax -= rnd(7);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}

		break;

	case SPE_TUNNELS:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_tunnelfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("Lots of tunnels are built!");
		if (rn2(2)) {
			u.uenmax -= rnd(4);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}

		break;

	case SPE_FARMING:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_farmfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("Farmland appears.");
		if (!rn2(3)) {
			u.uenmax -= rnd(3);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}

		break;

	case SPE_MOUNTAINS:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_mountainfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("An underground mountain! Wow!");
		if (rn2(2)) {
			u.uenmax -= rnd(7);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}

		break;

	case SPE_DIVING:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_watertunnelfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("Flooded tunnels have been built!");
		if (rn2(2)) {
			u.uenmax -= rnd(3);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}

		break;

	case SPE_CRYSTALLIZATION:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_crystalwaterfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("There's water on the ceiling!");
		if (rn2(2)) {
			u.uenmax -= rnd(2);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}

		break;

	case SPE_MOORLAND:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_moorfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("The floor becomes swampy.");
		if (!rn2(4)) {
			u.uenmax -= rnd(2);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}

		break;

	case SPE_URINE:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_urinefloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("Mira does her thing!");
		if (!rn2(5)) {
			u.uenmax -= rnd(2);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}

		break;

	case SPE_QUICKSAND:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_shiftingsandfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("Deadly sandholes appear!");
		if (rn2(2)) {
			u.uenmax -= rnd(3);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}

		break;

	case SPE_STYX:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_styxfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("You're in the styx!");
		if (rn2(2)) {
			u.uenmax -= rnd(2);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}

		break;

	case SPE_SNOW:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_snowfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("Winter is coming!");
		if (!rn2(4)) {
			u.uenmax -= rnd(2);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}

		break;

	case SPE_ASH:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_ashfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("Ash terrain appears!");
		if (rn2(2)) {
			u.uenmax -= rnd(3);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}

		break;

	case SPE_SAND:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_sandfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("Sandy deserts!");
		if (rn2(2)) {
			u.uenmax -= rnd(2);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}

		break;

	case SPE_PAVING:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_pavementfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("Whoa, there's a paved road!");
		if (!rn2(5)) {
			u.uenmax -= rnd(2);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}

		break;

	case SPE_HIGHWAY:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_highwayfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("You build a highway to the left!");
		if (rn2(2)) {
			u.uenmax -= rnd(6);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}

		break;

	case SPE_GRASSLAND:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_grassfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("Grass is growing!");
		if (!rn2(4)) {
			u.uenmax -= rnd(2);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}

		break;

	case SPE_NETHER_MIST:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_nethermistfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("Purple mist appears!");
		if (!rn2(4)) {
			u.uenmax -= rnd(2);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}

		break;

	case SPE_STALACTITE:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_stalactitefloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("Stalactites shoot from the ceiling!");
		if (!rn2(3)) {
			u.uenmax -= rnd(2);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}

		break;

	case SPE_CRYPT:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_cryptfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("You entered the crypts!");
		if (!rn2(5)) {
			u.uenmax -= rnd(2);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}

		break;

	case SPE_BUBBLE_BOBBLE:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_bubblefloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("Floating bubbles!");
		if (rn2(2)) {
			u.uenmax -= rnd(2);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}

		break;

	case SPE_RAIN:
		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_raincloudfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("There's rain clouds now!");
		if (rn2(2)) {
			u.uenmax -= rnd(2);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}

		break;

	case SPE_CHAOS_TERRAIN:

		{
		int madepoolQ = 0;
		do_clear_areaX(u.ux, u.uy, 5 + rnd(5), do_terrainfloodg, (void *)&madepoolQ);
		if (madepoolQ) pline("Chaotic terrain is generated!");
		if (rn2(2)) {
			u.uenmax -= rnd(4);
			if (u.uenmax < 0) u.uenmax = 0;
			if (u.uen > u.uenmax) u.uen = u.uenmax;
			pline("Casting this spell is straining for your maximum mana supply.");
		}

		}

		break;

	case SPE_BOMBING:

		{
		int attempts = 0;
		register struct permonst *ptrZ;

			do {
				ptrZ = rndmonst();
				attempts++;
				if (attempts && (attempts % 10000 == 0)) u.mondiffhack++;

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

			u.mondiffhack = 0;

		}

		break;

	case SPE_ANTI_MAGIC_SHELL:
			pline("You produce an anti-magic shell.");
			u.antimagicshell += rnd(100 + (spell_damage_bonus(SPE_ANTI_MAGIC_SHELL) * 5) );
			/* can't cast it with the shell active, so no need to check for that :D --Amy */

		break;

	case SPE_POSSESSION:
		{
			if (CannotSelectItemsInPrompts) break;
			register struct obj *poss;
possessionchoice:
            	poss = getobj((const char *)revivables, "possess");
            	if (!poss) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to possess a corpse.");
				else goto possessionchoice;
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
		} while(( (notake(&mons[u.wormpolymorph]) && rn2(4) ) || ((mons[u.wormpolymorph].mlet == S_BAT) && rn2(2)) || ((mons[u.wormpolymorph].mlet == S_EYE) && rn2(2) ) || ((mons[u.wormpolymorph].mmove == 1) && rn2(4) ) || ((mons[u.wormpolymorph].mmove == 2) && rn2(3) ) || ((mons[u.wormpolymorph].mmove == 3) && rn2(2) ) || ((mons[u.wormpolymorph].mmove == 4) && !rn2(3) ) || ( (mons[u.wormpolymorph].mlevel < 10) && ((mons[u.wormpolymorph].mlevel + 1) < rnd(u.ulevel)) ) || (!haseyes(&mons[u.wormpolymorph]) && rn2(2) ) || ( is_nonmoving(&mons[u.wormpolymorph]) && rn2(5) ) || ( is_eel(&mons[u.wormpolymorph]) && rn2(5) ) || ( is_nonmoving(&mons[u.wormpolymorph]) && rn2(20) ) || (is_jonadabmonster(&mons[u.wormpolymorph]) && rn2(20)) || ( uncommon2(&mons[u.wormpolymorph]) && !rn2(4) ) || ( uncommon3(&mons[u.wormpolymorph]) && !rn2(3) ) || ( uncommon5(&mons[u.wormpolymorph]) && !rn2(2) ) || ( uncommon7(&mons[u.wormpolymorph]) && rn2(3) ) || ( uncommon10(&mons[u.wormpolymorph]) && rn2(5) ) || ( is_eel(&mons[u.wormpolymorph]) && rn2(20) ) ) );

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

			if (golemfuel->lamplit) { /* we don't want timer-related segfault panics --Amy */
				pline("You need a torch that isn't lit! Turn off all lit torches first!");
				break;
			}

			if (golemfuel) {
				if (golemfuel->quan > 1) {
					golemfuel->quan--;
					golemfuel->owt = weight(golemfuel);
				}
				else useup(golemfuel);
			}

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

totemsummonchoice:
			if (CannotSelectItemsInPrompts) break;
            	poss = getobj((const char *)revivables, "revive");
            	if (!poss) {
				if (yn("Really exit with no object selected?") == 'y')
					pline("You just wasted the opportunity to summon from a totem.");
				else goto totemsummonchoice;
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
			    else if (((rnd(30 - ACURR(A_CHA))) < 4) && !resist(posmon, SPBOOK_CLASS, 0, NOTELL))
				(void) tamedog(posmon, (struct obj *) 0, FALSE);
			    else if (((rnd(30 - ACURR(A_CHA))) < 4) && !resist(posmon, SPBOOK_CLASS, 0, NOTELL))
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

		u.sterilized = 10 + (spell_damage_bonus(SPE_STERILIZE) * 4);

		break;

	case SPE_DISRUPTION_SHIELD:

		You("activate your mana shield.");

		u.disruptionshield = 30 + (spell_damage_bonus(SPE_DISRUPTION_SHIELD) * 7);

		break;

	case SPE_HOLY_SHIELD:

		You("activate your holy shield.");

		u.holyshield = 20 + (spell_damage_bonus(SPE_HOLY_SHIELD) * 3);

		break;

	case SPE_AVALANCHE:
		{
			register struct trap *avaltrap;
			avaltrap = maketrap(u.ux, u.uy, COLLAPSE_TRAP, 0, FALSE);
			if (!avaltrap) pline("For some reason, the avalanche does not go off!");

			if (avaltrap && avaltrap->ttyp == COLLAPSE_TRAP) {
				dotrap(avaltrap, 0);

				pline("You are severely hurt and unable to move due to being buried!");

				u.uhpmax -= rnd(5);
				if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
				if (u.uhp < 1) {
					u.youaredead = 1;
					killer = "an avalanche";
					killer_format = KILLED_BY;
					done(DIED);
					u.youaredead = 0;
				}
				if (Upolyd) {
					u.mhmax -= rnd(5);
					if (u.mh > u.mhmax) u.mh = u.mhmax;
					if (u.mh < 1) {
						u.youaredead = 1;
						killer = "an avalanche";
						killer_format = KILLED_BY;
						done(DIED);
						u.youaredead = 0;
					}
				}
				u.uenmax -= rnd(5);
				if (u.uenmax < 0) u.uenmax = 0;
				if (u.uen > u.uenmax) u.uen = u.uenmax;

				if (isstunfish) {
					if (StrongFree_action) {
					    nomul(-(rnz(3)), "buried by an avalanche", TRUE);
						nomovemsg = "You finally dig yourself out of the debris.";
					} else if (Free_action) {
					    nomul(-(rnz(5)), "buried by an avalanche", TRUE);
						nomovemsg = "You finally dig yourself out of the debris.";
					} else {
					    nomul(-(rnz(10)), "buried by an avalanche", TRUE);
						nomovemsg = "You finally dig yourself out of the debris.";
					}
				} else {
					if (StrongFree_action) {
					    nomul(-(rnd(3)), "buried by an avalanche", TRUE);
						nomovemsg = "You finally dig yourself out of the debris.";
					} else if (Free_action) {
					    nomul(-(rnd(5)), "buried by an avalanche", TRUE);
						nomovemsg = "You finally dig yourself out of the debris.";
					} else {
					    nomul(-(rnd(10)), "buried by an avalanche", TRUE);
						nomovemsg = "You finally dig yourself out of the debris.";
					}
				}
			}

		}

		break;

	case SPE_DEMEMORIZE:

		if (spellid(0) == NO_SPELL)  { /* should never happen, but I put it here just in case --Amy */
			You("don't know any spells, and therefore you cannot dememorize them either.");
			break;
		}

		pline("Choose a spell to dememorize.");
dememostart:
		if (!dememorizespell()) {
			if (yn("Really exit with no spell selected?") == 'y')
				pline("You just wasted the opportunity to dememorize a spell.");
			else goto dememostart;
		}

		break;

	case SPE_INERTIA_CONTROL:

		if (spellid(0) == NO_SPELL)  {
			You("don't know any spells, and therefore inertia control fails.");
			break;
		}

		pline("Choose a spell to control.");
controlagain:
		if (!inertiacontrolspell(50)) {
			if (yn("Really exit with no spell selected?") == 'y')
				pline("You just wasted the opportunity to control a spell.");
			else goto controlagain;
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

			if ((level.flags.noteleport || Race_if(PM_STABILISATOR) || u.antitelespelltimeout) && !Race_if(PM_RODNEYAN) ) {
				pline("A mysterious force prevents you from teleporting!");
				break;
			}

			if ( ( (u.uhave.amulet && !u.freeplaymode && (u.amuletcompletelyimbued || !rn2(3))) || CannotTeleport || On_W_tower_level(&u.uz) || (u.usteed && mon_has_amulet(u.usteed)) ) ) {
				You_feel("disoriented for a moment.");
				break;
			}

			for(nexusmon = fmon; nexusmon; nexusmon = nexusmon->nmon) {
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
		u.uhpmax -= rnd(2);
		if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		if (u.uhp < 1) {
			u.youaredead = 1;
			killer = "invoking geolysis with too little health";
			killer_format = KILLED_BY;
			done(DIED);
			u.youaredead = 0;
		}
		if (Upolyd) {
			u.mhmax -= rnd(2);
			if (u.mh > u.mhmax) u.mh = u.mhmax;
		}

		/* re-casting it will restart the countdown rather than add to the duration --Amy */
		u.geolysis = 15 + (spell_damage_bonus(SPE_GEOLYSIS) * 5);

		break;

	case SPE_DRIPPING_TREAD:

		pline("You sacrifice some of your %s and start dripping elements.", body_part(BLOOD));
		u.uhpmax -= rnd(3);
		if (u.uhp > u.uhpmax) u.uhp = u.uhpmax;
		if (u.uhp < 1) {
			u.youaredead = 1;
			killer = "dripping the elements with too little health";
			killer_format = KILLED_BY;
			done(DIED);
			u.youaredead = 0;
		}
		if (Upolyd) {
			u.mhmax -= rnd(3);
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
		u.drippingtread = 20 + (spell_damage_bonus(SPE_DRIPPING_TREAD) * 3);

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

			if (!rn2(StrongPoison_resistance ? 10 : Poison_resistance ? 5 : 3)) {
				int typ = rn2(A_MAX);
				poisontell(typ);
				(void) adjattrib(typ, Poison_resistance ? -1 : -rno(5), TRUE, TRUE);
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
		explode(u.ux, u.uy, ZT_SPELL(ZT_POISON_GAS), 24 + (spell_damage_bonus(SPE_STEAM_VENOM) * 2), SCROLL_CLASS, EXPL_NOXIOUS);

		break;

	case SPE_CREATE_FAMILIAR:
		u.lamefarmer = TRUE;
		if (!rn2(5)) (void) make_familiar((struct obj *)0, u.ux, u.uy, FALSE, FALSE);
		else if (!rn2(2)) {
			pline("The summoned monster does not seem to be friendly!");
			(void) makemon((struct permonst *)0, u.ux, u.uy, MM_NOSPECIALS);
		} else if (rn2(4)) {
			pline("The spell fizzled out!");
		} else {
			pline("The spell backfired!");
			badeffect();
		}
		u.ublesscnt += 50;
		adjalign(-3);
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

	case SPE_FORCIBLE_MOVE:

		if (!jump(-5)) {
			pline("%s", nothing_happens);
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}

		break;

	case SPE_JUMPING:
		if (!jump(max(role_skill,1))) {
			pline("%s", nothing_happens);
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;

	case SPE_LOOT_IMPROVEMENT:
		if(!(HMagicFindBonus & INTRINSIC)) {
			You_feel("able to find magical items!");
			incr_itimeout(&HMagicFindBonus, HMagicFindBonus ? rnd(5) : (10 + spell_damage_bonus(SPE_LOOT_IMPROVEMENT)) );
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;

	case SPE_MYSTERY_PROTECTION:
		if(!(HMysteryResist & INTRINSIC)) {
			You_feel("protected from mysterious magic.");
			incr_itimeout(&HMysteryResist, HMysteryResist ? (rnd(3) + spell_damage_bonus(SPE_MYSTERY_PROTECTION)) : (rn1(33, 15) + spell_damage_bonus(SPE_MYSTERY_PROTECTION)*3));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;

	case SPE_RESIST_POISON:
		if(!(HPoison_resistance & INTRINSIC)) {
			You_feel("healthy ..... for the moment at least.");
			incr_itimeout(&HPoison_resistance, HPoison_resistance ? (rnd(10) + spell_damage_bonus(SPE_RESIST_POISON)) : (rn1(100, 50) + spell_damage_bonus(SPE_RESIST_POISON)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
	case SPE_ANTI_DISINTEGRATION:
		if(!(HDisint_resistance & INTRINSIC)) {
			You_feel("quite firm for a while.");
			incr_itimeout(&HDisint_resistance, HDisint_resistance ? (rnd(10) + spell_damage_bonus(SPE_ANTI_DISINTEGRATION)) : (rn1(100, 50) + spell_damage_bonus(SPE_ANTI_DISINTEGRATION)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
	case SPE_MAGICTORCH:
		if(!(HSight_bonus & INTRINSIC)) {
			You("can see in the dark!");
			incr_itimeout(&HSight_bonus, rn1(20, 10) + spell_damage_bonus(SPE_MAGICTORCH)*20);
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
	case SPE_DISPLACEMENT:
		if(!(HDisplaced & INTRINSIC)) {
			pline("Your image is displaced!");
			incr_itimeout(&HDisplaced, HDisplaced ? (rnd(5) + spell_damage_bonus(SPE_DISPLACEMENT)) : (rn1(200, 100) + spell_damage_bonus(SPE_DISPLACEMENT)*20));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
	case SPE_TRUE_SIGHT:
		if(!(HSee_invisible & INTRINSIC)) {
			pline("You can see invisible things!");
			incr_itimeout(&HSee_invisible, HSee_invisible ? (rnd(5) + spell_damage_bonus(SPE_TRUE_SIGHT)) : (rn1(50, 25) + spell_damage_bonus(SPE_TRUE_SIGHT)*5));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
	case SPE_CONFLICT:
		if(!(HConflict & INTRINSIC)) {
			pline("You start generating conflict!");
			incr_itimeout(&HConflict, HConflict ? (rnd(5) + spell_damage_bonus(SPE_CONFLICT)) : (rn1(20, 10) + spell_damage_bonus(SPE_CONFLICT)*3));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
	case SPE_ESP:
		if(!(HTelepat & INTRINSIC)) {
			You_feel("a strange mental acuity.");
			incr_itimeout(&HTelepat, HTelepat ? (rnd(5) + spell_damage_bonus(SPE_ESP)) : (rn1(100, 50) + spell_damage_bonus(SPE_ESP)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
	case SPE_RADAR:
		if(!(HWarning & INTRINSIC)) {
			pline("You turn on your radar.");
			incr_itimeout(&HWarning, HWarning ? (rnd(5) + spell_damage_bonus(SPE_RADAR)) : (rn1(100, 50) + spell_damage_bonus(SPE_RADAR)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
	case SPE_REGENERATION:
		if(!(HRegeneration & INTRINSIC)) {
			pline("You direct your internal energy to closing your wounds.");
			incr_itimeout(&HRegeneration, HRegeneration ? (rnd(5) + spell_damage_bonus(SPE_REGENERATION)) : (rn1(100, 50) + spell_damage_bonus(SPE_REGENERATION)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
	case SPE_SEARCHING:
		if(!(HSearching & INTRINSIC)) {
			pline("You start searching.");
			incr_itimeout(&HSearching, HSearching ? (rnd(5) + spell_damage_bonus(SPE_SEARCHING)) : (rn1(100, 50) + spell_damage_bonus(SPE_SEARCHING)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
	case SPE_FREE_ACTION:
		if(!(HFree_action & INTRINSIC)) {
			pline("You are resistant to paralysis.");
			incr_itimeout(&HFree_action, HFree_action ? (rnd(5) + spell_damage_bonus(SPE_FREE_ACTION)) : (rn1(100, 50) + spell_damage_bonus(SPE_FREE_ACTION)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
	case SPE_STEALTH:
		if(!(HStealth & INTRINSIC)) {
			pline("You start moving silently.");
			incr_itimeout(&HStealth, HStealth ? (rnd(5) + spell_damage_bonus(SPE_STEALTH)) : (rn1(100, 50) + spell_damage_bonus(SPE_STEALTH)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
	case SPE_INFRAVISION:
		if(!(HInfravision & INTRINSIC)) {
			pline("Your %s are suddenly very sensitive!", makeplural(body_part(EYE)));
			incr_itimeout(&HInfravision, HInfravision ? (rnd(5) + spell_damage_bonus(SPE_INFRAVISION)) : (rn1(100, 50) + spell_damage_bonus(SPE_INFRAVISION)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
	case SPE_COAGULATION:
		if(!(HDiminishedBleeding & INTRINSIC)) {
			pline("Your %s is boiling!", body_part(BLOOD));
			incr_itimeout(&HDiminishedBleeding, HDiminishedBleeding ? (rnd(5) + spell_damage_bonus(SPE_COAGULATION)) : (rn1(100, 50) + spell_damage_bonus(SPE_COAGULATION)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
	case SPE_CURE_PARALYSIS:
		if (multi < 0) {
			multi = -1;
			TimeStopped = 0; /* filthy exploit exploiter */
			You("magically regain consciousness!");
		} else You("weren't paralyzed to begin with, so there was nothing to cure.");
		break;
	case SPE_SMELL_MONSTER:
		if(!(HScentView & INTRINSIC)) {
			pline("Your %s is suddenly very sensitive!", body_part(NOSE));
			incr_itimeout(&HScentView, HScentView ? (rnd(5) + spell_damage_bonus(SPE_SMELL_MONSTER)) : (rn1(100, 50) + spell_damage_bonus(SPE_SMELL_MONSTER)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
	case SPE_ECHOLOCATION:
		pline("Your %s are suddenly very sensitive!", body_part(EARS));
		if (u.echolocationspell) u.echolocationspell += (rnd(5) + spell_damage_bonus(SPE_ECHOLOCATION));
		else u.echolocationspell += (rn1(100, 50) + spell_damage_bonus(SPE_ECHOLOCATION)*10);
		break;
	case SPE_BOTOX_RESIST:
		if(!(HSick_resistance & INTRINSIC)) {
			You_feel("resistant to sickness.");
			incr_itimeout(&HSick_resistance, HSick_resistance ? (rnd(10) + spell_damage_bonus(SPE_BOTOX_RESIST)) : (rn1(100, 50) + spell_damage_bonus(SPE_BOTOX_RESIST)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
	case SPE_DRAGON_BLOOD:
		if(!(HDrain_resistance & INTRINSIC)) {
			You_feel("resistant to level drainage.");
			incr_itimeout(&HDrain_resistance, HDrain_resistance ? (rnd(10) + spell_damage_bonus(SPE_DRAGON_BLOOD)) : (rn1(100, 50) + spell_damage_bonus(SPE_DRAGON_BLOOD)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
	case SPE_ANTI_MAGIC_FIELD:
		if(!(HAntimagic & INTRINSIC)) {
			You_feel("resistant to magic.");
			incr_itimeout(&HAntimagic, HAntimagic ? (rnd(10) + spell_damage_bonus(SPE_ANTI_MAGIC_FIELD)) : (rn1(100, 50) + spell_damage_bonus(SPE_ANTI_MAGIC_FIELD)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;

	case SPE_NO_EFFECT:

		pline("%s", nothing_happens);
		if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
			pline("Oh wait, actually something bad happens...");
			badeffect();
		}

		break;

	case SPE_CURE_WOUNDED_LEGS:

		if (Wounded_legs) pline("A warm glow spreads through your %s!", makeplural(body_part(LEG)));

		if (HWounded_legs > 0) {
			HWounded_legs -= rn1(20, 20);
			if (HWounded_legs < 0) HWounded_legs = 0; /* fail safe */
		}

		break;

	case SPE_CURE_GLIB:

		if (Glib) {
			pline("You clean your %s.", makeplural(body_part(HAND)));
			if (Glib > (rn1(Role_if(PM_HEALER) ? 200 : 100, 20))) {
				int effreduction = rnd(Glib / 2);
				if (effreduction > 0) {
					Glib -= effreduction;
					Your("glib counter is reduced.");
				}
				if (!Role_if(PM_HEALER) && !rn2(500)) {
					pline("The spell backlashes!");
					badeffect();
				}
			} else {
				Glib = 0;
			}
		}

		break;

	case SPE_CUTTING:

		switch (rnd(7)) {

			case 1: pline("You put the knife to your lower %s and cut...", body_part(ARM)); break;
			case 2: pline("You use a sharp object to cut open your belly..."); break;
			case 3: pline("You slide your body along a rough surface and sustain terrible %s rashes.", body_part(BODY_SKIN)); break;
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
		int wflvl = (u.ulevel / 2);
		if (wflvl < 1) wflvl = 1;

		for (wfm = fmon; wfm; wfm = wfm2) {
			wfm2 = wfm->nmon;
			if ( ((wfm->m_lev < wflvl) || (!rn2(4) && wfm->m_lev < (2 * wflvl))) && wfm->mnum != quest_info(MS_NEMESIS) && !(wfm->data->geno & G_UNIQ) ) {
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
			gainlevelmaybe();
			increasesanity_noeffect(rnz(1000), TRUE); /* doesn't give bad effects --Amy */
			flags.botl = TRUE;
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

		if (level.flags.nommap) {

			pline("Whoops, something blocks the spell's power and causes it to backfire.");
			badeffect();
			break;

		}

		if (role_skill >= P_SUPREME_MASTER) n = 4;
		else if (role_skill >= P_GRAND_MASTER) n = 5;
		else if (role_skill >= P_MASTER) n = 7;
		else if (role_skill >= P_EXPERT) n = 9;
		else if (role_skill >= P_SKILLED) n = 11;
		else if (role_skill >= P_BASIC) n = 13;
		else n = 15;	/* Unskilled or restricted */
		if (!rn2(n)) {
		    struct trap *t;
		    long save_Hconf = HConfusion,
			 save_Hhallu = HHallucination;
	
		    HConfusion = HHallucination = 0L;
		    for (t = ftrap; t != 0; t = t->ntrap) {
			if (!rn2(2)) continue;
			/* easier trap difficulty check compared to other detection methods because map level is powerful --Amy */
			if (!t->hiddentrap && (t->trapdiff < rnd(150)) ) t->tseen = 1;
			map_trap(t, TRUE);
		    }
		    do_mappingY();
		    HConfusion = save_Hconf;
		    HHallucination = save_Hhallu;
		    You_feel("knowledgable!");
		    object_detect(pseudo, 0, FALSE);

		    if (!rn2(3)) {
			pline("The spell backlashes!");
			badeffect();
		    }

		} else
		    pline("The map refuses to reveal its secrets.");
		break;

	case SPE_GODMODE:
		if (rn2(3)) incr_itimeout(&Invulnerable, rnd(5) );
		else incr_itimeout(&Invulnerable, rnd(5 + spell_damage_bonus(SPE_GODMODE) ) );
		You_feel("invincible!");
		break;
	case SPE_ACIDSHIELD:
		if(!(HAcid_resistance & INTRINSIC)) {
			You("are resistant to acid now. Your items, however, are not.");
			incr_itimeout(&HAcid_resistance, HAcid_resistance ? (rnd(10) + spell_damage_bonus(SPE_ACIDSHIELD)) : (rn1(100, 50) + spell_damage_bonus(SPE_ACIDSHIELD)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
	case SPE_RESIST_PETRIFICATION:
		if(!(HStone_resistance & INTRINSIC)) {
			You_feel("more limber. Let's eat some cockatrice meat!");
			incr_itimeout(&HStone_resistance, HStone_resistance ? (rnd(5) + spell_damage_bonus(SPE_RESIST_PETRIFICATION)) : (rn1(40, 20) + spell_damage_bonus(SPE_RESIST_PETRIFICATION)*4));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
	case SPE_RESIST_SLEEP:
		if(!(HSleep_resistance & INTRINSIC)) {
			if (FunnyHallu)
				pline("Too much coffee!");
			else
				You("no longer feel tired.");
			incr_itimeout(&HSleep_resistance, HSleep_resistance ? (rnd(10) + spell_damage_bonus(SPE_RESIST_SLEEP)) : (rn1(100, 50) + spell_damage_bonus(SPE_RESIST_SLEEP)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
	case SPE_FLYING:
		if(!(HFlying & INTRINSIC)) {
			You("start flying!");
			incr_itimeout(&HFlying, HFlying ? (rnd(4) + spell_damage_bonus(SPE_FLYING)) : (rn1(20, 25) + spell_damage_bonus(SPE_FLYING)*20));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
	case SPE_ENDURE_COLD:
		if(!(HCold_resistance & INTRINSIC)) {
			You_feel("warmer.");
			incr_itimeout(&HCold_resistance, HCold_resistance ? (rnd(10) + spell_damage_bonus(SPE_ENDURE_COLD)) : (rn1(100, 50) + spell_damage_bonus(SPE_ENDURE_COLD)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
	case SPE_RESIST_STUN:
		if(!(HStun_resist & INTRINSIC)) {
			You_feel("resistant to stun.");
			incr_itimeout(&HStun_resist, HStun_resist ? (rnd(10) + spell_damage_bonus(SPE_RESIST_STUN)) : (rn1(100, 50) + spell_damage_bonus(SPE_RESIST_STUN)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
	case SPE_RESIST_CONFUSION:
		if(!(HConf_resist & INTRINSIC)) {
			You_feel("resistant to confusion.");
			incr_itimeout(&HConf_resist, HConf_resist ? (rnd(10) + spell_damage_bonus(SPE_RESIST_CONFUSION)) : (rn1(100, 50) + spell_damage_bonus(SPE_RESIST_CONFUSION)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
	case SPE_RESIST_CONTAMINATION:
		if(!(HCont_resist & INTRINSIC)) {
			You_feel("resistant to contamination.");
			incr_itimeout(&HCont_resist, HCont_resist ? (rnd(10) + spell_damage_bonus(SPE_RESIST_CONTAMINATION)) : (rn1(100, 50) + spell_damage_bonus(SPE_RESIST_CONTAMINATION)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
	case SPE_ENDURE_HEAT:
		if(!(HFire_resistance & INTRINSIC)) {
			if (FunnyHallu)
				pline("Excellent! You feel, like, totally cool!");
			else
				You_feel("colder.");
			incr_itimeout(&HFire_resistance, HFire_resistance ? (rnd(10) + spell_damage_bonus(SPE_ENDURE_HEAT)) : (rn1(100, 50) + spell_damage_bonus(SPE_ENDURE_HEAT)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;
	case SPE_INSULATE:
		if(!(HShock_resistance & INTRINSIC)) {
			if (FunnyHallu)
				pline("Bummer! You've been grounded!");
			else
				You("are not at all shocked by this feeling.");
			incr_itimeout(&HShock_resistance, HShock_resistance ? (rnd(10) + spell_damage_bonus(SPE_INSULATE)) : (rn1(100, 50) + spell_damage_bonus(SPE_INSULATE)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;

	case SPE_HOLD_AIR:
		if(!(HMagical_breathing & INTRINSIC)) {
			You("hold your breath.");
			incr_itimeout(&HMagical_breathing, HMagical_breathing ? (rnd(5) + spell_damage_bonus(SPE_HOLD_AIR)) : (rn1(40, 20) + spell_damage_bonus(SPE_HOLD_AIR)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;

	case SPE_SWIMMING:
		if(!(HSwimming & INTRINSIC)) {
			You("grow water wings.");
			incr_itimeout(&HSwimming, HSwimming ? (rnd(10) + spell_damage_bonus(SPE_SWIMMING)) : (rn1(100, 50) + spell_damage_bonus(SPE_SWIMMING)*10));
		} else {
			pline("%s", nothing_happens);	/* Already have as intrinsic */
			if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
				pline("Oh wait, actually something bad happens...");
				badeffect();
			}
		}
		break;

	case SPE_RESIST_RANDOM_ELEMENT:
		switch (rnd(9)) {
			case 1:
				if(!(HShock_resistance & INTRINSIC)) {
					if (FunnyHallu)
						pline("Bummer! You've been grounded!");
					else
						You("are not at all shocked by this feeling.");
					incr_itimeout(&HShock_resistance, HShock_resistance ? (rnd(10) + spell_damage_bonus(SPE_RESIST_RANDOM_ELEMENT)) : (rn1(100, 50) + spell_damage_bonus(SPE_RESIST_RANDOM_ELEMENT)*10));
				} else {
					pline("%s", nothing_happens);	/* Already have as intrinsic */
					if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
						pline("Oh wait, actually something bad happens...");
						badeffect();
					}
				}
				break;
			case 2:
				if(!(HFire_resistance & INTRINSIC)) {
					if (FunnyHallu)
						pline("Excellent! You feel, like, totally cool!");
					else
						You_feel("colder.");
					incr_itimeout(&HFire_resistance, HFire_resistance ? (rnd(10) + spell_damage_bonus(SPE_RESIST_RANDOM_ELEMENT)) : (rn1(100, 50) + spell_damage_bonus(SPE_RESIST_RANDOM_ELEMENT)*10));
				} else {
					pline("%s", nothing_happens);	/* Already have as intrinsic */
					if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
						pline("Oh wait, actually something bad happens...");
						badeffect();
					}
				}
				break;
			case 3:
				if(!(HCold_resistance & INTRINSIC)) {
					You_feel("warmer.");
					incr_itimeout(&HCold_resistance, HCold_resistance ? (rnd(10) + spell_damage_bonus(SPE_RESIST_RANDOM_ELEMENT)) : (rn1(100, 50) + spell_damage_bonus(SPE_RESIST_RANDOM_ELEMENT)*10));
				} else {
					pline("%s", nothing_happens);	/* Already have as intrinsic */
					if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
						pline("Oh wait, actually something bad happens...");
						badeffect();
					}
				}
				break;
			case 4:
				if(!(HSleep_resistance & INTRINSIC)) {
					if (FunnyHallu)
						pline("Too much coffee!");
					else
						You("no longer feel tired.");
					incr_itimeout(&HSleep_resistance, HSleep_resistance ? (rnd(10) + spell_damage_bonus(SPE_RESIST_RANDOM_ELEMENT)) : (rn1(100, 50) + spell_damage_bonus(SPE_RESIST_RANDOM_ELEMENT)*10));
				} else {
					pline("%s", nothing_happens);	/* Already have as intrinsic */
					if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
						pline("Oh wait, actually something bad happens...");
						badeffect();
					}
				}
				break;
			case 5:
				if(!(HStone_resistance & INTRINSIC)) {
					You_feel("more limber. Let's eat some cockatrice meat!");
					incr_itimeout(&HStone_resistance, HStone_resistance ? (rnd(5) + spell_damage_bonus(SPE_RESIST_RANDOM_ELEMENT)) : (rn1(40, 20) + spell_damage_bonus(SPE_RESIST_RANDOM_ELEMENT)*4));
				} else {
					pline("%s", nothing_happens);	/* Already have as intrinsic */
					if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
						pline("Oh wait, actually something bad happens...");
						badeffect();
					}
				}
				break;
			case 6:
				if(!(HAcid_resistance & INTRINSIC)) {
					You("are resistant to acid now. Your items, however, are not.");
					incr_itimeout(&HAcid_resistance, HAcid_resistance ? (rnd(10) + spell_damage_bonus(SPE_RESIST_RANDOM_ELEMENT)) : (rn1(100, 50) + spell_damage_bonus(SPE_RESIST_RANDOM_ELEMENT)*10));
				} else {
					pline("%s", nothing_happens);	/* Already have as intrinsic */
					if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
						pline("Oh wait, actually something bad happens...");
						badeffect();
					}
				}
				break;
			case 7:
				if(!(HSick_resistance & INTRINSIC)) {
					You_feel("resistant to sickness.");
					incr_itimeout(&HSick_resistance, HSick_resistance ? (rnd(10) + spell_damage_bonus(SPE_RESIST_RANDOM_ELEMENT)) : (rn1(100, 50) + spell_damage_bonus(SPE_RESIST_RANDOM_ELEMENT)*10));
				} else {
					pline("%s", nothing_happens);	/* Already have as intrinsic */
					if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
						pline("Oh wait, actually something bad happens...");
						badeffect();
					}
				}
				break;
			case 8:
				if(!(HPoison_resistance & INTRINSIC)) {
					You_feel("healthy ..... for the moment at least.");
					incr_itimeout(&HPoison_resistance, HPoison_resistance ? (rnd(10) + spell_damage_bonus(SPE_RESIST_RANDOM_ELEMENT)) : (rn1(100, 50) + spell_damage_bonus(SPE_RESIST_RANDOM_ELEMENT)*10));
				} else {
					pline("%s", nothing_happens);	/* Already have as intrinsic */
					if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
						pline("Oh wait, actually something bad happens...");
						badeffect();
					}
				}
				break;
			case 9:
				if(!(HDisint_resistance & INTRINSIC)) {
					You_feel("quite firm for a while.");
					incr_itimeout(&HDisint_resistance, HDisint_resistance ? (rnd(10) + spell_damage_bonus(SPE_RESIST_RANDOM_ELEMENT)) : (rn1(100, 50) + spell_damage_bonus(SPE_RESIST_RANDOM_ELEMENT)*10));
				} else {
					pline("%s", nothing_happens);	/* Already have as intrinsic */
					if (FailureEffects || u.uprops[FAILURE_EFFECTS].extrinsic || have_failurestone()) {
						pline("Oh wait, actually something bad happens...");
						badeffect();
					}
				}
				break;
		}

		break;

	case SPE_FORBIDDEN_KNOWLEDGE:
		if(!(HHalf_spell_damage & INTRINSIC)) {
			if (FunnyHallu)
				pline("Let the casting commence!");
			else
				You_feel("a sense of spell knowledge.");
			incr_itimeout(&HHalf_spell_damage, rn1(500, 250) + spell_damage_bonus(SPE_FORBIDDEN_KNOWLEDGE)*50);
		}
		if(!(HHalf_physical_damage & INTRINSIC)) {
			if (FunnyHallu)
				You_feel("like a tough motherfucker!");
			else
				You("are resistant to normal damage.");
			incr_itimeout(&HHalf_physical_damage, rn1(500, 250) + spell_damage_bonus(SPE_FORBIDDEN_KNOWLEDGE)*50);
		}
		u.ugangr++;

		break;

	case SPE_ALTER_REALITY:

		alter_reality(0);

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

		u.lamefarmer = TRUE;

		if (rn2(3) && role_skill >= P_SKILLED) cnt += rnd(role_skill - P_BASIC);
		while(cnt--) {
			mtmp = make_helper((pseudo->otyp == SPE_FLAME_SPHERE) ?
					PM_SUMMONED_FLAMING_SPHERE : PM_SUMMONED_FREEZING_SPHERE, u.ux, u.uy);
			if (!mtmp) continue;
			mtmp->mtame = 10;
			mtmp->mhpmax = mtmp->mhp = 1;
			mtmp->isspell = mtmp->uexp = TRUE;
		} /* end while... */

		u.lamefarmer = FALSE;

		break;
	}

	case SPE_SHOCKING_SPHERE:
	{	register int cnt = 1;
		struct monst *mtmp;

		u.lamefarmer = TRUE;

		if (rn2(3) && role_skill >= P_SKILLED) cnt += rnd(role_skill - P_BASIC);
		while(cnt--) {
			mtmp = make_helper(PM_SUMMONED_SHOCKING_SPHERE, u.ux, u.uy);
			if (!mtmp) continue;
			mtmp->mtame = 10;
			mtmp->mhpmax = mtmp->mhp = 1;
			mtmp->isspell = mtmp->uexp = TRUE;
		} /* end while... */

		u.lamefarmer = FALSE;

		break;
	}

	case SPE_ACID_SPHERE:
	{	register int cnt = 1;
		struct monst *mtmp;

		u.lamefarmer = TRUE;

		if (rn2(3) && role_skill >= P_SKILLED) cnt += rnd(role_skill - P_BASIC);
		while(cnt--) {
			mtmp = make_helper(PM_SUMMONED_ACID_SPHERE, u.ux, u.uy);
			if (!mtmp) continue;
			mtmp->mtame = 10;
			mtmp->mhpmax = mtmp->mhp = 1;
			mtmp->isspell = mtmp->uexp = TRUE;
		} /* end while... */

		u.lamefarmer = FALSE;

		break;
	}


	/* KMH -- new spells */
	case SPE_PASSWALL:
		if (!Passes_walls)
			You_feel("ethereal.");
		incr_itimeout(&HPasses_walls, rn1(10, 5));

		/* way too uber, needs nerf --Amy */
		if (!rn2(5)) badeffect();

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
			badeffect();
		}
		break;
	case SPE_REFLECTION:
		cast_reflection();
		break;
	case SPE_REPAIR_ARMOR:
		/* removes one level of erosion (both types) for a random piece of armor */
		if (CannotSelectItemsInPrompts) break;
repairarmorchoice:
		otmp = getobj(allnoncount, "magically enchant");
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
				if (otmp && objects[(otmp)->otyp].oc_material == MT_CELESTIUM && !stack_too_big(otmp)) {
					if (!otmp->cursed) bless(otmp);
					else uncurse(otmp, FALSE);
				}
			} else {
				if (!Blind) {
					pline("Your %s glows briefly, but looks as new as ever.",xname(otmp));
				}
			}
		} else {
			if (yn("Really exit with no object selected?") == 'y')
				pline("You just wasted the opportunity to enchant your armor.");
			else goto repairarmorchoice;
			/* the player can probably feel this, so no need for a !Blind check :) */
			pline("Your embarrassing %s rash clears up slightly.", body_part(BODY_SKIN));
		}
		break;

	case SPE_REROLL_ARTIFACT:

		if (CannotSelectItemsInPrompts) break;
		pline("You may choose an artifact in your inventory to reroll. It may not be a worn one though.");
rerollartifactchoice:
		otmp = getobj(allnoncount, "reroll");
		if (!otmp) {
			if (yn("Really exit with no object selected?") == 'y')
				pline("You just wasted the opportunity to reroll an artifact.");
			else goto rerollartifactchoice;
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
				otmp->otyp = rnd_class(IMPLANT_OF_ABSORPTION,IMPLANT_OF_VIRTUE);
				break;
		}

		otmp->owt = weight(otmp);

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
		if(uwep && (uwep->oclass == WEAPON_CLASS || uwep->oclass == BALL_CLASS || uwep->oclass == GEM_CLASS || uwep->oclass == CHAIN_CLASS || uwep->oclass == VENOM_CLASS || is_weptool(uwep))) {
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

			(void) safe_teleds_normalterrain(FALSE);

			recursioneffect();

		}

		break;

	default:
		/*impossible("Unknown spell %d attempted.", spell);*/
		pline("You attempted to cast a spell %d that either doesn't exist in this game, or it has been genocided.", spell);
		obfree(pseudo, (struct obj *)0);
		return(0);
	}

	if (metronomespell) {
		return(1);
	}

	/* gain skill for successful cast */
	use_skill(skill, spellev(spell));
	if (rn2(2)) use_skill(skill, spellev(spell)); /* let's make gaining spellcasting skill a little bit easier --Amy */
	if (!rn2(50)) use_skill(skill, (spellev(spell) * 10) ); /* jackpot! */

	/* WAC successful casting increases solidity of knowledge */

	if (!SpellColorCyan && !(pseudo && pseudo->otyp == SPE_ADD_SPELL_MEMORY) ) {

		int castboost = Race_if(PM_DUNADAN) ? DUNADAN_CAST_BOOST : CAST_BOOST;
		int baseboost = castboost; /* because multiplicative bonuses shouldn't exponentially increase it --Amy */
		boolean canboostmore = TRUE;

		if (pseudo) {
			switch (pseudo->otyp) {

				case SPE_ALTER_REALITY:
				case SPE_REBOOT:
				case SPE_CLONE_MONSTER:
				case SPE_THRONE_GAMBLE:
				case SPE_GAIN_LEVEL:
				case SPE_ATTUNE_MAGIC:
				case SPE_REROLL_ARTIFACT:
				case SPE_CHARGING:
				case SPE_RELOCATION:
				case SPE_AULE_SMITHING:
				case SPE_REPAIR_WEAPON:
				case SPE_REPAIR_ARMOR:
				case SPE_PASSWALL:
					canboostmore = FALSE; break;

				default: break;
			}
		}

		if (canboostmore) {

			if (irisartiboost()) {

				castboost += (baseboost * irisartiboost() / 100);

			}

		/* if a spell is down to low memory, then casting it successfully saves it from being immediately forgotten */
			if (spellknow(spell) < 5000) {
				if (spellknow(spell) < 100) {
					castboost += (baseboost * 75 / 100);
				} else if (spellknow(spell) < 1000) {
					castboost += (baseboost * 50 / 100);
				} else if (spellknow(spell) < 2000) {
					castboost += (baseboost * 40 / 100);
				} else if (spellknow(spell) < 3000) {
					castboost += (baseboost * 30 / 100);
				} else if (spellknow(spell) < 4000) {
					castboost += (baseboost * 20 / 100);
				} else if (spellknow(spell) < 5000) {
					castboost += (baseboost * 10 / 100);
				}
			}

			if (Spellboost) castboost += (baseboost * 10 / 100);
			if (StrongSpellboost) castboost += (baseboost * 10 / 100);
			if (tech_inuse(T_SPELL_SPAM)) castboost += (baseboost * 10 / 100);

			if (!PlayerCannotUseSkills) {

				switch (P_SKILL(P_MEMORIZATION)) {
					case P_BASIC: castboost += (baseboost * 5 / 100); break;
					case P_SKILLED: castboost += (baseboost * 10 / 100); break;
					case P_EXPERT: castboost += (baseboost * 15 / 100); break;
					case P_MASTER: castboost += (baseboost * 20 / 100); break;
					case P_GRAND_MASTER: castboost += (baseboost * 25 / 100); break;
					case P_SUPREME_MASTER: castboost += (baseboost * 30 / 100); break;
				}
			}

			if (spellev(spell) >= 2 && !rn2(2)) {
				castboost += (baseboost * 10 / 100);
			}
			if (spellev(spell) >= 3 && !rn2(2)) {
				castboost += (baseboost * 9 / 100);
			}
			if (spellev(spell) >= 4 && !rn2(2)) {
				castboost += (baseboost * 8 / 100);
			}
			if (spellev(spell) >= 5 && !rn2(2)) {
				castboost += (baseboost * 7 / 100);
			}
			if (spellev(spell) >= 6 && !rn2(2)) {
				castboost += (baseboost * 6 / 100);
			}
			if (spellev(spell) >= 7 && !rn2(2)) {
				castboost += (baseboost * 5 / 100);
			}
			if (spellev(spell) >= 8 && !rn2(2)) {
				castboost += (baseboost * 4 / 100);
			}
		}

		boostknow(spell, castboost);
		if (((rnd(spellev(spell) + 5)) > 5) && canboostmore) boostknow(spell, castboost); /* higher-level spells boost more --Amy */
		if (!rn2(52 - (spellev(spell) * 2) ) && canboostmore && !Race_if(PM_DUNADAN) ) { /* jackpot! */
			boostknow(spell, (castboost * 5) );
			boostknow(spell, (castboost * spellev(spell) ) );
		}

		if (Role_if(PM_MAHOU_SHOUJO)) boostknow(spell, castboost);
		if (Role_if(PM_PSYKER)) boostknow(spell, castboost);

	}

	if (!(iflags.memorizationknown) && !(spl_book[spell].sp_memorize)) {
		if (yn("You just cast a dememorized spell! If this means that you want to continue using that spell later, you might want to set it back to memorization mode so that its spell memory lasts longer. Press y now to set the spell back to memorization mode, or n to keep it dememorized.") == 'y') {
			spl_book[spell].sp_memorize = TRUE;
			pline("Spell memorization was activated!");
		}
	}

	if (SpellColorCyan) {
		boolean cyanwillgodown = TRUE;

		/* keep this synchronized with age_spells(), please --Amy */

		if (Keen_memory && !rn2(StrongKeen_memory ? 3 : 5)) cyanwillgodown = FALSE;
		if (uarm && uarm->oartifact == ART_COPE_OF_THE_ELDRITCH_KNIGH && rn2(2)) cyanwillgodown = FALSE;
		if (uarmc && itemhasappearance(uarmc, APP_GUILD_CLOAK) ) cyanwillgodown = FALSE;
		if (uamul && uamul->oartifact == ART_SUPER_MEMORY) cyanwillgodown = FALSE;
		if (tech_inuse(T_SPELL_SPAM)) cyanwillgodown = FALSE;
		if (uarms && uarms->oartifact == ART_UNUSUAL_ENCH && !rn2(5)) cyanwillgodown = FALSE;
		if (uarm && uarm->oartifact == ART_FLEECELGIRL_FROM_THE_OLD_T && !rn2(2)) cyanwillgodown = FALSE;
		if (rnd(urole.spelrete + 100) > 100) cyanwillgodown = FALSE;

		if (!rn2(2) && spellmemorize(spell)) cyanwillgodown = FALSE;

		if (spellmemorize(spell) && ACURR(A_INT) > 14) {
			int intelsave = ACURR(A_INT) - 14;
			if (intelsave > rn2(66)) cyanwillgodown = FALSE;
		}

		if (!PlayerCannotUseSkills && spellmemorize(spell) && P_SKILL(P_MEMORIZATION) >= P_BASIC) {

			int savememochance = 0;

			switch (P_SKILL(P_MEMORIZATION)) {
				case P_BASIC: savememochance = 1; break;
				case P_SKILLED: savememochance = 2; break;
				case P_EXPERT: savememochance = 3; break;
				case P_MASTER: savememochance = 4; break;
				case P_GRAND_MASTER: savememochance = 5; break;
				case P_SUPREME_MASTER: savememochance = 6; break;
			}

			if (savememochance > rn2(10)) cyanwillgodown = FALSE;

		}

		if (!PlayerCannotUseSkills && spellmemorize(spell) && P_SKILL(spell_skilltype(spellid(spell))) >= P_BASIC) {

			int savememochance = 0;

			switch (P_SKILL(spell_skilltype(spellid(spell)))) {
				case P_BASIC: savememochance = 1; break;
				case P_SKILLED: savememochance = 2; break;
				case P_EXPERT: savememochance = 3; break;
				case P_MASTER: savememochance = 4; break;
				case P_GRAND_MASTER: savememochance = 5; break;
				case P_SUPREME_MASTER: savememochance = 6; break;
			}

			if (savememochance > rn2(20)) cyanwillgodown = FALSE;

		}

		if (cyanwillgodown) {
			drainknow(spell, rnd(100));
			if (spellknow(spell) < 0) spl_book[spell].sp_know = 0;
		}
	} /* end cyan spells */

	if (pseudo && pseudo->otyp == SPE_REBOOT && !rn2(10) ) {

		drainknow(spell, (rnd(20000)));
		if (spellknow(spell) < 0) spl_book[spell].sp_know = 0;

	}

	if (pseudo && ((pseudo->otyp == SPE_ALTER_REALITY) || (pseudo->otyp == SPE_CLONE_MONSTER) ) ) {

		drainknow(spell, (rnd(20000)));
		if (spellknow(spell) < 0) spl_book[spell].sp_know = 0;

	}

	if (pseudo && (pseudo->otyp == SPE_THRONE_GAMBLE) && !rn2(6) ) {

		drainknow(spell, (rnd(100000)));
		if (spellknow(spell) < 0) spl_book[spell].sp_know = 0;

	}

	if (pseudo && (pseudo->otyp == SPE_GAIN_LEVEL) && !rn2(200) ) {

		drainknow(spell, (rnd(50000)));
		if (spellknow(spell) < 0) spl_book[spell].sp_know = 0;

	}

	if (pseudo && (pseudo->otyp == SPE_ATTUNE_MAGIC) && !rn2(6) ) {

		drainknow(spell, (rnd(100000)));
		if (spellknow(spell) < 0) spl_book[spell].sp_know = 0;

	}

	if (pseudo && (pseudo->otyp == SPE_REROLL_ARTIFACT) && !rn2(5) ) {

		drainknow(spell, (rnd(100000)));
		if (spellknow(spell) < 0) spl_book[spell].sp_know = 0;

	}

	if (pseudo && (pseudo->otyp == SPE_CHARGING) && !rn2(role_skill == P_SUPREME_MASTER ? 9 : role_skill == P_GRAND_MASTER ? 8 : role_skill == P_MASTER ? 7 : role_skill == P_EXPERT ? 6 : role_skill == P_SKILLED ? 5 : 4) ) {

		drainknow(spell, (rnd(100000)));
		if (spellknow(spell) < 0) spl_book[spell].sp_know = 0;

	}

	if (pseudo && (pseudo->otyp == SPE_RELOCATION) && !rn2(role_skill == P_SUPREME_MASTER ? 9 : role_skill == P_GRAND_MASTER ? 8 : role_skill == P_MASTER ? 7 : role_skill == P_EXPERT ? 6 : role_skill == P_SKILLED ? 5 : 4) ) {

		drainknow(spell, (rnd(10000)));
		if (spellknow(spell) < 0) spl_book[spell].sp_know = 0;

	}

	if (pseudo && pseudo->otyp == SPE_ADD_SPELL_MEMORY) {

		drainknow(spell, 500);
		if (spellknow(spell) < 0) spl_book[spell].sp_know = 0;

	}

	if (pseudo && (pseudo->otyp == SPE_AULE_SMITHING) && !rn2(role_skill == P_SUPREME_MASTER ? 9 : role_skill == P_GRAND_MASTER ? 8 : role_skill == P_MASTER ? 7 : role_skill == P_EXPERT ? 6 : role_skill == P_SKILLED ? 5 : 4) ) {

		drainknow(spell, (rnd(100000)));
		if (spellknow(spell) < 0) spl_book[spell].sp_know = 0;

	}

	if (pseudo && ((pseudo->otyp == SPE_REPAIR_WEAPON) || (pseudo->otyp == SPE_REPAIR_ARMOR)) && !rn2(role_skill == P_SUPREME_MASTER ? 15 : role_skill == P_GRAND_MASTER ? 14 : role_skill == P_MASTER ? 13 : role_skill == P_EXPERT ? 12 : role_skill == P_SKILLED ? 11 : 10) ) {

		drainknow(spell, (rnd(25000)));
		if (spellknow(spell) < 0) spl_book[spell].sp_know = 0;

	}

	if (pseudo && (pseudo->otyp == SPE_PASSWALL) && !rn2(role_skill == P_SUPREME_MASTER ? 30 : role_skill == P_GRAND_MASTER ? 25 : role_skill == P_MASTER ? 24 : role_skill == P_EXPERT ? 23 : role_skill == P_SKILLED ? 22 : 20) ) {

		drainknow(spell, (rnd(10000)));
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

	/* particle cannon and one point shoot need "gauge"; since this isn't Elona, we don't have an actual gauge meter,
	 * so I decided that it just takes 50 turns to reload --Amy */
	if (pseudo && (pseudo->otyp == SPE_PARTICLE_CANNON)) {
		pline("The particle cannon can be used again at turn %ld.", (moves + u.gaugetimer));
	}
	if (pseudo && (pseudo->otyp == SPE_CONVERGE_BREATH)) {
		pline("Converge Breath can be used again at turn %ld.", (moves + u.gaugetimer));
	}
	if (pseudo && (pseudo->otyp == SPE_IMPACT_GUNFIRE)) {
		pline("Impact Gunfire can be used again at turn %ld.", (moves + u.gaugetimer));
	}
	if (pseudo && (pseudo->otyp == SPE_ONE_POINT_SHOOT)) {
		pline("One Point Shoot can be used again at turn %ld.", (moves + u.gaugetimer));
	}
	if (pseudo && (pseudo->otyp == SPE_BATTERING_RAM)) {
		pline("Battering Ram can be used again at turn %ld.", (moves + u.battertimer));
	}
	if (pseudo && (pseudo->otyp == SPE_BUBBLING_HOLE)) {
		nomul(-2, "recovering from the bubbling hole", TRUE);
	}

	obfree(pseudo, (struct obj *)0);	/* now, get rid of it */
	return(1);
}

/* #spelldelete command: allows the player to erase the bottommost spell outright, but only if it's a forgotten one */
int
dodeletespell()
{
	int n;
	for (n = 0; n < MAXSPELL && spellid(n) != NO_SPELL; n++)
		continue;
	if (n) {

		n--; /* fix off-by-one error */
		if (spellid(n) == NO_SPELL) {
			impossible("ERROR: dodeletespell() trying to erase empty spell?");
			return 0;
		}

		if (spellknow(n) > 0) {
			pline("This doesn't work as long as your bottommost spell still has memory left.");
		} else {
			pline("Your bottommost spell is %s, which has no memory left.", spellname(n));
			if (yn("Erase it from the list?") == 'y') {
				spellid(n) = NO_SPELL;
				/* no need to nullify spell memory because it's already zero */
				pline("Done! The spell has been removed from your list.");
			}
		}
	} else pline("It seems that you have no known spells in the first place.");

	/* shouldn't cost a turn, regardless of result */
	return 0;
}

void
losespells()
{
	boolean confused = (Confusion != 0);
	int  n, nzap, i;
	int thisone, thisonetwo, choicenumber;

	if (Keen_memory && rn2(StrongKeen_memory ? 20 : 4)) return;
	if (Role_if(PM_MASTERMIND) && mastermindsave()) return;

	/* have no spells? well then... magic vacuum :P --Amy */
	if (spellid(0) == NO_SPELL) {
		make_magicvacuum((HMagicVacuum & TIMEOUT) + rnd(10) + rnd(monster_difficulty() * 100) );
	}

	/* reduce memory of one known spell that still has memory left --Amy */
	for (n = 0; n < MAXSPELL && spellid(n) != NO_SPELL; n++)
		continue;
	if (n) {
		thisone = -1;
		choicenumber = 0;
		for (n = 0; ((n < MAXSPELL) && spellid(n) != NO_SPELL); n++) {
			if ((!choicenumber || (!rn2(choicenumber + 1)) ) && (spellknow(n) > 0)) {
				thisone = n;
			}
			if (spellknow(n) > 0) choicenumber++;
		}

		if (choicenumber > 0 && thisone >= 0 && (spellknow(thisone) > 0)) {
			if (rn2(10)) {
				spellknow(thisone) = rn2(spellknow(thisone));
			} else {
				spellknow(thisone) = 0;
			}
		}
	}

	/* maybe also reduce spell memory a bit more, to make the amnesia effect matter --Amy */
	if (!rn2(5)) spellmemoryloss(level_difficulty() + 1);

	book = 0;
	for (n = 0; n < MAXSPELL && spellid(n) != NO_SPELL; n++)
		continue;
	if (n) {
		nzap = rn2(isfriday ? 20 : 50) ? 1 : (rnd(n) + confused);

		if (n > urole.spelrete) {
			int retention = urole.spelrete;
			if (retention < 1) {
				impossible("player's spell retention isn't positive??");
				retention = 1;
			}
			int highamount = n;
			while (highamount > retention) {
				nzap++;
				highamount -= rnd(retention);
			}
		}

		if (nzap > n) nzap = n;
		for (i = n - nzap; i < n; i++) {
		    spellid(i) = NO_SPELL;
		    exercise(A_WIS, FALSE);	/* ouch! */
		}
	}

	/* now if you have too many forgotten spells, remove them --Amy */
removeagain:
	for (n = 0; n < MAXSPELL && spellid(n) != NO_SPELL; n++)
		continue;
	if (n > 4) {
		thisone = -1;
		thisonetwo = -1;
		choicenumber = 0;
		for (n = 0; ((n < MAXSPELL) && spellid(n) != NO_SPELL); n++) {
			if (spellknow(n) <= 0) {
				thisone = n;
				choicenumber++;
			}
		}

		if (choicenumber > 5 && thisone >= 0 && spellknow(thisone) <= 0) {

			spellknow(thisone) = 0; /* make sure the spell memory is deleted! */
			spellid(thisone) = NO_SPELL;

			for (n = thisone; n < MAXSPELL; n++) {
				if (spellid(n) != NO_SPELL) thisonetwo = n;
			}
			if (thisonetwo >= 0 && spellid(thisonetwo) != NO_SPELL) { /* move last known spell to the one we've erased */
				spellknow(thisone) = spellknow(thisonetwo);
				spellid(thisone) = spellid(thisonetwo);
				spellev(thisone) = spellev(thisonetwo);
				spellname(thisone) = spellname(thisonetwo);

				spellknow(thisonetwo) = 0; /* make sure the spell memory is deleted! */
				spellid(thisonetwo) = NO_SPELL;

			}

			if (choicenumber > 6) goto removeagain;
		}
	}


}

/* for bad effects: spell zonking similar to the amnesia effect */
void
evilspellforget()
{
	int n;
	int thisone, thisonetwo, choicenumber;

	/* reduce memory of one known spell that still has memory left --Amy */
	for (n = 0; n < MAXSPELL && spellid(n) != NO_SPELL; n++)
		continue;
	if (n) {
		thisone = -1;
		choicenumber = 0;
		for (n = 0; ((n < MAXSPELL) && spellid(n) != NO_SPELL); n++) {
			if ((!choicenumber || (!rn2(choicenumber + 1)) ) && (spellknow(n) > 0)) {
				thisone = n;
			}
			if (spellknow(n) > 0) choicenumber++;
		}

		if (choicenumber > 0 && thisone >= 0 && (spellknow(thisone) > 0)) {
			if (rn2(10)) {
				spellknow(thisone) = rn2(spellknow(thisone));
				pline("You forget a lot of %s spell knowledge!", spellname(thisone));
			} else {
				spellknow(thisone) = 0;
				pline("You forget all usage of the %s spell!", spellname(thisone));
			}
		}
	}

	/* now if you have too many forgotten spells, remove them --Amy */
removeagain:
	for (n = 0; n < MAXSPELL && spellid(n) != NO_SPELL; n++)
		continue;
	if (n > 4) {
		thisone = -1;
		thisonetwo = -1;
		choicenumber = 0;
		for (n = 0; ((n < MAXSPELL) && spellid(n) != NO_SPELL); n++) {
			if (spellknow(n) <= 0) {
				thisone = n;
				choicenumber++;
			}
		}

		if (choicenumber > 5 && thisone >= 0 && spellknow(thisone) <= 0) {

			spellknow(thisone) = 0; /* make sure the spell memory is deleted! */
			spellid(thisone) = NO_SPELL;

			for (n = thisone; n < MAXSPELL; n++) {
				if (spellid(n) != NO_SPELL) thisonetwo = n;
			}
			if (thisonetwo >= 0 && spellid(thisonetwo) != NO_SPELL) { /* move last known spell to the one we've erased */
				spellknow(thisone) = spellknow(thisonetwo);
				spellid(thisone) = spellid(thisonetwo);
				spellev(thisone) = spellev(thisonetwo);
				spellname(thisone) = spellname(thisonetwo);

				spellknow(thisonetwo) = 0; /* make sure the spell memory is deleted! */
				spellid(thisonetwo) = NO_SPELL;

			}

			if (choicenumber > 6) goto removeagain;
		}
	}

}

/* having too many forgotten spells just clogs up the interface, so we'll occasionally remove one --Amy */
void
removeforgottenspell(guaranteed)
boolean guaranteed;
{
	int n, thisone, thisonetwo, choicenumber;

	for (n = 0; n < MAXSPELL && spellid(n) != NO_SPELL; n++)
		continue;
	if ((n > 4) || guaranteed) {
		thisone = -1;
		thisonetwo = -1;
		choicenumber = 0;
		for (n = 0; ((n < MAXSPELL) && spellid(n) != NO_SPELL); n++) {
			if (spellknow(n) <= 0) {
				thisone = n;
				choicenumber++;
			}
		}

		if ( ((choicenumber > 5) || guaranteed) && thisone >= 0 && spellknow(thisone) <= 0) {

			pline("You completely forgot the %s spell.", spellname(thisone));
			if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
			spellknow(thisone) = 0; /* make sure the spell memory is deleted! */
			spellid(thisone) = NO_SPELL;

			for (n = thisone; n < MAXSPELL; n++) {
				if (spellid(n) != NO_SPELL) thisonetwo = n;
			}
			if (thisonetwo >= 0 && spellid(thisonetwo) != NO_SPELL) { /* move last known spell to the one we've erased */
				spellknow(thisone) = spellknow(thisonetwo);
				spellid(thisone) = spellid(thisonetwo);
				spellev(thisone) = spellev(thisonetwo);
				spellname(thisone) = spellname(thisonetwo);

				spellknow(thisonetwo) = 0; /* make sure the spell memory is deleted! */
				spellid(thisonetwo) = NO_SPELL;

			}

		}
	}

}

void
spellmemoryloss(lossamount)
int lossamount;
{
	int n, thisone, choicenumber, spell, nzap;

	for (n = 0; n < MAXSPELL && spellid(n) != NO_SPELL; n++)
		continue;
	if (n) {
		thisone = -1;
		choicenumber = 0;
		for (n = 0; n < MAXSPELL && spellid(n) != NO_SPELL; n++) {
			if ((spellknow(n) > 0) && (!choicenumber || (!rn2(choicenumber + 1))) ) {
				thisone = n;
			}
			if (spellknow(n) > 0) choicenumber++;
		}

		/* sometimes we're mean and reduce it by more, depending on how many spells you have... --Amy */
		if (!rn2(3) && choicenumber > 1) {
			lossamount *= (50 + rno(choicenumber));
			lossamount /= 50;
		}
		lossamount *= rnd(5);

		if (choicenumber > 0 && thisone >= 0) {
			drainknow(thisone, (lossamount * 100));
			if (spellknow(thisone) < 0) {
				spl_book[thisone].sp_know = 0;
				pline("You lose all knowledge of the %s spell!", spellname(thisone));
			}
			else pline("Your knowledge of the %s spell is reduced!", spellname(thisone));
		}

	}

	book = 0;
	nzap = 0;
	for (n = 0; n < MAXSPELL && spellid(n) != NO_SPELL; n++)
		continue;

	if (n > urole.spelrete) {
		int retention = urole.spelrete;
		if (retention < 1) {
			impossible("player's spell retention isn't positive??");
			retention = 1;
		}
		int highamount = n;
		while (highamount > retention) {
			nzap++;
			highamount -= rnd(retention);
		}
	}

	if (n && nzap > 0) pline("Some of your spells got erased!");

morezapping:

	for (n = 0; n < MAXSPELL && spellid(n) != NO_SPELL; n++)
		continue;

	if (n >= 0 && spellid(n) == NO_SPELL) n--;

	if (nzap > n) nzap = n;
	if (n && nzap > 0) {

		while (nzap > 0) {

			if (spellknow(n) > 0) drainknow(n, 10000);
			if (spellknow(n) < 0) spellknow(n) = 0;

			if (spellknow(n) <= 0) {
				spl_book[n].sp_know = 0;
				spellid(n) = NO_SPELL;
				exercise(A_WIS, FALSE);	/* ouch! */
				nzap--;
				n--;
				goto morezapping;
			}
			nzap--;
		}
	}

}

/* halve memory of a randomly selected spell with nonzero memory --Amy */
void
spellmemoryhalve()
{
	int n, thisone, choicenumber, spell, nzap;

	for (n = 0; n < MAXSPELL && spellid(n) != NO_SPELL; n++)
		continue;
	if (n) {
		thisone = -1;
		choicenumber = 0;
		for (n = 0; n < MAXSPELL && spellid(n) != NO_SPELL; n++) {
			if ((spellknow(n) > 0) && (!choicenumber || (!rn2(choicenumber + 1))) ) {
				thisone = n;
			}
			if (spellknow(n) > 0) choicenumber++;
		}

		if (choicenumber > 0 && thisone >= 0) {
			spl_book[thisone].sp_know /= 2;
			if (spellknow(thisone) < 0) {
				spl_book[thisone].sp_know = 0;
				pline("You lose all knowledge of the %s spell!", spellname(thisone));
			}
			else pline("Your knowledge of the %s spell is cut in half!", spellname(thisone));
		}

	}

}

/* delete all memory of a randomly selected spell with nonzero memory --Amy */
void
spellmemorynull()
{
	int n, thisone, choicenumber, spell, nzap;

	for (n = 0; n < MAXSPELL && spellid(n) != NO_SPELL; n++)
		continue;
	if (n) {
		thisone = -1;
		choicenumber = 0;
		for (n = 0; n < MAXSPELL && spellid(n) != NO_SPELL; n++) {
			if ((spellknow(n) > 0) && (!choicenumber || (!rn2(choicenumber + 1))) ) {
				thisone = n;
			}
			if (spellknow(n) > 0) choicenumber++;
		}

		if (choicenumber > 0 && thisone >= 0) {
			spl_book[thisone].sp_know = 0;
			pline("You lose all knowledge of the %s spell!", spellname(thisone));
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

	if (MenuIsBugged) {
	pline("The spell menu command is currently unavailable!");
	if (flags.moreforced && !MessagesSuppressed) display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	return 0;
	}

	int dememonum = 0;
	int i;

	char spellcolorbuf[BUFSZ];

	if (SpellColorPink) sprintf(spellcolorbuf, "Your spells are pink.");
	else if (SpellColorBrightCyan) sprintf(spellcolorbuf, "Your spells are bright cyan.");
	else if (SpellColorCyan) sprintf(spellcolorbuf, "Your spells are cyan.");
	else if (SpellColorBlack) sprintf(spellcolorbuf, "Your spells are black.");
	else if (SpellColorOrange) sprintf(spellcolorbuf, "Your spells are orange.");
	else if (SpellColorRed) sprintf(spellcolorbuf, "Your spells are red.");
	else if (SpellColorPlatinum) sprintf(spellcolorbuf, "Your spells are platinum.");
	else if (SpellColorSilver) sprintf(spellcolorbuf, "Your spells are silver.");
	else if (SpellColorMetal) sprintf(spellcolorbuf, "Your spells are metal.");
	else if (SpellColorGreen) sprintf(spellcolorbuf, "Your spells are green.");
	else if (SpellColorBlue) sprintf(spellcolorbuf, "Your spells are blue.");
	else if (SpellColorGray) sprintf(spellcolorbuf, "Your spells are completely gray.");
	else if (SpellColorBrown) sprintf(spellcolorbuf, "Your spells are brown.");
	else if (SpellColorWhite) sprintf(spellcolorbuf, "Your spells are white.");
	else if (SpellColorViolet) sprintf(spellcolorbuf, "Your spells are violet.");
	else if (SpellColorYellow) sprintf(spellcolorbuf, "Your spells are yellow.");
	else sprintf(spellcolorbuf, "Currently known spells");

	if (spellid(0) == NO_SPELL)
	    You("don't know any spells right now.");
	else {
	    while (dospellmenu( spellcolorbuf, SPELLMENU_VIEW, &splnum, 0)) {
		sprintf(qbuf, "Reordering spells; swap '%s' with",
			YouHaveLostSpells ? "spell" : spellname(splnum));
		if (!dospellmenu(qbuf, splnum, &othnum, 0)) break;

		spl_tmp = spl_book[splnum];
		spl_book[splnum] = spl_book[othnum];
		spl_book[othnum] = spl_tmp;
	    }
	}

	dememonum = 0;
	for (i = 0; i < MAXSPELL; i++) {
		if (spellid(i) == NO_SPELL) break;
		if (spellmemorize(i)) continue;
		dememonum++;
	}
	if (iflags.memorizationknown) dememonum = 0;

	if (dememonum >= 10) pline("You set a very large amount of spells to dememorization mode. A common misconception of players unfamiliar with the memorization system is that dememorizing some spells would magically make the memorized spells last longer. That is NOT the case. The spells are independent from each other. Every spell set to memorization mode gets a 10%% reduction in spell decay rate per memorization skill level (i.e. spell memory persists longer), so if you want to keep casting those spells, you should set them back to memorization mode. You can do that by opening the spell menu (with the + key) and pressing ? twice, which brings you to the memorization menu.");
	else if (dememonum >= 5) pline("You set many spells to dememorization mode (indicated by a - next to the spell level). All those spells aren't getting the spell memory bonus from memorization skill, which is highly impractical unless you WANT to forget those spells for some bizarre reason. It's advisable to have only a single spell set to dememorization mode, i.e. the one you want to turn into a forgotten spell. You can change it in the spell menu (hit + to open it and ? twice to navigate to the memorization menu).");
	else if (dememonum >= 2) pline("You set several spells to dememorization mode (indicated by a - next to the spell level), which means that those spells currently don't benefit from increased spell memory. If you want your spells to benefit from the memorization skill, you should open the spell menu (+ key by default) and navigate to the memorization menu by pressing ? twice, then setting the spells in question back to memorization mode.");

	return 0;
}

STATIC_OVL boolean
dospellmenu(prompt, splaction, spell_no, specialmenutype)
const char *prompt;
int splaction;	/* SPELLMENU_CAST, SPELLMENU_VIEW, or spl_book[] index */
int *spell_no;
/* specialmenutype: 0 = show spells, 1 = describe spells, 2 = memorize spells */
int specialmenutype;
{
	winid tmpwin;
	int i, n, how;
	char buf[BUFSZ];
	menu_item *selected;
	anything any;
	boolean describe;

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
		sprintf(buf, "%-20s     Level  Pw  %-10s Fail  Memory", "    Name", " Category");
	else
		sprintf(buf, "Name\tLevel\tPw\t Category\tFail");
	if (flags.menu_style == MENU_TRADITIONAL)
		strcat(buf, iflags.menu_tab_sep ? "\tKey" : "  Key");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	if (!YouHaveLostSpells) {for (i = 0; i < MAXSPELL && spellid(i) != NO_SPELL; i++) {
		sprintf(buf, iflags.menu_tab_sep ?
			"%s\t%-d%s\t%s%s %d\t%-d%%" : "%-20s  %2d%s%s %3d  %-8s %4d%%"
			"   %3d%%",
			spellname(i), spellev(i),
			((spellknow(i) > 1000) || SpellColorCyan) ? " " : (spellknow(i) ? "!" : "*"),
			spellmemorize(i) ? "+" : "-",
			manacost(spellid(i)),
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

	if (splaction == SPELLMENU_VIEW) {
		if (specialmenutype == 0) {
			any.a_int = -1;	/* must be non-zero */
			add_menu(tmpwin, NO_GLYPH, &any,
				'?', 0, ATR_NONE, "Describe a spell instead",
				MENU_UNSELECTED);
		}
		else if (specialmenutype == 1) {
			any.a_int = -1;	/* must be non-zero */
			add_menu(tmpwin, NO_GLYPH, &any,
				'?', 0, ATR_NONE, "Memorize a spell instead",
				MENU_UNSELECTED);
		}
		else if (specialmenutype == 2) {
			any.a_int = -1;	/* must be non-zero */
			add_menu(tmpwin, NO_GLYPH, &any,
				'?', 0, ATR_NONE, "View spells instead",
				MENU_UNSELECTED);
		}
	}

	end_menu(tmpwin, prompt);

	how = PICK_ONE;
	/*if (splaction == SPELLMENU_VIEW && spellid(1) == NO_SPELL)
	    how = PICK_NONE;*/	/* only one spell => nothing to swap with */ /* Amy edit: blah, we have the ? option now */
	n = select_menu(tmpwin, how, &selected);
	destroy_nhwindow(tmpwin);
	if (n > 0 && selected[0].item.a_int == -1) {

		char spellcolorbuf[BUFSZ];

		if (SpellColorPink) sprintf(spellcolorbuf, "Your spells are pink.");
		else if (SpellColorBrightCyan) sprintf(spellcolorbuf, "Your spells are bright cyan.");
		else if (SpellColorCyan) sprintf(spellcolorbuf, "Your spells are cyan.");
		else if (SpellColorBlack) sprintf(spellcolorbuf, "Your spells are black.");
		else if (SpellColorOrange) sprintf(spellcolorbuf, "Your spells are orange.");
		else if (SpellColorRed) sprintf(spellcolorbuf, "Your spells are red.");
		else if (SpellColorPlatinum) sprintf(spellcolorbuf, "Your spells are platinum.");
		else if (SpellColorSilver) sprintf(spellcolorbuf, "Your spells are silver.");
		else if (SpellColorMetal) sprintf(spellcolorbuf, "Your spells are metal.");
		else if (SpellColorGreen) sprintf(spellcolorbuf, "Your spells are green.");
		else if (SpellColorBlue) sprintf(spellcolorbuf, "Your spells are blue.");
		else if (SpellColorGray) sprintf(spellcolorbuf, "Your spells are completely gray.");
		else if (SpellColorBrown) sprintf(spellcolorbuf, "Your spells are brown.");
		else if (SpellColorWhite) sprintf(spellcolorbuf, "Your spells are white.");
		else if (SpellColorViolet) sprintf(spellcolorbuf, "Your spells are violet.");
		else if (SpellColorYellow) sprintf(spellcolorbuf, "Your spells are yellow.");
		else if (specialmenutype == 0) sprintf(spellcolorbuf, "Choose a spell to describe");
		else if (specialmenutype == 1) sprintf(spellcolorbuf, "Choose a spell to (de-)memorize");
		else if (specialmenutype == 2) sprintf(spellcolorbuf, "Currently known spells");
		else sprintf(spellcolorbuf, "Bug: unknown spell menu title");

		return dospellmenu(spellcolorbuf, splaction, spell_no, (specialmenutype == 2) ? 0 : (specialmenutype + 1));
	}
	if (n > 0 && splaction == SPELLMENU_VIEW && specialmenutype == 1) { /* describe */

		register struct obj *pseudo;

		pseudo = mksobj(spellid(selected[0].item.a_int - 1), FALSE, 2, FALSE);
		if (!pseudo) {
			impossible("bugged pseudo object for spell description.");
			return dospellmenu(prompt, splaction, spell_no, specialmenutype);
		}
		if (pseudo->otyp == GOLD_PIECE) pseudo->otyp = spellid(selected[0].item.a_int - 1); /* minimalist fix */
		pseudo->blessed = pseudo->cursed = 0;
		pseudo->quan = 20L;			/* do not let useup get it */

		if (YouHaveLostSpells) {
			/* you cheater! you're not supposed to see what the spells are :P */
			return dospellmenu(prompt, splaction, spell_no, specialmenutype);
		}

		(void) itemactions(pseudo, TRUE, TRUE);
		obfree(pseudo, (struct obj *)0);	/* now, get rid of it */
		return dospellmenu(prompt, splaction, spell_no, specialmenutype);
	}
	if (n > 0 && splaction == SPELLMENU_VIEW && specialmenutype == 2) { /* memorize */

		spl_book[selected[0].item.a_int - 1].sp_memorize = !spl_book[selected[0].item.a_int - 1].sp_memorize;

		char spellcolorbuf[BUFSZ];

		if (YouHaveLostSpells) sprintf(spellcolorbuf, "Memorization setting changed."); /* no, you don't get to see what it is now :P */
		else if (spl_book[selected[0].item.a_int - 1].sp_memorize) sprintf(spellcolorbuf, "Spell set to memorization mode!");
		else sprintf(spellcolorbuf, "Memorization for this spell deactivated.");

		return dospellmenu(spellcolorbuf, splaction, spell_no, specialmenutype);
	}
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

	sprintf(buf, "%-20s   Level  Pw  %-10s Fail  Memory", "    Name", " Category");
	dump("  ",buf);
	for (i = 0; i < MAXSPELL && spellid(i) != NO_SPELL; i++) {
		sprintf(buf, "%c - %-19s %2d%s%s %3d  %-8s %4d%%  %3d%%",
			spellet(i), spellname(i), spellev(i),
			((spellknow(i) > 1000) || SpellColorCyan) ? " " : (spellknow(i) ? "!" : "*"),
			spellmemorize(i) ? "+" : "-",
			manacost(spellid(i)),
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
int
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
	if (uarm && !SpellColorMetal && !(uarm->blessed && !issoviet) && !(uarm->otyp >= ELVEN_TOGA && uarm->otyp <= ROBE_OF_WEAKNESS) && !(uarm->oartifact == ART_HA_MONK) && !(uarm->oartifact == ART_BOBAIS) && !(uarm->oartifact == ART_AMMY_S_RETRIBUTION) ) 
	    splcaster += (issoviet ? 2 : 1);

	/* Robes are body armour in SLASH'EM */
	if (uarm && !SpellColorMetal && (is_metallic(uarm) || is_filmitem(uarm)) && !is_etheritem(uarm) && !is_meteosteelitem(uarm)) {

		/* Amy grepping target: "materialeffect" */
		/* baseline = 15, used by iron, alumen and sinnum */
		switch (objects[(uarm)->otyp].oc_material) {
			default: break;
			case MT_METAL: armorpenalties *= 16; armorpenalties /= 15; break;
			case MT_STEEL: armorpenalties *= 16; armorpenalties /= 15; break;
			case MT_GREEN_STEEL: armorpenalties *= 12; armorpenalties /= 15; break;
			case MT_MERCURIAL: armorpenalties *= 3; armorpenalties /= 15; break;
			case MT_BRONZE: armorpenalties *= 21; armorpenalties /= 15; break;
			case MT_TITANIUM: armorpenalties *= 18; armorpenalties /= 15; break;
			case MT_COPPER: armorpenalties *= 17; armorpenalties /= 15; break;
			case MT_COBALT: armorpenalties *= 16; armorpenalties /= 15; break;
			case MT_SILVER: armorpenalties *= 17; armorpenalties /= 15; break;
			case MT_GOLD: armorpenalties *= 8; armorpenalties /= 15; break;
			case MT_TINSEL: armorpenalties *= 10; armorpenalties /= 15; break;
			case MT_PLATINUM: armorpenalties *= 18; armorpenalties /= 15; break;
			case MT_FILM: armorpenalties *= 18; armorpenalties /= 15; break;
			case MT_ALLOY: armorpenalties *= 18; armorpenalties /= 15; break;
			case MT_MITHRIL: armorpenalties *= 13; armorpenalties /= 15; break;
			case MT_VIVA: armorpenalties *= 12; armorpenalties /= 15; break;
			case MT_POURPOOR: armorpenalties *= 20; armorpenalties /= 15; break;
			case MT_LEAD: armorpenalties *= 25; armorpenalties /= 15; break;
			case MT_CHROME: armorpenalties *= 11; armorpenalties /= 15; break;
		}
		if (Race_if(PM_BOVER)) armorpenalties *= 3;

		if (uwep && (weapon_type(uwep) == P_QUARTERSTAFF || weapon_type(uwep) == P_ORB)) {
			armorpenalties *= 4;
			armorpenalties /= 5;
		}

		if (uarmg && itemhasappearance(uarmg, APP_VELVET_GLOVES) ) armorpenalties /= 2;
		if (uarmh && uarmh->oartifact == ART_METAL_NEGATER) armorpenalties /= 2;

		if (uarm && uarm->blessed && !issoviet) armorpenalties /= 2;

		splcaster += (urole.spelarmr * armorpenalties / (issoviet ? 12 : 30));
	}
	if (SpellColorMetal && (!uarm || !is_metallic(uarm))) {

		if (uwep && (weapon_type(uwep) == P_QUARTERSTAFF || weapon_type(uwep) == P_ORB)) {
			armorpenalties *= 4;
			armorpenalties /= 5;
		}

		if (uarmg && itemhasappearance(uarmg, APP_VELVET_GLOVES) ) armorpenalties /= 2;
		if (uarmh && uarmh->oartifact == ART_METAL_NEGATER) armorpenalties /= 2;

		if (uarm && uarm->blessed && !issoviet) armorpenalties /= 2;

		splcaster += (urole.spelarmr * armorpenalties / (issoviet ? 12 : 30));
	}

	if (uarmc && !SpellColorMetal && (is_metallic(uarmc) || is_filmitem(uarmc)) && !is_etheritem(uarmc) && !is_meteosteelitem(uarmc)) {

		switch (objects[(uarmc)->otyp].oc_material) {
			default: break;
			case MT_METAL: armorpenalties *= 16; armorpenalties /= 15; break;
			case MT_STEEL: armorpenalties *= 16; armorpenalties /= 15; break;
			case MT_GREEN_STEEL: armorpenalties *= 12; armorpenalties /= 15; break;
			case MT_MERCURIAL: armorpenalties *= 3; armorpenalties /= 15; break;
			case MT_BRONZE: armorpenalties *= 21; armorpenalties /= 15; break;
			case MT_TITANIUM: armorpenalties *= 18; armorpenalties /= 15; break;
			case MT_COPPER: armorpenalties *= 17; armorpenalties /= 15; break;
			case MT_COBALT: armorpenalties *= 16; armorpenalties /= 15; break;
			case MT_SILVER: armorpenalties *= 17; armorpenalties /= 15; break;
			case MT_GOLD: armorpenalties *= 8; armorpenalties /= 15; break;
			case MT_TINSEL: armorpenalties *= 10; armorpenalties /= 15; break;
			case MT_PLATINUM: armorpenalties *= 18; armorpenalties /= 15; break;
			case MT_FILM: armorpenalties *= 18; armorpenalties /= 15; break;
			case MT_ALLOY: armorpenalties *= 18; armorpenalties /= 15; break;
			case MT_MITHRIL: armorpenalties *= 13; armorpenalties /= 15; break;
			case MT_VIVA: armorpenalties *= 12; armorpenalties /= 15; break;
			case MT_POURPOOR: armorpenalties *= 20; armorpenalties /= 15; break;
			case MT_LEAD: armorpenalties *= 25; armorpenalties /= 15; break;
			case MT_CHROME: armorpenalties *= 11; armorpenalties /= 15; break;
		}
		if (Race_if(PM_BOVER)) armorpenalties *= 3;

		if (uwep && (weapon_type(uwep) == P_QUARTERSTAFF || weapon_type(uwep) == P_ORB)) {
			armorpenalties *= 4;
			armorpenalties /= 5;
		}

		if (uarmg && itemhasappearance(uarmg, APP_VELVET_GLOVES) ) armorpenalties /= 2;
		if (uarmh && uarmh->oartifact == ART_METAL_NEGATER) armorpenalties /= 2;

		if (uarmc && uarmc->blessed && !issoviet) armorpenalties /= 2;

		splcaster += (urole.spelarmr * armorpenalties / (issoviet ? 36 : 50));
	}
	if (SpellColorMetal && (!uarmc || !is_metallic(uarmc))) {

		if (uwep && (weapon_type(uwep) == P_QUARTERSTAFF || weapon_type(uwep) == P_ORB)) {
			armorpenalties *= 4;
			armorpenalties /= 5;
		}

		if (uarmg && itemhasappearance(uarmg, APP_VELVET_GLOVES) ) armorpenalties /= 2;
		if (uarmh && uarmh->oartifact == ART_METAL_NEGATER) armorpenalties /= 2;

		if (uarmc && uarmc->blessed && !issoviet) armorpenalties /= 2;

		splcaster += (urole.spelarmr * armorpenalties / (issoviet ? 36 : 50));

	}

	if (uarmu && !SpellColorMetal && (is_metallic(uarmu) || is_filmitem(uarmu)) && !is_etheritem(uarmu) && !is_meteosteelitem(uarmu)) {

		switch (objects[(uarmu)->otyp].oc_material) {
			default: break;
			case MT_METAL: armorpenalties *= 16; armorpenalties /= 15; break;
			case MT_STEEL: armorpenalties *= 16; armorpenalties /= 15; break;
			case MT_GREEN_STEEL: armorpenalties *= 12; armorpenalties /= 15; break;
			case MT_MERCURIAL: armorpenalties *= 3; armorpenalties /= 15; break;
			case MT_BRONZE: armorpenalties *= 21; armorpenalties /= 15; break;
			case MT_TITANIUM: armorpenalties *= 18; armorpenalties /= 15; break;
			case MT_COPPER: armorpenalties *= 17; armorpenalties /= 15; break;
			case MT_COBALT: armorpenalties *= 16; armorpenalties /= 15; break;
			case MT_SILVER: armorpenalties *= 17; armorpenalties /= 15; break;
			case MT_GOLD: armorpenalties *= 8; armorpenalties /= 15; break;
			case MT_TINSEL: armorpenalties *= 10; armorpenalties /= 15; break;
			case MT_PLATINUM: armorpenalties *= 18; armorpenalties /= 15; break;
			case MT_FILM: armorpenalties *= 18; armorpenalties /= 15; break;
			case MT_ALLOY: armorpenalties *= 18; armorpenalties /= 15; break;
			case MT_MITHRIL: armorpenalties *= 13; armorpenalties /= 15; break;
			case MT_VIVA: armorpenalties *= 12; armorpenalties /= 15; break;
			case MT_POURPOOR: armorpenalties *= 20; armorpenalties /= 15; break;
			case MT_LEAD: armorpenalties *= 25; armorpenalties /= 15; break;
			case MT_CHROME: armorpenalties *= 11; armorpenalties /= 15; break;
		}
		if (Race_if(PM_BOVER)) armorpenalties *= 3;

		if (uwep && (weapon_type(uwep) == P_QUARTERSTAFF || weapon_type(uwep) == P_ORB)) {
			armorpenalties *= 4;
			armorpenalties /= 5;
		}

		if (uarmg && itemhasappearance(uarmg, APP_VELVET_GLOVES) ) armorpenalties /= 2;
		if (uarmh && uarmh->oartifact == ART_METAL_NEGATER) armorpenalties /= 2;

		if (uarmu && uarmu->blessed && !issoviet) armorpenalties /= 2;

		splcaster += (urole.spelarmr * armorpenalties / (issoviet ? 72 : 100));
	}
	if (SpellColorMetal && (!uarmu || !is_metallic(uarmu))) {

		if (uwep && (weapon_type(uwep) == P_QUARTERSTAFF || weapon_type(uwep) == P_ORB)) {
			armorpenalties *= 4;
			armorpenalties /= 5;
		}

		if (uarmg && itemhasappearance(uarmg, APP_VELVET_GLOVES) ) armorpenalties /= 2;
		if (uarmh && uarmh->oartifact == ART_METAL_NEGATER) armorpenalties /= 2;

		if (uarmu && uarmu->blessed && !issoviet) armorpenalties /= 2;

		splcaster += (urole.spelarmr * armorpenalties / (issoviet ? 72 : 100));

	}

	if (uarms && !SpellColorMetal) {
		if (!is_metallic(uarms) || is_etheritem(uarms) || is_meteosteelitem(uarms)) shieldpenalties /= 3;

		switch (objects[(uarms)->otyp].oc_material) {
			default: break;
			case MT_METAL: shieldpenalties *= 16; shieldpenalties /= 15; break;
			case MT_STEEL: shieldpenalties *= 16; shieldpenalties /= 15; break;
			case MT_GREEN_STEEL: shieldpenalties *= 12; shieldpenalties /= 15; break;
			case MT_MERCURIAL: shieldpenalties *= 3; shieldpenalties /= 15; break;
			case MT_BRONZE: shieldpenalties *= 21; shieldpenalties /= 15; break;
			case MT_TITANIUM: shieldpenalties *= 18; shieldpenalties /= 15; break;
			case MT_COPPER: shieldpenalties *= 17; shieldpenalties /= 15; break;
			case MT_COBALT: shieldpenalties *= 16; shieldpenalties /= 15; break;
			case MT_SILVER: shieldpenalties *= 17; shieldpenalties /= 15; break;
			case MT_GOLD: shieldpenalties *= 8; shieldpenalties /= 15; break;
			case MT_TINSEL: shieldpenalties *= 10; shieldpenalties /= 15; break;
			case MT_PLATINUM: shieldpenalties *= 18; shieldpenalties /= 15; break;
			case MT_FILM: shieldpenalties *= 18; shieldpenalties /= 15; break;
			case MT_ALLOY: shieldpenalties *= 18; shieldpenalties /= 15; break;
			case MT_MITHRIL: shieldpenalties *= 13; shieldpenalties /= 15; break;
			case MT_VIVA: shieldpenalties *= 12; shieldpenalties /= 15; break;
			case MT_POURPOOR: shieldpenalties *= 20; shieldpenalties /= 15; break;
			case MT_LEAD: shieldpenalties *= 25; shieldpenalties /= 15; break;
			case MT_CHROME: shieldpenalties *= 11; shieldpenalties /= 15; break;
		}
		if (Race_if(PM_BOVER)) shieldpenalties *= 3;

		if (uwep && (weapon_type(uwep) == P_QUARTERSTAFF || weapon_type(uwep) == P_ORB)) {
			shieldpenalties *= 4;
			shieldpenalties /= 5;
		}

		if (uarmg && itemhasappearance(uarmg, APP_VELVET_GLOVES) ) shieldpenalties /= 2;
		if (uarmh && uarmh->oartifact == ART_METAL_NEGATER) shieldpenalties /= 2;

		if (uarms && uarms->blessed && !issoviet) shieldpenalties /= 2;

		splcaster += (urole.spelshld * shieldpenalties / (issoviet ? 12 : 30));
	}
	if (SpellColorMetal && (!uarms || !is_metallic(uarms))) {

		if (uwep && (weapon_type(uwep) == P_QUARTERSTAFF || weapon_type(uwep) == P_ORB)) {
			shieldpenalties *= 4;
			shieldpenalties /= 5;
		}

		if (uarmg && itemhasappearance(uarmg, APP_VELVET_GLOVES) ) shieldpenalties /= 2;
		if (uarmh && uarmh->oartifact == ART_METAL_NEGATER) shieldpenalties /= 2;

		if (uarms && uarms->blessed && !issoviet) shieldpenalties /= 2;

		splcaster += (urole.spelshld * shieldpenalties / (issoviet ? 12 : 30));

	}

	if (uarmh && !SpellColorMetal && (is_metallic(uarmh) || is_filmitem(uarmh)) && !is_etheritem(uarmh) && !is_meteosteelitem(uarmh) && uarmh->otyp != HELM_OF_BRILLIANCE) {

		switch (objects[(uarmh)->otyp].oc_material) {
			default: break;
			case MT_METAL: armorpenalties *= 16; armorpenalties /= 15; break;
			case MT_STEEL: armorpenalties *= 16; armorpenalties /= 15; break;
			case MT_GREEN_STEEL: armorpenalties *= 12; armorpenalties /= 15; break;
			case MT_MERCURIAL: armorpenalties *= 3; armorpenalties /= 15; break;
			case MT_BRONZE: armorpenalties *= 21; armorpenalties /= 15; break;
			case MT_TITANIUM: armorpenalties *= 18; armorpenalties /= 15; break;
			case MT_COPPER: armorpenalties *= 17; armorpenalties /= 15; break;
			case MT_COBALT: armorpenalties *= 16; armorpenalties /= 15; break;
			case MT_SILVER: armorpenalties *= 17; armorpenalties /= 15; break;
			case MT_GOLD: armorpenalties *= 8; armorpenalties /= 15; break;
			case MT_TINSEL: armorpenalties *= 10; armorpenalties /= 15; break;
			case MT_PLATINUM: armorpenalties *= 18; armorpenalties /= 15; break;
			case MT_FILM: armorpenalties *= 18; armorpenalties /= 15; break;
			case MT_ALLOY: armorpenalties *= 18; armorpenalties /= 15; break;
			case MT_MITHRIL: armorpenalties *= 13; armorpenalties /= 15; break;
			case MT_VIVA: armorpenalties *= 12; armorpenalties /= 15; break;
			case MT_POURPOOR: armorpenalties *= 20; armorpenalties /= 15; break;
			case MT_LEAD: armorpenalties *= 25; armorpenalties /= 15; break;
			case MT_CHROME: armorpenalties *= 11; armorpenalties /= 15; break;
		}
		if (Race_if(PM_BOVER)) armorpenalties *= 3;

		if (uwep && (weapon_type(uwep) == P_QUARTERSTAFF || weapon_type(uwep) == P_ORB)) {
			armorpenalties *= 4;
			armorpenalties /= 5;
		}

		if (uarmg && itemhasappearance(uarmg, APP_VELVET_GLOVES) ) armorpenalties /= 2;
		if (uarmh && uarmh->oartifact == ART_METAL_NEGATER) armorpenalties /= 2;

		if (uarmh && uarmh->blessed && !issoviet) armorpenalties /= 2;

		splcaster += (uarmhbon * armorpenalties / (issoviet ? 10 : 20));
	}
	if (SpellColorMetal && (!uarmh || !is_metallic(uarmh))) {

		if (uwep && (weapon_type(uwep) == P_QUARTERSTAFF || weapon_type(uwep) == P_ORB)) {
			armorpenalties *= 4;
			armorpenalties /= 5;
		}

		if (uarmg && itemhasappearance(uarmg, APP_VELVET_GLOVES) ) armorpenalties /= 2;
		if (uarmh && uarmh->oartifact == ART_METAL_NEGATER) armorpenalties /= 2;

		if (uarmh && uarmh->blessed && !issoviet) armorpenalties /= 2;

		splcaster += (uarmhbon * armorpenalties / (issoviet ? 10 : 20));

	}

	if (uarmg && !SpellColorMetal && (is_metallic(uarmg) || is_filmitem(uarmg)) && !is_etheritem(uarmg) && !is_meteosteelitem(uarmg)) {

		switch (objects[(uarmg)->otyp].oc_material) {
			default: break;
			case MT_METAL: armorpenalties *= 16; armorpenalties /= 15; break;
			case MT_STEEL: armorpenalties *= 16; armorpenalties /= 15; break;
			case MT_GREEN_STEEL: armorpenalties *= 12; armorpenalties /= 15; break;
			case MT_MERCURIAL: armorpenalties *= 3; armorpenalties /= 15; break;
			case MT_BRONZE: armorpenalties *= 21; armorpenalties /= 15; break;
			case MT_TITANIUM: armorpenalties *= 18; armorpenalties /= 15; break;
			case MT_COPPER: armorpenalties *= 17; armorpenalties /= 15; break;
			case MT_COBALT: armorpenalties *= 16; armorpenalties /= 15; break;
			case MT_SILVER: armorpenalties *= 17; armorpenalties /= 15; break;
			case MT_GOLD: armorpenalties *= 8; armorpenalties /= 15; break;
			case MT_TINSEL: armorpenalties *= 10; armorpenalties /= 15; break;
			case MT_PLATINUM: armorpenalties *= 18; armorpenalties /= 15; break;
			case MT_FILM: armorpenalties *= 18; armorpenalties /= 15; break;
			case MT_ALLOY: armorpenalties *= 18; armorpenalties /= 15; break;
			case MT_MITHRIL: armorpenalties *= 13; armorpenalties /= 15; break;
			case MT_VIVA: armorpenalties *= 12; armorpenalties /= 15; break;
			case MT_POURPOOR: armorpenalties *= 20; armorpenalties /= 15; break;
			case MT_LEAD: armorpenalties *= 25; armorpenalties /= 15; break;
			case MT_CHROME: armorpenalties *= 11; armorpenalties /= 15; break;
		}
		if (Race_if(PM_BOVER)) armorpenalties *= 3;

		if (uwep && (weapon_type(uwep) == P_QUARTERSTAFF || weapon_type(uwep) == P_ORB)) {
			armorpenalties *= 4;
			armorpenalties /= 5;
		}

		if (uarmg && itemhasappearance(uarmg, APP_VELVET_GLOVES) ) armorpenalties /= 2;
		if (uarmh && uarmh->oartifact == ART_METAL_NEGATER) armorpenalties /= 2;

		if (uarmg && uarmg->blessed && !issoviet) armorpenalties /= 2;

		splcaster += (uarmgbon * armorpenalties / (issoviet ? 10 : 20));
	}
	if (SpellColorMetal && (!uarmg || !is_metallic(uarmg))) {

		if (uwep && (weapon_type(uwep) == P_QUARTERSTAFF || weapon_type(uwep) == P_ORB)) {
			armorpenalties *= 4;
			armorpenalties /= 5;
		}

		if (uarmg && itemhasappearance(uarmg, APP_VELVET_GLOVES) ) armorpenalties /= 2;
		if (uarmh && uarmh->oartifact == ART_METAL_NEGATER) armorpenalties /= 2;

		if (uarmg && uarmg->blessed && !issoviet) armorpenalties /= 2;

		splcaster += (uarmgbon * armorpenalties / (issoviet ? 10 : 20));

	}

	if (uarmf && !SpellColorMetal && (is_metallic(uarmf) || is_filmitem(uarmf)) && !is_etheritem(uarmf) && !is_meteosteelitem(uarmf)) {

		switch (objects[(uarmf)->otyp].oc_material) {
			default: break;
			case MT_METAL: armorpenalties *= 16; armorpenalties /= 15; break;
			case MT_STEEL: armorpenalties *= 16; armorpenalties /= 15; break;
			case MT_GREEN_STEEL: armorpenalties *= 12; armorpenalties /= 15; break;
			case MT_MERCURIAL: armorpenalties *= 3; armorpenalties /= 15; break;
			case MT_BRONZE: armorpenalties *= 21; armorpenalties /= 15; break;
			case MT_TITANIUM: armorpenalties *= 18; armorpenalties /= 15; break;
			case MT_COPPER: armorpenalties *= 17; armorpenalties /= 15; break;
			case MT_COBALT: armorpenalties *= 16; armorpenalties /= 15; break;
			case MT_SILVER: armorpenalties *= 17; armorpenalties /= 15; break;
			case MT_GOLD: armorpenalties *= 8; armorpenalties /= 15; break;
			case MT_TINSEL: armorpenalties *= 10; armorpenalties /= 15; break;
			case MT_PLATINUM: armorpenalties *= 18; armorpenalties /= 15; break;
			case MT_FILM: armorpenalties *= 18; armorpenalties /= 15; break;
			case MT_ALLOY: armorpenalties *= 18; armorpenalties /= 15; break;
			case MT_MITHRIL: armorpenalties *= 13; armorpenalties /= 15; break;
			case MT_VIVA: armorpenalties *= 12; armorpenalties /= 15; break;
			case MT_POURPOOR: armorpenalties *= 20; armorpenalties /= 15; break;
			case MT_LEAD: armorpenalties *= 25; armorpenalties /= 15; break;
			case MT_CHROME: armorpenalties *= 11; armorpenalties /= 15; break;
		}
		if (Race_if(PM_BOVER)) armorpenalties *= 3;

		if (uwep && (weapon_type(uwep) == P_QUARTERSTAFF || weapon_type(uwep) == P_ORB)) {
			armorpenalties *= 4;
			armorpenalties /= 5;
		}

		if (uarmg && itemhasappearance(uarmg, APP_VELVET_GLOVES) ) armorpenalties /= 2;
		if (uarmh && uarmh->oartifact == ART_METAL_NEGATER) armorpenalties /= 2;

		if (uarmf && uarmf->blessed && !issoviet) armorpenalties /= 2;

		splcaster += (uarmfbon * armorpenalties / (issoviet ? 10 : 20));
	}

	if (SpellColorMetal && (!uarmf || !is_metallic(uarmf))) {

		if (uwep && (weapon_type(uwep) == P_QUARTERSTAFF || weapon_type(uwep) == P_ORB)) {
			armorpenalties *= 4;
			armorpenalties /= 5;
		}

		if (uarmg && itemhasappearance(uarmg, APP_VELVET_GLOVES) ) armorpenalties /= 2;
		if (uarmh && uarmh->oartifact == ART_METAL_NEGATER) armorpenalties /= 2;

		if (uarmf && uarmf->blessed && !issoviet) armorpenalties /= 2;

		splcaster += (uarmfbon * armorpenalties / (issoviet ? 10 : 20));

	}

	if (spellid(spell) == urole.spelspec)
		splcaster += urole.spelsbon;

	if (spellid(spell) == u.superspecialspell)
		splcaster -= 5;

	/* `healing spell' bonus */
	if (spell_skilltype(spellid(spell)) == P_HEALING_SPELL)
		splcaster += special;

	if (uarm && uarm->otyp == ROBE_OF_POWER) splcaster -= 3;
	if (uarmc && uarmc->otyp == ROBELIKE_CLOAK) splcaster -= 3;
	if (uarm && uarm->oartifact == ART_ROBUN_NUMBER) splcaster -= 3;
	if (uarm && itemhasappearance(uarm, APP_VANILLIC_ROBE)) splcaster -= 3;
	if (uarm && uarm->otyp == ROBE_OF_SPELL_POWER) splcaster -= 3;
	if (uarmg && uarmg->oartifact == ART_GAUNTLETS_OF_SPELL_POWER) splcaster -= 3;
	if (uarm && uarm->oartifact == ART_SPIDERSILK) splcaster -= 3;
	if (uarm && uarm->oartifact == ART_WEB_OF_LOLTH) splcaster -= 3;
	if (uarm && uarm->oartifact == ART_WEB_OF_HOHOTH) splcaster -= 3;
	if (uwep && uwep->oartifact == ART_ARYFAERN_KERYM) splcaster -= 3;
	if (uarmg && uarmg->oartifact == ART_POWERORB_BOOSTING) splcaster -= 3;

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
	if (!Race_if(PM_BABYLONIAN)) {
		if ( skill == P_BASIC) splcaster -= 3;
		if ( skill == P_SKILLED) splcaster -= 6;
		if ( skill == P_EXPERT) splcaster -= 9;
		if ( skill == P_MASTER) splcaster -= 12;
		if ( skill == P_GRAND_MASTER) splcaster -= 15;
		if ( skill == P_SUPREME_MASTER) splcaster -= 18;
	}

	/* casting it often (and thereby keeping it in memory) should also improve chances...
	 * Amy note: 700% is the cap where it stops getting better, this is intentional */
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

	if (splcaster < 0) {
		splcaster /= 5;
		if (splcaster > 0) splcaster = 0; /* fail safe */
		if (splcaster < -10) splcaster = -10; /* absolute limit */
	}
	if (splcaster > 50) splcaster = 50;

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

	if ( (CasterProblem || u.uprops[CASTER_PROBLEM].extrinsic || have_antimagicstone() || (uarm && uarm->oartifact == ART_HLOVAL_S_PROBLEM) || (uarm && uarm->oartifact == ART_ROBE_OF_THE_ARCHMAGI) || (uarmc && uarmc->oartifact == ART_HERETICAL_FIGURE) || (uarm && uarm->oartifact == ART_SHIVANHUNTER_S_UNUSED_PRIZ) ) && chance > 0) {

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
			chance -= 32;
			break;
		case 7:
			chance -= 70;
			break;
		case 8:
			chance -= 125;
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

	if (Race_if(PM_TONBERRY)) {
		switch (spellev(spell)) {
			case 1:
				chance -= 10;
				break;
			case 2:
				chance -= 20;
				break;
			case 3:
				chance -= 30;
				break;
			case 4:
				chance -= 40;
				break;
			case 5:
				chance -= 50;
				break;
			case 6:
				chance -= 60;
				break;
			case 7:
				chance -= 70;
				break;
			case 8:
				chance -= 80;
				break;

		}
	}

	if (issoviet) chance -= 30;
	if (Role_if(PM_NOOB_MODE_BARB)) chance -= 50;
	if (u.martialstyle == MARTIALSTYLE_MUAYTHAI) chance -= 20;

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
				chance -= 58;
				if (!Role_if(PM_CHAOS_SORCEROR)) chance -= 60;
				break;
			case 7:
				chance -= 70;
				if (!Role_if(PM_CHAOS_SORCEROR)) chance -= 80;
				break;
			case 8:
				chance -= 75;
				if (!Role_if(PM_CHAOS_SORCEROR)) chance -= 100;
				break;

		}

	}

	if (uarmg && itemhasappearance(uarmg, APP_UNCANNY_GLOVES)) chance -= 10;

	if (uarmh && uarmh->oartifact == ART_WAITING_FOR_MELEE) chance -= 15;
	if (uarm && uarm->oartifact == ART_DRAGON_PLATE) chance -= 20;
	if (uarmf && uarmf->oartifact == ART_THICKER_THAN_THE_HEAD) chance -= 20;
	if (uarm && uarm->oartifact == ART_MADE_OF_IRON) chance -= 20;
	if (uarms && uarms->oartifact == ART_HEAVE_FIELD) chance -= 20;
	if (uarmh && uarmh->oartifact == ART_FREDERIK_S_COMBAT_HELMET) chance -= 15;
	if (uarmh && uarmh->oartifact == ART_FRIEDRICH_S_BATTLE_VISOR) chance -= 20;
	if (StrongMagicVacuum) chance -= 50;

	if (Race_if(PM_PLAYER_GOLEM)) {
		chance -= 25;
		if (spellev(spell) > 1) chance -= ((spellev(spell) - 1) * 5);
	}

	if (uarm && uarm->oartifact == ART_ROFLCOPTER_WEB) chance += 10;
	if (uarm && uarm->otyp == ROBE_OF_SPELL_POWER) chance += 20;
	if (Role_if(PM_CELLAR_CHILD) && uarm && uarm->otyp == MAGE_PLATE_MAIL) chance += 20;
	if (uarmh && uarmh->oartifact == ART_ZERO_PERCENT_FAILURE) chance += 10;
	if (uarmf && uarmf->oartifact == ART_SPELLBOOTS) chance += 10;
	if (uarmc && uarmc->oartifact == ART_HENRIETTA_S_HEAVY_CASTER) chance += 15;
	if (uarm && uarm->oartifact == ART_OAR_SUPER_GRAPHICS_CARD_BU) chance += 15;
	if (uarm && uarm->oartifact == ART_HELIOKOPIS_S_RAGEQUIT) chance += 20;
	if (have_albiestone()) chance += 100;
	if (uarmf && uarmf->oartifact == ART_ROWALLAN_S_BALLING) chance += 10;
	if (uarmg && uarmg->oartifact == ART_ERASMUS__SCRIPTED_GAUNTLET) chance += 30;
	if (uimplant && uimplant->oartifact == ART_SPINAL_MATRIX_OF_RAISTLIN) chance += 20;
	if (uimplant && uimplant->oartifact == ART_SYNAPSE_CROWN_OF_GANDALF) chance += 15;
	if (uarm && uarm->oartifact == ART_ROBXOR_ZSCH) chance += 20;
	if (uarm && uarm->oartifact == ART_PAUA_FOR_ME_) chance += 12;
	if (u.tempspellchance20) chance += 20;
	if (uarmh && uarmh->oartifact == ART_SPELLSPELLSPELL___) {
		chance += 20;
		if (Role_if(PM_CELLAR_CHILD)) chance += 20;
	}
	if (uarm && uarm->oartifact == ART_FAMPLATE) chance += 20;
	if (uarm && uarm->oartifact == ART_FINICKYMAG) chance += 15;
	if (uarmf && uarmf->oartifact == ART_SUNALI_S_SUMMONING_STORM) chance += 15;
	if (uwep && uwep->otyp == OLDEST_STAFF) chance += 10;
	if (uleft && uleft->oartifact == ART_HENRIETTA_S_MAGICAL_AID) chance += 50;
	if (uright && uright->oartifact == ART_HENRIETTA_S_MAGICAL_AID) chance += 50;
	if (uarmu && uarmu->oartifact == ART_KEITH_S_UNDEROOS) chance += 50;
	if (Role_if(PM_ARCHEOLOGIST) && uamul && uamul->oartifact == ART_ARCHEOLOGIST_SONG) chance += 10;
	if (uarmh && itemhasappearance(uarmh, APP_KNOWLEDGEABLE_HELMET) ) chance += 10;
	if (uarmc && itemhasappearance(uarmc, APP_SCIENCE_CLOAK) ) chance += 10;
	if (u.tiksrvzllatdown) chance += 10;
	if (uarmf && uarmf->oartifact == ART_JONADAB_S_EVERYDAY_WEAR) chance += 5;
	if (uarmf && uarmf->oartifact == ART_DON_T_FALL_INTO_THE_ABYSS) chance += 10;
	if (uwep && uwep->oartifact == ART_BAOBHAN_MOUNTAIN) chance += 10;
	if (uleft && uleft->oartifact == ART_SPELLER_OUT) chance += 10;
	if (uright && uright->oartifact == ART_SPELLER_OUT) chance += 10;
	if (uwep && uwep->oartifact == ART_ORIGIN_OF_CASTING) chance += 20;
	if (uwep && uwep->oartifact == ART_HELIOKOPIS_S_PLAYSTYLE) chance += 20;
	if (uarmg && uarmg->oartifact == ART_FLOEMMELFLOEMMELFLOEMMELFL) chance += 5;
	if (uarmc && uarmc->oartifact == ART_CASTEASY) chance += 5;
	if (uwep && uwep->oartifact == ART_INFASPEL) chance += 5;
	if (uarmg && uarmg->oartifact == ART_HOGWILD_WIZ) chance += 10;
	if (bmwride(ART_ZIN_BA)) chance += 10;
	if (uarmg && uarmg->oartifact == ART_WAND_INTO_SPELL) chance += 10;
	if (uarmc && uarmc->oartifact == ART_SHROUD) chance += 10;
	if (have_magicrystal()) chance += 5;
	if (uarmh && uarmh->oartifact == ART_ALL_THE_BETTER_SPELLING) chance += 5;
	if (uarm && uarm->oartifact == ART_KAMAKURA_BORI) chance += 20;
	if (uarmh && uarmh->oartifact == ART_DREHTURN) chance += 10;

	if (spell_skilltype(spellid(spell)) == P_HEALING_SPELL) {
		if (uwep && uwep->oartifact == ART_CELESTIAL_SCEPTRE) chance += 50;
	}

	if (uwep && uwep->oartifact == ART_I_WANT_IT_DEAD_AWAY && (spellid(spell) == SPE_DISINTEGRATION) ) chance += 50;

	/* higher spell skill should do SOMEthing --Amy */
	skill = P_SKILL(spell_skilltype(spellid(spell)));
	if (PlayerCannotUseSkills) skill = P_ISRESTRICTED;

	if (!Race_if(PM_BABYLONIAN)) {
		if (skill >= P_BASIC) chance++;
		if (skill >= P_SKILLED) chance++;
		if (skill >= P_EXPERT) chance++;
		if (skill >= P_MASTER) chance += 5;
		if (skill >= P_GRAND_MASTER) chance += 5;
		if (skill >= P_SUPREME_MASTER) chance += 5;
	}

	if (uarm && objects[(uarm)->otyp].oc_material == MT_CELESTIUM) chance += 2;
	if (uarmu && objects[(uarmu)->otyp].oc_material == MT_CELESTIUM) chance += 2;
	if (uarmc && objects[(uarmc)->otyp].oc_material == MT_CELESTIUM) chance += 2;
	if (uarmf && objects[(uarmf)->otyp].oc_material == MT_CELESTIUM) chance += 2;
	if (uarmg && objects[(uarmg)->otyp].oc_material == MT_CELESTIUM) chance += 2;
	if (uarmh && objects[(uarmh)->otyp].oc_material == MT_CELESTIUM) chance += 2;
	if (uarms && objects[(uarms)->otyp].oc_material == MT_CELESTIUM) chance += 2;
	if (uwep && objects[(uwep)->otyp].oc_material == MT_CELESTIUM) chance += 2;
	if (u.twoweap && uswapwep && objects[(uswapwep)->otyp].oc_material == MT_CELESTIUM) chance += 2;
	if (uamul && objects[(uamul)->otyp].oc_material == MT_CELESTIUM) chance += 2;
	if (uleft && objects[(uleft)->otyp].oc_material == MT_CELESTIUM) chance += 2;
	if (uright && objects[(uright)->otyp].oc_material == MT_CELESTIUM) chance += 2;
	if (uimplant && objects[(uimplant)->otyp].oc_material == MT_CELESTIUM) chance += 2;
	if (ublindf && objects[(ublindf)->otyp].oc_material == MT_CELESTIUM) chance += 2;

	if (Race_if(PM_PLAYER_FAIRY)) {
		chance += 33;

		if (uwep && is_metallic(uwep) && !is_etheritem(uwep) && !is_meteosteelitem(uwep)) {

			switch (objects[(uwep)->otyp].oc_material) {
				default: chance -= 20; break;
				case MT_METAL: chance -= 22; break;
				case MT_STEEL: chance -= 22; break;
				case MT_GREEN_STEEL: chance -= 16; break;
				case MT_MERCURIAL: chance -= 4; break;
				case MT_BRONZE: chance -= 28; break;
				case MT_TITANIUM: chance -= 26; break;
				case MT_COPPER: chance -= 24; break;
				case MT_COBALT: chance -= 22; break;
				case MT_SILVER: chance -= 24; break;
				case MT_GOLD: chance -= 10; break;
				case MT_TINSEL: chance -= 12; break;
				case MT_PLATINUM: chance -= 26; break;
				case MT_FILM: chance -= 26; break;
				case MT_ALLOY: chance -= 26; break;
				case MT_MITHRIL: chance -= 18; break;
				case MT_VIVA: chance -= 16; break;
				case MT_POURPOOR: chance -= 30; break;
				case MT_LEAD: chance -= 40; break;
				case MT_CHROME: chance -= 14; break;
			}
		}

		if (u.twoweap && uswapwep && is_metallic(uswapwep) && !is_etheritem(uswapwep) && !is_meteosteelitem(uswapwep)) {

			switch (objects[(uswapwep)->otyp].oc_material) {
				default: chance -= 20; break;
				case MT_METAL: chance -= 22; break;
				case MT_STEEL: chance -= 22; break;
				case MT_GREEN_STEEL: chance -= 16; break;
				case MT_MERCURIAL: chance -= 4; break;
				case MT_BRONZE: chance -= 28; break;
				case MT_TITANIUM: chance -= 26; break;
				case MT_COPPER: chance -= 24; break;
				case MT_COBALT: chance -= 22; break;
				case MT_SILVER: chance -= 24; break;
				case MT_GOLD: chance -= 10; break;
				case MT_TINSEL: chance -= 12; break;
				case MT_PLATINUM: chance -= 26; break;
				case MT_FILM: chance -= 26; break;
				case MT_ALLOY: chance -= 26; break;
				case MT_MITHRIL: chance -= 18; break;
				case MT_VIVA: chance -= 16; break;
				case MT_POURPOOR: chance -= 30; break;
				case MT_LEAD: chance -= 40; break;
				case MT_CHROME: chance -= 14; break;
			}
		}

		if (uarm && is_metallic(uarm) && !is_etheritem(uarm) && !is_meteosteelitem(uarm)) {

			switch (objects[(uarm)->otyp].oc_material) {
				default: chance -= 20; break;
				case MT_METAL: chance -= 22; break;
				case MT_STEEL: chance -= 22; break;
				case MT_GREEN_STEEL: chance -= 16; break;
				case MT_MERCURIAL: chance -= 4; break;
				case MT_BRONZE: chance -= 28; break;
				case MT_TITANIUM: chance -= 26; break;
				case MT_COPPER: chance -= 24; break;
				case MT_COBALT: chance -= 22; break;
				case MT_SILVER: chance -= 24; break;
				case MT_GOLD: chance -= 10; break;
				case MT_TINSEL: chance -= 12; break;
				case MT_PLATINUM: chance -= 26; break;
				case MT_FILM: chance -= 26; break;
				case MT_ALLOY: chance -= 26; break;
				case MT_MITHRIL: chance -= 18; break;
				case MT_VIVA: chance -= 16; break;
				case MT_POURPOOR: chance -= 30; break;
				case MT_LEAD: chance -= 40; break;
				case MT_CHROME: chance -= 14; break;
			}
		}

		if (uarmc && is_metallic(uarmc) && !is_etheritem(uarmc) && !is_meteosteelitem(uarmc)) {

			switch (objects[(uarmc)->otyp].oc_material) {
				default: chance -= 20; break;
				case MT_METAL: chance -= 22; break;
				case MT_STEEL: chance -= 22; break;
				case MT_GREEN_STEEL: chance -= 16; break;
				case MT_MERCURIAL: chance -= 4; break;
				case MT_BRONZE: chance -= 28; break;
				case MT_TITANIUM: chance -= 26; break;
				case MT_COPPER: chance -= 24; break;
				case MT_COBALT: chance -= 22; break;
				case MT_SILVER: chance -= 24; break;
				case MT_GOLD: chance -= 10; break;
				case MT_TINSEL: chance -= 12; break;
				case MT_PLATINUM: chance -= 26; break;
				case MT_FILM: chance -= 26; break;
				case MT_ALLOY: chance -= 26; break;
				case MT_MITHRIL: chance -= 18; break;
				case MT_VIVA: chance -= 16; break;
				case MT_POURPOOR: chance -= 30; break;
				case MT_LEAD: chance -= 40; break;
				case MT_CHROME: chance -= 14; break;
			}
		}

		if (uarmh && is_metallic(uarmh) && !is_etheritem(uarmh) && !is_meteosteelitem(uarmh)) {

			switch (objects[(uarmh)->otyp].oc_material) {
				default: chance -= 20; break;
				case MT_METAL: chance -= 22; break;
				case MT_STEEL: chance -= 22; break;
				case MT_GREEN_STEEL: chance -= 16; break;
				case MT_MERCURIAL: chance -= 4; break;
				case MT_BRONZE: chance -= 28; break;
				case MT_TITANIUM: chance -= 26; break;
				case MT_COPPER: chance -= 24; break;
				case MT_COBALT: chance -= 22; break;
				case MT_SILVER: chance -= 24; break;
				case MT_GOLD: chance -= 10; break;
				case MT_TINSEL: chance -= 12; break;
				case MT_PLATINUM: chance -= 26; break;
				case MT_FILM: chance -= 26; break;
				case MT_ALLOY: chance -= 26; break;
				case MT_MITHRIL: chance -= 18; break;
				case MT_VIVA: chance -= 16; break;
				case MT_POURPOOR: chance -= 30; break;
				case MT_LEAD: chance -= 40; break;
				case MT_CHROME: chance -= 14; break;
			}
		}

		if (uarms && is_metallic(uarms) && !is_etheritem(uarms) && !is_meteosteelitem(uarms)) {

			switch (objects[(uarms)->otyp].oc_material) {
				default: chance -= 20; break;
				case MT_METAL: chance -= 22; break;
				case MT_STEEL: chance -= 22; break;
				case MT_GREEN_STEEL: chance -= 16; break;
				case MT_MERCURIAL: chance -= 4; break;
				case MT_BRONZE: chance -= 28; break;
				case MT_TITANIUM: chance -= 26; break;
				case MT_COPPER: chance -= 24; break;
				case MT_COBALT: chance -= 22; break;
				case MT_SILVER: chance -= 24; break;
				case MT_GOLD: chance -= 10; break;
				case MT_TINSEL: chance -= 12; break;
				case MT_PLATINUM: chance -= 26; break;
				case MT_FILM: chance -= 26; break;
				case MT_ALLOY: chance -= 26; break;
				case MT_MITHRIL: chance -= 18; break;
				case MT_VIVA: chance -= 16; break;
				case MT_POURPOOR: chance -= 30; break;
				case MT_LEAD: chance -= 40; break;
				case MT_CHROME: chance -= 14; break;
			}
		}

		if (uarmg && is_metallic(uarmg) && !is_etheritem(uarmg) && !is_meteosteelitem(uarmg)) {

			switch (objects[(uarmg)->otyp].oc_material) {
				default: chance -= 20; break;
				case MT_METAL: chance -= 22; break;
				case MT_STEEL: chance -= 22; break;
				case MT_GREEN_STEEL: chance -= 16; break;
				case MT_MERCURIAL: chance -= 4; break;
				case MT_BRONZE: chance -= 28; break;
				case MT_TITANIUM: chance -= 26; break;
				case MT_COPPER: chance -= 24; break;
				case MT_COBALT: chance -= 22; break;
				case MT_SILVER: chance -= 24; break;
				case MT_GOLD: chance -= 10; break;
				case MT_TINSEL: chance -= 12; break;
				case MT_PLATINUM: chance -= 26; break;
				case MT_FILM: chance -= 26; break;
				case MT_ALLOY: chance -= 26; break;
				case MT_MITHRIL: chance -= 18; break;
				case MT_VIVA: chance -= 16; break;
				case MT_POURPOOR: chance -= 30; break;
				case MT_LEAD: chance -= 40; break;
				case MT_CHROME: chance -= 14; break;
			}
		}

		if (uarmf && is_metallic(uarmf) && !is_etheritem(uarmf) && !is_meteosteelitem(uarmf)) {

			switch (objects[(uarmf)->otyp].oc_material) {
				default: chance -= 20; break;
				case MT_METAL: chance -= 22; break;
				case MT_STEEL: chance -= 22; break;
				case MT_GREEN_STEEL: chance -= 16; break;
				case MT_MERCURIAL: chance -= 4; break;
				case MT_BRONZE: chance -= 28; break;
				case MT_TITANIUM: chance -= 26; break;
				case MT_COPPER: chance -= 24; break;
				case MT_COBALT: chance -= 22; break;
				case MT_SILVER: chance -= 24; break;
				case MT_GOLD: chance -= 10; break;
				case MT_TINSEL: chance -= 12; break;
				case MT_PLATINUM: chance -= 26; break;
				case MT_FILM: chance -= 26; break;
				case MT_ALLOY: chance -= 26; break;
				case MT_MITHRIL: chance -= 18; break;
				case MT_VIVA: chance -= 16; break;
				case MT_POURPOOR: chance -= 30; break;
				case MT_LEAD: chance -= 40; break;
				case MT_CHROME: chance -= 14; break;
			}
		}

		if (uarmu && is_metallic(uarmu) && !is_etheritem(uarmu) && !is_meteosteelitem(uarmu)) {

			switch (objects[(uarmu)->otyp].oc_material) {
				default: chance -= 20; break;
				case MT_METAL: chance -= 22; break;
				case MT_STEEL: chance -= 22; break;
				case MT_GREEN_STEEL: chance -= 16; break;
				case MT_MERCURIAL: chance -= 4; break;
				case MT_BRONZE: chance -= 28; break;
				case MT_TITANIUM: chance -= 26; break;
				case MT_COPPER: chance -= 24; break;
				case MT_COBALT: chance -= 22; break;
				case MT_SILVER: chance -= 24; break;
				case MT_GOLD: chance -= 10; break;
				case MT_TINSEL: chance -= 12; break;
				case MT_PLATINUM: chance -= 26; break;
				case MT_FILM: chance -= 26; break;
				case MT_ALLOY: chance -= 26; break;
				case MT_MITHRIL: chance -= 18; break;
				case MT_VIVA: chance -= 16; break;
				case MT_POURPOOR: chance -= 30; break;
				case MT_LEAD: chance -= 40; break;
				case MT_CHROME: chance -= 14; break;
			}
		}

		if (uamul && is_metallic(uamul) && !is_etheritem(uamul) && !is_meteosteelitem(uamul)) {

			switch (objects[(uamul)->otyp].oc_material) {
				default: chance -= 20; break;
				case MT_METAL: chance -= 22; break;
				case MT_STEEL: chance -= 22; break;
				case MT_GREEN_STEEL: chance -= 16; break;
				case MT_MERCURIAL: chance -= 4; break;
				case MT_BRONZE: chance -= 28; break;
				case MT_TITANIUM: chance -= 26; break;
				case MT_COPPER: chance -= 24; break;
				case MT_COBALT: chance -= 22; break;
				case MT_SILVER: chance -= 24; break;
				case MT_GOLD: chance -= 10; break;
				case MT_TINSEL: chance -= 12; break;
				case MT_PLATINUM: chance -= 26; break;
				case MT_FILM: chance -= 26; break;
				case MT_ALLOY: chance -= 26; break;
				case MT_MITHRIL: chance -= 18; break;
				case MT_VIVA: chance -= 16; break;
				case MT_POURPOOR: chance -= 30; break;
				case MT_LEAD: chance -= 40; break;
				case MT_CHROME: chance -= 14; break;
			}
		}

		if (uimplant && is_metallic(uimplant) && !is_etheritem(uimplant) && !is_meteosteelitem(uimplant)) {

			switch (objects[(uimplant)->otyp].oc_material) {
				default: chance -= 20; break;
				case MT_METAL: chance -= 22; break;
				case MT_STEEL: chance -= 22; break;
				case MT_GREEN_STEEL: chance -= 16; break;
				case MT_MERCURIAL: chance -= 4; break;
				case MT_BRONZE: chance -= 28; break;
				case MT_TITANIUM: chance -= 26; break;
				case MT_COPPER: chance -= 24; break;
				case MT_COBALT: chance -= 22; break;
				case MT_SILVER: chance -= 24; break;
				case MT_GOLD: chance -= 10; break;
				case MT_TINSEL: chance -= 12; break;
				case MT_PLATINUM: chance -= 26; break;
				case MT_FILM: chance -= 26; break;
				case MT_ALLOY: chance -= 26; break;
				case MT_MITHRIL: chance -= 18; break;
				case MT_VIVA: chance -= 16; break;
				case MT_POURPOOR: chance -= 30; break;
				case MT_LEAD: chance -= 40; break;
				case MT_CHROME: chance -= 14; break;
			}
		}

		if (uleft && is_metallic(uleft) && !is_etheritem(uleft) && !is_meteosteelitem(uleft)) {

			switch (objects[(uleft)->otyp].oc_material) {
				default: chance -= 20; break;
				case MT_METAL: chance -= 22; break;
				case MT_STEEL: chance -= 22; break;
				case MT_GREEN_STEEL: chance -= 16; break;
				case MT_MERCURIAL: chance -= 4; break;
				case MT_BRONZE: chance -= 28; break;
				case MT_TITANIUM: chance -= 26; break;
				case MT_COPPER: chance -= 24; break;
				case MT_COBALT: chance -= 22; break;
				case MT_SILVER: chance -= 24; break;
				case MT_GOLD: chance -= 10; break;
				case MT_TINSEL: chance -= 12; break;
				case MT_PLATINUM: chance -= 26; break;
				case MT_FILM: chance -= 26; break;
				case MT_ALLOY: chance -= 26; break;
				case MT_MITHRIL: chance -= 18; break;
				case MT_VIVA: chance -= 16; break;
				case MT_POURPOOR: chance -= 30; break;
				case MT_LEAD: chance -= 40; break;
				case MT_CHROME: chance -= 14; break;
			}
		}

		if (uright && is_metallic(uright) && !is_etheritem(uright) && !is_meteosteelitem(uright)) {

			switch (objects[(uright)->otyp].oc_material) {
				default: chance -= 20; break;
				case MT_METAL: chance -= 22; break;
				case MT_STEEL: chance -= 22; break;
				case MT_GREEN_STEEL: chance -= 16; break;
				case MT_MERCURIAL: chance -= 4; break;
				case MT_BRONZE: chance -= 28; break;
				case MT_TITANIUM: chance -= 26; break;
				case MT_COPPER: chance -= 24; break;
				case MT_COBALT: chance -= 22; break;
				case MT_SILVER: chance -= 24; break;
				case MT_GOLD: chance -= 10; break;
				case MT_TINSEL: chance -= 12; break;
				case MT_PLATINUM: chance -= 26; break;
				case MT_FILM: chance -= 26; break;
				case MT_ALLOY: chance -= 26; break;
				case MT_MITHRIL: chance -= 18; break;
				case MT_VIVA: chance -= 16; break;
				case MT_POURPOOR: chance -= 30; break;
				case MT_LEAD: chance -= 40; break;
				case MT_CHROME: chance -= 14; break;
			}
		}

		if (ublindf && is_metallic(ublindf) && !is_etheritem(ublindf) && !is_meteosteelitem(ublindf)) {

			switch (objects[(ublindf)->otyp].oc_material) {
				default: chance -= 20; break;
				case MT_METAL: chance -= 22; break;
				case MT_STEEL: chance -= 22; break;
				case MT_GREEN_STEEL: chance -= 16; break;
				case MT_MERCURIAL: chance -= 4; break;
				case MT_BRONZE: chance -= 28; break;
				case MT_TITANIUM: chance -= 26; break;
				case MT_COPPER: chance -= 24; break;
				case MT_COBALT: chance -= 22; break;
				case MT_SILVER: chance -= 24; break;
				case MT_GOLD: chance -= 10; break;
				case MT_TINSEL: chance -= 12; break;
				case MT_PLATINUM: chance -= 26; break;
				case MT_FILM: chance -= 26; break;
				case MT_ALLOY: chance -= 26; break;
				case MT_MITHRIL: chance -= 18; break;
				case MT_VIVA: chance -= 16; break;
				case MT_POURPOOR: chance -= 30; break;
				case MT_LEAD: chance -= 40; break;
				case MT_CHROME: chance -= 14; break;
			}
		}

	}

	if (Upolyd && dmgtype(youmonst.data, AD_SPEL) ) {
		chance += 5;
	}
	if (Upolyd && dmgtype(youmonst.data, AD_CLRC) ) {
		chance += 5;
	}
	if (Upolyd && dmgtype(youmonst.data, AD_CAST) ) {
		chance += 10;
	}

	if (!PlayerCannotUseSkills && uwep && is_lightsaber(uwep) && (uwep->lamplit || Role_if(PM_SHADOW_JEDI)) ) {
		switch (P_SKILL(P_NIMAN)) {
			default: break;
			case P_BASIC: chance += 5; break;
			case P_SKILLED: chance += 10; break;
			case P_EXPERT: chance += 15; break;
			case P_MASTER: chance += 20; break;
			case P_GRAND_MASTER: chance += 25; break;
			case P_SUPREME_MASTER: chance += 30; break;
		}

	}

	if (!PlayerCannotUseSkills) {
		switch (P_SKILL(P_NIMAN)) {
			default: break;
			case P_BASIC: chance += 1; break;
			case P_SKILLED: chance += 2; break;
			case P_EXPERT: chance += 3; break;
			case P_MASTER: chance += 4; break;
			case P_GRAND_MASTER: chance += 5; break;
			case P_SUPREME_MASTER: chance += 6; break;
		}

	}

	if (Race_if(PM_INKA) && spellid(spell) == SPE_NATURE_BEAM)
		chance += 100;

	if (Race_if(PM_VIETIS)) chance -= 20;

	if (uimplant && uimplant->oartifact == ART_COMBAT_NODE_OF_THARION_BLA) chance -= 10;
	if (uarmc && itemhasappearance(uarmc, APP_DNETHACK_CLOAK)) chance -= 10;
	if (uarms && uarms->oartifact == ART_DNETHACKC_NOONISHNESS) chance -= 10;
	if (RngeDnethack) chance -= 10;
	if (Race_if(PM_INHERITOR)) chance -= 10;
	if (RngeUnnethack) chance -= 33;
	if (ACURR(A_INT) < 3) chance -= 25;

	/* artifacts and other items that boost the chance after "hard" penalties are applied go here --Amy */

	if (uarmc && itemhasappearance(uarmc, APP_FAILUNCAP_CLOAK) ) chance += 5;
	if (uimplant && uimplant->oartifact == ART_CRANIAL_PRISM_OF_MERLIN) chance += 10;
	if (uarm && uarm->oartifact == ART_ROBXOR_ZSCH) chance += 5;
	if (powerfulimplants() && uimplant && uimplant->oartifact == ART_IMANI_S_POINT) chance += 10;
	if (uarmh && itemhasappearance(uarmh, APP_FAILUNCAP_HELMET) ) chance += 5;
	if (uarmg && itemhasappearance(uarmg, APP_FAILUNCAP_GLOVES) ) chance += 5;
	if (uarmf && itemhasappearance(uarmf, APP_FAILUNCAP_SHOES) ) chance += 5;
	if (uarm && uarm->oartifact == ART_ALTADOON_HERMA_MORA) chance += 20;
	if (uarmg && uarmg->oartifact == ART_HOGWILD_WIZ) chance += 5;
	if (uarmg && uarmg->oartifact == ART_WAND_INTO_SPELL) chance += 10;
	if (have_magicrystal()) chance += 5;
	if (uarmh && uarmh->oartifact == ART_ALL_THE_BETTER_SPELLING) chance += 5;

	if (uwep && uwep->oartifact == ART_RASSCHEN_TAAK) chance += 5;

	if (Role_if(PM_OTAKU) && uarmc && itemhasappearance(uarmc, APP_FOURCHAN_CLOAK)) chance += 5;

	/* very high INT gives a boost, even if your casting stat is WIS --Amy */
	if (ACURR(A_INT) >= 16) {
		chance += (ACURR(A_INT) - 15);
	}

	if (uarm && uarm->oartifact == ART_MOTHERFUCKER_TROPHY) {
		chance += 20;
	}

	if (uarmf && uarmf->oartifact == ART_MY_DEAR_SLAVE && chance > 0) chance *= 2;

	/* "bullshit change" by Amy: make it quite a bit harder to get to 0% fail, because spells are generally easier to
	 * cast compared to vanilla which results in difficult spells being too easy for non-caster roles
	 * THIS MUST COME AFTER ANYTHING THAT BOOSTS THE CHANCE */
	if (chance > 50) {
		int chancediff = (chance - 50);
		chancediff /= 2;
		if (chancediff > 50) chancediff = 50;
		chance -= chancediff;

		if (chance > 90) {
			int chancediff = (chance - 90);
			chancediff *= 9;
			chancediff /= 10;
			if (chancediff > 90) chancediff = 90;
			chance -= chancediff;
		}
	}

	/* Clamp to percentile */
	if (chance > 100) chance = 100;

	if (RememberanceEffect || u.uprops[REMEMBERANCE_EFFECT].extrinsic || have_rememberancestone()) {
		if (chance > (spellknow(spell) / 100)) chance = (spellknow(spell) / 100);
	}

	if (uarmc && itemhasappearance(uarmc, APP_SHELL_CLOAK) ) chance -= 20;

	if (is_grassland(u.ux, u.uy) && !(uamul && uamul->oartifact == ART_CONTRO_MOVE) && !(uarm && uarm->oartifact == ART_OBERTURT) && !(uarms && uarms->oartifact == ART_FLORENSE_S_GEBLOOMEL) && !(uarm && uarm->oartifact == ART_DORL_TSCH) && !(uarmf && itemhasappearance(uarmf, APP_GARDEN_SLIPPERS))) chance -= 10;
	if (Numbed) chance -= 10;

	if (Role_if(PM_FAILED_EXISTENCE)) chance /= 2; /* at least 50% fail for all spells */
	if (uarms && uarms->otyp == COMPLETE_BLOCKAGE_SHIELD) chance /= 2;
	if (uarmc && uarmc->oartifact == ART_ARTIFICIAL_FAKE_DIFFICULTY) {
		chance *= 5;
		chance /= 6;
	}
	if (ACURR(A_INT) == 1) { /* 25% autofail if you're as stupid as possible */
		chance *= 3;
		chance /= 4;
	}

	if (Race_if(PM_INHERITOR)) chance--;

	/* REALLY clamp chance now */
	if (chance > 100) chance = 100;

	if (SpellColorSilver) {

		if (chance == 50) chance = 100;
		else if (chance == 100) chance = 10;
		else if (chance > 50) chance -= ((chance - 50) * 2);
		else if (chance > 0) chance *= 2;

	}

	if (chance < (issoviet ? 0 : (spellev(spell) == 8) ? 0 : (spellev(spell) == 7) ? 1 : (spellev(spell) == 6) ? 2 : (spellev(spell) == 5) ? 5 : 10) ) chance = (issoviet ? 0 : (spellev(spell) == 8) ? 0 : (spellev(spell) == 7) ? 1 : (spellev(spell) == 6) ? 2 : (spellev(spell) == 5) ? 5 : 10); /* used to be 0, but that was just stupid in my opinion --Amy */

	return chance;
}

/* increase memory of a spell: now a function; inspired by Elona, the increase is now lower if your current spell memory
 * is already rather high. We're trying to make the decrease not be too harsh at first, but 2000% spell memory should be
 * extremely difficult to reach. */
void
boostknow(spell,boost)
int spell, boost;
{
	int priorknow = spl_book[spell].sp_know;
	int boostmultiplier = 100;
	if (priorknow > 190100) boostmultiplier = 1;
	else if (priorknow > 180100) boostmultiplier = 2;
	else if (priorknow > 170100) boostmultiplier = 3;
	else if (priorknow > 160100) boostmultiplier = 4;
	else if (priorknow > 150100) boostmultiplier = 5;
	else if (priorknow > 140100) boostmultiplier = 6;
	else if (priorknow > 130100) boostmultiplier = 7;
	else if (priorknow > 120100) boostmultiplier = 8;
	else if (priorknow > 110100) boostmultiplier = 9;
	else if (priorknow > 100100) boostmultiplier = 10;
	else if (priorknow > 90100) boostmultiplier = 15;
	else if (priorknow > 80100) boostmultiplier = 20;
	else if (priorknow > 70100) boostmultiplier = 30;
	else if (priorknow > 60100) boostmultiplier = 40;
	else if (priorknow > 50100) boostmultiplier = 50;
	else if (priorknow > 40100) boostmultiplier = 60;
	else if (priorknow > 30100) boostmultiplier = 70;
	else if (priorknow > 20100) boostmultiplier = 80;
	else if (priorknow > 10100) boostmultiplier = 90;

	boost *= boostmultiplier;
	boost /= 100;

	if (MagicVacuum) {
		boost /= (StrongMagicVacuum) ? 5 : 2;
		if (priorknow > 9999) boost /= 2;
	}

	if (boost < 0) boost = 0;

	spl_book[spell].sp_know = ((spl_book[spell].sp_know + boost > MAX_KNOW) ? MAX_KNOW : (spl_book[spell].sp_know + boost) );

}

/* reduce memory of a spell, e.g. because you have cyan spells or cast stuff like add spell memory */
void
drainknow(spell, drainamount)
int spell, drainamount;
{
	if (spl_book[spell].sp_know < drainamount) spl_book[spell].sp_know = 0;
	else spl_book[spell].sp_know -= drainamount;
}

void
incrnknow(spell, initial)
int spell;
boolean initial; /* FALSE if you knew the spell before, otherwise TRUE; reduction only if FALSE */
{
	int priorknow = spl_book[spell].sp_know;
	int knowvalue = (Race_if(PM_DUNADAN) ? DUNADAN_KEEN : KEEN);
	int boostmultiplier = 100;
	if (priorknow > 190100) boostmultiplier = 1;
	else if (priorknow > 180100) boostmultiplier = 2;
	else if (priorknow > 170100) boostmultiplier = 3;
	else if (priorknow > 160100) boostmultiplier = 4;
	else if (priorknow > 150100) boostmultiplier = 5;
	else if (priorknow > 140100) boostmultiplier = 6;
	else if (priorknow > 130100) boostmultiplier = 7;
	else if (priorknow > 120100) boostmultiplier = 8;
	else if (priorknow > 110100) boostmultiplier = 9;
	else if (priorknow > 100100) boostmultiplier = 10;
	else if (priorknow > 90100) boostmultiplier = 15;
	else if (priorknow > 80100) boostmultiplier = 20;
	else if (priorknow > 70100) boostmultiplier = 30;
	else if (priorknow > 60100) boostmultiplier = 40;
	else if (priorknow > 50100) boostmultiplier = 50;
	else if (priorknow > 40100) boostmultiplier = 60;
	else if (priorknow > 30100) boostmultiplier = 70;
	else if (priorknow > 20100) boostmultiplier = 80;
	else if (priorknow > 10100) boostmultiplier = 90;

	if (initial) boostmultiplier = 100;

	knowvalue *= boostmultiplier;
	knowvalue /= 100;

	if (MagicVacuum) {
		knowvalue /= (StrongMagicVacuum) ? 5 : 2;
		if (priorknow > 9999) knowvalue /= 2;
	}

	if (knowvalue < 0) knowvalue = 0;

	spl_book[spell].sp_know = (spl_book[spell].sp_know < 1) ? knowvalue : ((spl_book[spell].sp_know + knowvalue) > MAX_KNOW) ? MAX_KNOW : (spl_book[spell].sp_know + knowvalue);


}

/* oh my god why oh why does that have to depend on an actual item, why can't we use the item's ID --Amy */
void
initialspell(obj)
struct obj *obj;
{
	initialwonderspell(obj->otyp);
}


/* Learn a spell during creation of the initial inventory */
void
initialwonderspell(wospelnum)
int wospelnum;
{
	int i;

	for (i = 0; i < MAXSPELL; i++) {
	    if (spellid(i) == wospelnum) { /* not a bug - after all, you might e.g. play a haxor --Amy */
	    	/* pline("Error: Spell %s already known.", OBJ_NAME(objects[wospelnum])); */

		/* In Soviet Russia, enhancements aren't a thing. In fact, they don't even know how to spell the word 'enhancement'. Therefore, if someone goes ahead and suggests an enhancement that consists of double spellbooks giving twice the starting spellcasting memory, they say NOPE THAT IS INCOMPATIBLE WITH COMMUNISM and refuse to implement it. --Amy */
		if (!issoviet) {
			incrnknow(i, TRUE);
			if (Role_if(PM_MAHOU_SHOUJO)) incrnknow(i, TRUE);

			if (spl_book[i].sp_lev == 3) incrnknow(i, TRUE);
			if (spl_book[i].sp_lev == 4) { incrnknow(i, TRUE); incrnknow(i, TRUE);}
			if (spl_book[i].sp_lev == 5) { incrnknow(i, TRUE); incrnknow(i, TRUE); incrnknow(i, TRUE);}
			if (spl_book[i].sp_lev == 6) { incrnknow(i, TRUE); incrnknow(i, TRUE); incrnknow(i, TRUE); incrnknow(i, TRUE);}
			if (spl_book[i].sp_lev == 7) { incrnknow(i, TRUE); incrnknow(i, TRUE); incrnknow(i, TRUE); incrnknow(i, TRUE); incrnknow(i, TRUE);}
			if (spl_book[i].sp_lev == 8) { incrnknow(i, TRUE); incrnknow(i, TRUE); incrnknow(i, TRUE); incrnknow(i, TRUE); incrnknow(i, TRUE); incrnknow(i, TRUE);}
		}

	         return;
	    }
	    if (spellid(i) == NO_SPELL)  {
	        spl_book[i].sp_id = wospelnum;
	        spl_book[i].sp_lev = objects[wospelnum].oc_level;
		  spl_book[i].sp_memorize = TRUE;
	        incrnknow(i, TRUE);
			if (Role_if(PM_MAHOU_SHOUJO)) incrnknow(i, TRUE);

			/* high-level starting spells will be known for a longer time
			 * since you might not be able to cast them at all when you're just starting --Amy */
			if (spl_book[i].sp_lev == 3) incrnknow(i, TRUE);
			if (spl_book[i].sp_lev == 4) { incrnknow(i, TRUE); incrnknow(i, TRUE);}
			if (spl_book[i].sp_lev == 5) { incrnknow(i, TRUE); incrnknow(i, TRUE); incrnknow(i, TRUE);}
			if (spl_book[i].sp_lev == 6) { incrnknow(i, TRUE); incrnknow(i, TRUE); incrnknow(i, TRUE); incrnknow(i, TRUE);}
			if (spl_book[i].sp_lev == 7) { incrnknow(i, TRUE); incrnknow(i, TRUE); incrnknow(i, TRUE); incrnknow(i, TRUE); incrnknow(i, TRUE);}
			if (spl_book[i].sp_lev == 8) { incrnknow(i, TRUE); incrnknow(i, TRUE); incrnknow(i, TRUE); incrnknow(i, TRUE); incrnknow(i, TRUE); incrnknow(i, TRUE);}

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

	if (getspell(&spell_no, FALSE)) {
		if (spellknow(spell_no) <= 0) {
			You("are unable to focus your memory of the spell.");
			return (FALSE);
		} else if (spellknow(spell_no) <= 1000) {
			Your("focus and reinforce your memory of the spell.");
			incrnknow(spell_no, FALSE);
			if (Role_if(PM_MAHOU_SHOUJO)) incrnknow(spell_no, FALSE);
			exercise(A_WIS, TRUE);      /* extra study */
			return (TRUE);
		} else if (spellknow(spell_no) < 11000) {
			int studyvalue = Race_if(PM_DUNADAN) ? DUNADAN_KEEN : KEEN;
			int studydiff = 11000 - spellknow(spell_no); /* between 1 and 10000 */

			studyvalue *= studydiff;
			studyvalue /= 10000;

			Your("focus and reinforce your memory of the spell.");
			boostknow(spell_no, studyvalue);
			if (Role_if(PM_MAHOU_SHOUJO)) boostknow(spell_no, studyvalue);
			return (TRUE);
		} else /* 11000 < spellknow(spell_no) <= whatever the maximum is */
			You("know that spell quite well already.");
	}
	return (FALSE);
}

boolean
inertiacontrolspell(duration)
int duration;
{
	int spell_no;

	if (getspell(&spell_no, FALSE)) {
		if (spellid(spell_no) == SPE_INERTIA_CONTROL) {
			You("cannot control the inertia control spell.");
			return (FALSE);
		} else if (spellid(spell_no) != NO_SPELL) {
			u.inertiacontrolspell = spellid(spell_no);
			u.inertiacontrolspellno = spell_no;
			u.inertiacontrol = duration;
			You("start controlling the %s spell.", spellname(spell_no));
			return (TRUE);
		} else {
			You("decided to not control any spell after all.");
			return (FALSE);
		}
	}
	return (FALSE);
}

boolean
dememorizespell()
{
	int spell_no;

	if (getspell(&spell_no, FALSE)) {
		if (spellid(spell_no) != NO_SPELL) {
			spl_book[spell_no].sp_know = 0;
			pline("Alright, the %s spell is a forgotten spell now.", spellname(spell_no));
			return (TRUE);
		} else {
			You("decided to not dememorize any spell after all.");
			return (FALSE);
		}

	}
	return (FALSE);
}

boolean
addsomespellmemory()
{
	int spell_no;

	if (getspell(&spell_no, FALSE)) {
		if (spellid(spell_no) == SPE_ADD_SPELL_MEMORY) {
			You("cannot add memory to that spell.");
		} else if (spellid(spell_no) != NO_SPELL) {
			if (rn2(10) && spellknow(spell_no) <= 0) {
				pline("Your attempt to regain knowledge of that forgotten spell fails.");
				return (TRUE);
			}
			pline("Your knowledge of the %s spell increases.", spellname(spell_no));
			boostknow(spell_no, 500);
			return (TRUE);
		} else {
			You("decided to not add memory to any spell after all.");
			return (FALSE);
		}

	}
	return (FALSE);
}

void
extramemory()
{
	if (spellid(0) == NO_SPELL) {
		You("don't know any spells, and therefore you cannot add spell memory to them either.");
		return;
	}

	pline("Choose a spell to add spell memory.");
addxtragain:
	if (!addsomespellmemoryX()) {
		if (yn("Really exit with no spell selected?") == 'y')
			pline("You just wasted the opportunity to add memory to a spell.");
		else goto addxtragain;
	}
}

boolean
addsomespellmemoryX()
{
	int spell_no;

	if (getspell(&spell_no, FALSE)) {
		if (spellid(spell_no) != NO_SPELL) {
			if (rn2(20) && spellknow(spell_no) <= 0) {
				pline("Your attempt to regain knowledge of that forgotten spell fails.");
				return (TRUE);
			}
			pline("Your knowledge of the %s spell increases.", spellname(spell_no));
			boostknow(spell_no, rnd(5000));
			return (TRUE);
		} else {
			You("decided to not add memory to any spell after all.");
			return (FALSE);
		}

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

/* percentage chance for mastermind role to resist amnesia and spell forgetting effects --Amy */
boolean
mastermindsave()
{
	int mmchance = 0;

	if (!Role_if(PM_MASTERMIND)) return FALSE; /* shouldn't happen */

	if (PlayerCannotUseSkills) return FALSE;

	switch (P_SKILL(P_MEMORIZATION)) {
		case P_BASIC: mmchance = 20; break;
		case P_SKILLED: mmchance = 40; break;
		case P_EXPERT: mmchance = 60; break;
		case P_MASTER: mmchance = 80; break;
		case P_GRAND_MASTER: mmchance = 90; break;
		case P_SUPREME_MASTER: mmchance = 95; break;
	}

	if (mmchance > rn2(100)) return TRUE;

	return FALSE;

}

/* lose all spells unconditionally --Amy */
void
delete_all_spells()
{
	int n;

	for (n = 0; n < MAXSPELL && spellid(n) != NO_SPELL; n++) {
	    spellid(n) = NO_SPELL;
	}

}

/* calculation for mana cost, externalized --Amy */
int
manacost(spellnum)
int spellnum;
{
	int energy = (objects[spellnum].oc_level * 5);
	if (energy < 5) energy = 5; /* fail safe */

	switch (spellnum) {
		case SPE_FINGER_OF_DEATH: energy *= 3; break;
		case SPE_TIME: energy *= 4; break;
		case SPE_INERTIA: energy *= 4; break;
		case SPE_TIME_STOP: energy *= 5; break;
		case SPE_PARALYSIS: energy *= 2; break;
		case SPE_HELLISH_BOLT: energy *= 2; break;
		case SPE_PETRIFY: energy *= 4; break;
		case SPE_JUMPING: energy *= 5; break;
		case SPE_ARMOR_SMASH: energy *= 5; energy /= 3; break;
		case SPE_GODMODE: energy *= 5; energy /= 2; break;
		case SPE_DISINTEGRATION: energy *= 5; break;
		case SPE_DISINTEGRATION_BEAM: energy *= 5; break;
		case SPE_FIXING: energy *= 3; break;
		case SPE_CONVERGE_BREATH: energy *= 4; break;
		case SPE_CHROMATIC_BEAM: energy *= 10; energy /= 7; break;
		case SPE_HEALING: energy *= 3; energy /= 2; break;
		case SPE_WATER_FLAME: energy *= 3; energy /= 2; break;
		case SPE_FIREBALL: energy *= 2; break;
		case SPE_SHINING_WAVE: energy *= 5; break;
		case SPE_RELOCATION: energy *= 5; break;
		case SPE_FIRE_BOLT: energy *= 3; energy /= 2; break;
		case SPE_DEFENSIVE_FIREBALL: energy *= 3; energy /= 2; break;
		case SPE_CONE_OF_COLD: energy *= 3; energy /= 2; break;
		case SPE_MULTIBEAM: energy *= 6; energy /= 5; break;
		case SPE_CALL_THE_ELEMENTS: energy *= 7; energy /= 4; break;
		case SPE_INFERNO: energy *= 3; energy /= 2; break;
		case SPE_ICE_BEAM: energy *= 3; energy /= 2; break;
		case SPE_HYPER_BEAM: energy *= 4; energy /= 3; break;
		case SPE_ELEMENTAL_BEAM: energy *= 6; energy /= 5; break;
		case SPE_NATURE_BEAM: energy *= 5; energy /= 4; break;
		case SPE_ULTRA_P: energy *= 3; energy /= 2; break;
		default: break;
	}

	return energy;
}

/* learn a random spell ("whichspell" == -1) or a specific one ("whichspell" = ID of the book) --Amy */
void
wonderspell(whichspell, memorybonus)
int whichspell, memorybonus; /* "memorybonus" is a bonus to the memory of the newly learned spell */
{
	register int randomspell = SPE_FORCE_BOLT + rn2((SPE_PSYBEAM + 1) - SPE_FORCE_BOLT);

	if (whichspell != -1) randomspell = whichspell;

	char splname[BUFSZ];
	int i;

	sprintf(splname, "%s", OBJ_NAME(objects[randomspell]) );

	for (i = 0; i < MAXSPELL; i++)  {
		if (spellid(i) == randomspell)  {
			if (spellknow(i) <= MAX_CAN_STUDY) {
				Your("knowledge of the %s spell is keener.", splname);
				incrnknow(i, FALSE);
				if (u.emynluincomplete) boostknow(i, 1000);
				if (uarmg && itemhasappearance(uarmg, APP_RUNIC_GLOVES) && !rn2(2) ) incrnknow(i, FALSE);
				if (Role_if(PM_MAHOU_SHOUJO)) incrnknow(i, FALSE);
				if (memorybonus) boostknow(i, memorybonus);

				if (!PlayerCannotUseSkills && P_SKILL(P_MEMORIZATION) >= P_BASIC) {

					char nervbuf[QBUFSZ];
					char thisisannoying = 0;

					if (!u.youhavememorized) {
						u.youhavememorized = TRUE;
						if (!iflags.memorizationknown) pline("You have the memorization skill, which allows you to gain extra spell memory for newly learned spells. Whenever you learn a spell, you are asked whether you want to use the skill to boost the new spell's memory. In the case of doubt you should ALWAYS ANSWER YES. If you answer no, you just throw the bonus away. (Exception is if you want a forgotten spell, but you only ever need one of those normally.)");
					}

					if (!iflags.memorizationknown) sprintf(nervbuf, "Memorize this spell to add more spell memory? In the case of doubt you should always answer yes, unless you want the bonus to go to waste.");
					else sprintf(nervbuf, "Memorize this spell to add more spell memory?");
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
						pline("Spell memory increased! You gained %d%% extra spell memory.", memoboost * 10);
					} else {
						spl_book[i].sp_memorize = FALSE;
						pline("You decided to throw away the spell memory bonus. The spell was set to non-memorization mode. If you did that by mistake, you should open the spell menu and turn memorization for this spell back on so that it properly benefits from memorization skill.");
					}

				}

			} else {
			    You("know %s quite well already.", splname);
			}
			break;
		} else if (spellid(i) == NO_SPELL)  {
			spl_book[i].sp_id = randomspell;
			spl_book[i].sp_lev = objects[randomspell].oc_level;
			spl_book[i].sp_memorize = TRUE;
			incrnknow(i, TRUE);
			if (u.emynluincomplete) boostknow(i, 1000);
			if (uarmg && itemhasappearance(uarmg, APP_RUNIC_GLOVES) && !rn2(2) ) incrnknow(i, TRUE);
			if (Role_if(PM_MAHOU_SHOUJO)) incrnknow(i, TRUE);
			if (memorybonus) boostknow(i, memorybonus);
			You("gain knowledge of the %s spell.", splname);
			if (randomspell == SPE_FORBIDDEN_KNOWLEDGE) {
				u.ugangr += 15;
				if (flags.soundok) You_hear("a thunderous growling and rumbling...");
				if (PlayerHearsSoundEffects) pline(issoviet ? "Pozdravlyayu, teper' vashe bozhestvo ochen' zol. Pochemu ty ne molish'sya? Eto mozhet byt' prosto privesti svoyu zhizn' do kontsa, a zatem vy mozhete svernut' luchshe, nesovetskimi kharakter nakonets-to! SDELAY ETO SEYCHAS!" : "Grrrrrrr! Grummel! Wummmmmm!");
			}

			if (randomspell == SPE_AMNESIA) {
				You_feel("dizzy!");
				forget(ALL_MAP, FALSE);
			}

			if (!PlayerCannotUseSkills && P_SKILL(P_MEMORIZATION) >= P_BASIC) {

				char nervbuf[QBUFSZ];
				char thisisannoying = 0;

				if (!u.youhavememorized) {
					u.youhavememorized = TRUE;
					if (!iflags.memorizationknown) pline("You have the memorization skill, which allows you to gain extra spell memory for newly learned spells. Whenever you learn a spell, you are asked whether you want to use the skill to boost the new spell's memory. In the case of doubt you should ALWAYS ANSWER YES. If you answer no, you just throw the bonus away. (Exception is if you want a forgotten spell, but you only ever need one of those normally.)");
				}

				if (!iflags.memorizationknown) sprintf(nervbuf, "Memorize this spell to add more spell memory? In the case of doubt you should always answer yes, unless you want the bonus to go to waste.");
				else sprintf(nervbuf, "Memorize this spell to add more spell memory?");
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
					pline("Spell memory increased! You gained %d%% extra spell memory.", memoboost * 10);
				} else {
					spl_book[i].sp_memorize = FALSE;
					pline("You decided to throw away the spell memory bonus. The spell was set to non-memorization mode. If you did that by mistake, you should open the spell menu and turn memorization for this spell back on so that it properly benefits from memorization skill.");
				}

			}

			break;
		}
	}

}

/*spell.c*/
