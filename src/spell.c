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
#define CAST_BOOST 	  500	/* memory increase for successful casting */
#define MAX_KNOW 	70000	/* Absolute Max timeout */
#define MAX_CAN_STUDY 	60000	/* Can study while timeout is less than */

#define MAX_STUDY_TIME 	  300	/* Max time for one study session */
#define MAX_SPELL_STUDY    30	/* Uses before spellbook crumbles */

#define spellknow(spell)	spl_book[spell].sp_know 

static const char all_count[] = { ALLOW_COUNT, ALL_CLASSES, 0 };

#define incrnknow(spell)        spl_book[spell].sp_know = ((spl_book[spell].sp_know < 1) ? KEEN \
				 : ((spl_book[spell].sp_know + KEEN) > MAX_KNOW) ? MAX_KNOW \
				 : spl_book[spell].sp_know + KEEN)
#define boostknow(spell,boost)  spl_book[spell].sp_know = ((spl_book[spell].sp_know + boost > MAX_KNOW) ? MAX_KNOW \
				 : spl_book[spell].sp_know + boost)

#define spellev(spell)		spl_book[spell].sp_lev
#define spellid(spell)          spl_book[spell].sp_id
#define spellname(spell)	OBJ_NAME(objects[spellid(spell)])
#define spellet(spell)	\
	((char)((spell < 26) ? ('a' + spell) : \
	        (spell < 52) ? ('A' + spell - 26) : \
		(spell < 78) ? ('0' + spell - 52) : 0 ))

STATIC_DCL int FDECL(spell_let_to_idx, (CHAR_P));
STATIC_DCL boolean FDECL(cursed_book, (struct obj *bp));
STATIC_DCL boolean FDECL(confused_book, (struct obj *));
STATIC_DCL void FDECL(deadbook, (struct obj *));
STATIC_PTR int NDECL(learn);
STATIC_DCL void NDECL(do_reset_learn);
STATIC_DCL boolean FDECL(getspell, (int *));
STATIC_DCL boolean FDECL(dospellmenu, (const char *,int,int *));
STATIC_DCL int FDECL(percent_success, (int));
STATIC_DCL void NDECL(cast_protection);
STATIC_DCL void NDECL(cast_reflection);
STATIC_DCL void FDECL(spell_backfire, (int));
STATIC_DCL const char *FDECL(spelltypemnemonic, (int));
STATIC_DCL int FDECL(isqrt, (int));

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

/* TRUE: book should be destroyed by caller */
STATIC_OVL boolean
cursed_book(bp)
	struct obj *bp;
{
	int lev = objects[bp->otyp].oc_level;

	switch(rn2(lev)) {
	case 0:
		You_feel("a wrenching sensation.");
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
			} else {
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

/* special effects for The Book of the Dead */
STATIC_OVL void
deadbook(book2)
struct obj *book2;
{
    struct monst *mtmp, *mtmp2;
    coord mm;

    You("turn the pages of the Book of the Dead...");
    makeknown(SPE_BOOK_OF_THE_DEAD);
    /* KMH -- Need ->known to avoid "_a_ Book of the Dead" */
    book2->known = 1;
    if(invocation_pos(u.ux, u.uy) && !On_stairs(u.ux, u.uy)) {
	register struct obj *otmp;
	register boolean arti1_primed = FALSE, arti2_primed = FALSE,
			 arti_cursed = FALSE;

	if(book2->cursed) {
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
		if(!otmp->cursed) arti1_primed = TRUE;
		else arti_cursed = TRUE;
	    }
	    if(otmp->otyp == BELL_OF_OPENING &&
	       (moves - otmp->age) < 5L) { /* you rang it recently */
		if(!otmp->cursed) arti2_primed = TRUE;
		else arti_cursed = TRUE;
	    }
	}

	if(arti_cursed) {
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
	if (Confusion && !Conf_resist && rn2(Role_if(PM_LIBRARIAN) ? 2 : 10) ) {		/* became confused while learning */
	    (void) confused_book(book);
	    book = 0;			/* no longer studying */
	    nomul((delay - end_delay), "reading a confusing book");	/* remaining delay is uninterrupted */
	    delay = end_delay;
	    return(0);
	}
	if (delay < end_delay) {    /* not if (delay++), so at end delay == 0 */
	    delay++;
	    return(1); /* still busy */
	}
	exercise(A_WIS, TRUE);		/* you're studying. */
	booktype = book->otyp;
	if(booktype == SPE_BOOK_OF_THE_DEAD) {
	    deadbook(book);
	    return(0);
	}

	Sprintf(splname, objects[booktype].oc_name_known ?
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
			if (Role_if(PM_MAHOU_SHOUJO)) incrnknow(i);
			book->spestudied++;
			You("have keen knowledge of the spell.");
			You(i > 0 ? "add %s to your repertoire." : "learn %s.",
			    splname);
			if (booktype == SPE_FORBIDDEN_KNOWLEDGE) {
				u.ugangr += 15;
				pline("You hear a thunderous growling and rumbling...");
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

	if ( (book->cursed || book->spe < 1) && !Role_if(PM_LIBRARIAN) ) {	/* maybe a demon cursed it */
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
	} else {
		/* KMH -- Simplified this code */
		if (booktype == SPE_BLANK_PAPER) {
			pline("This spellbook is all blank.");
			makeknown(booktype);
			return(1);
		}
		if (spellbook->spe && confused && rn2(Role_if(PM_LIBRARIAN) ? 2 : 10) ) {
		    check_unpaid_usage(spellbook, TRUE);
		    consume_obj_charge(spellbook, FALSE);
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
		    if ( (spellbook->cursed || (spellbook->spe < 1 && rn2(5)) ) && !Role_if(PM_LIBRARIAN) ) {
			too_hard = TRUE;
		    } else {
			/* uncursed - chance to fail */
			int read_ability = ACURR(A_INT) + 4 + u.ulevel/2
			    - 2*objects[booktype].oc_level
			    + ((ublindf && ublindf->otyp == LENSES) ? 2 : 0);
			/* only wizards know if a spell is too difficult */
			if (Role_if(PM_WIZARD) && read_ability < 20 &&
			    !confused && !spellbook->spe) {
			    char qbuf[QBUFSZ];
			    Sprintf(qbuf,
		      "This spellbook is %sdifficult to comprehend. Continue?",
				    (read_ability < 12 ? "very " : ""));
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

		if ( (too_hard || rn2(2)) && ( (spellbook->cursed && !Role_if(PM_LIBRARIAN) ) || !spellbook->spe)) {
		    boolean gone = cursed_book(spellbook);

		    nomul(delay, "reading a cursed book");			/* study time */
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
		} else if (confused) {
		    if (!confused_book(spellbook)) {
			spellbook->in_use = FALSE;
		    }
		    nomul(delay, "reading a book while confused");
		    delay = 0;
		    return(1);
		}
		spellbook->in_use = FALSE;

		/* The glowing words make studying easier */
		if (spellbook->otyp != SPE_BOOK_OF_THE_DEAD) {
		    delay *= 2;
		    if (spellbook->spe) {
			check_unpaid_usage(spellbook, TRUE);
			consume_obj_charge(spellbook, FALSE);
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
	        	Sprintf(qbuf, "Study for at least %i turns?", (multi+1));
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
	    if (spellknow(i))
		decrnknow(i);
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

	    if (nspells == 1)  Strcpy(lets, "a");
	    else if (nspells < 27)  Sprintf(lets, "a-%c", 'a' + nspells - 1);
	    else if (nspells == 27)  Sprintf(lets, "a-z A");
	    else if (nspells < 53)
		Sprintf(lets, "a-z A-%c", 'A' + nspells - 27);
	    else if (nspells == 53)  Sprintf(lets, "a-z A-Z 0");
	    else if (nspells < 78)
		Sprintf(lets, "a-z A-Z 0-%c", '0' + nspells - 53);
	    else  Sprintf(lets, "a-z A-Z 0-9");

	    for(;;)  {
		Sprintf(qbuf, "Cast which spell? [%s ?]", lets);
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
	return dospellmenu( (YellowSpells || u.uprops[YELLOW_SPELLS].extrinsic || have_yellowspellstone()) ? "Your spells are yellow." : "Choose which spell to cast",
			   SPELLMENU_CAST, spell_no);
}

/* the 'Z' command -- cast a spell */
int
docast()
{
	int spell_no;

	if (getspell(&spell_no))
	    return spelleffects(spell_no, FALSE);
	return 0;
}

STATIC_OVL const char*
spelltypemnemonic(int skill)
{
	switch (skill) {
	    case P_ATTACK_SPELL:
	        return " attack";
	    case P_HEALING_SPELL:
		return "healing";
	    case P_DIVINATION_SPELL:
	        return " divine";
	    case P_ENCHANTMENT_SPELL:
	        return "enchant";
		case P_PROTECTION_SPELL:
	        return "protect";
	    case P_BODY_SPELL:
	        return "   body";
	    case P_MATTER_SPELL:
	        return " matter";
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
		(!(AllSkillsUnskilled || u.uprops[SKILL_DEACTIVATED].extrinsic || have_unskilledstone()) && P_SKILL(spell_skilltype(SPE_PROTECTION)) >= P_EXPERT) ? 20 : 10;
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
	incr_itimeout(&HReflecting, rn1(10, HReflecting ? 20 : 100));
}


/* attempting to cast a forgotten spell will cause disorientation */
STATIC_OVL void
spell_backfire(spell)
int spell;
{
    long duration = (long)((spellev(spell) + 1) * 3);	 /* 6..24 */

    /* prior to 3.4.1, the only effect was confusion; it still predominates */
    switch (rn2(16)) {
    case 0:
    case 1:
    case 2:
    case 3: make_confused(duration, FALSE);			/* 40% */
	    break;
    case 4:
    case 5:
    case 6: make_confused(2L * duration / 3L, FALSE);		/* 30% */
	    make_stunned(duration / 3L, FALSE);
	    break;
    case 7:
    case 8: make_stunned(2L * duration / 3L, FALSE);		/* 20% */
	    make_confused(duration / 3L, FALSE);
	    break;
    case 9: make_stunned(duration, FALSE);			/* 10% */
	    break;
    case 10: make_numbed(duration, FALSE);			/* 10% */
	    break;
    case 11: make_frozen(duration, FALSE);			/* 10% */
	    break;
    case 12: make_burned(duration, FALSE);			/* 10% */
	    break;
    case 13: make_feared(duration, FALSE);			/* 10% */
	    break;
    case 14: make_blinded(duration, FALSE);			/* 10% */
	    break;
    case 15: make_hallucinated(duration, FALSE, 0L);			/* 10% */
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

	/*
	 * Find the skill the hero has in a spell type category.
	 * See spell_skilltype for categories.
	 */
	skill = spell_skilltype(spellid(spell));
	role_skill = P_SKILL(skill);
	if (AllSkillsUnskilled || u.uprops[SKILL_DEACTIVATED].extrinsic || have_unskilledstone()) role_skill = P_ISRESTRICTED;

	/*
	 * Spell casting no longer affects knowledge of the spell. A
	 * decrement of spell knowledge is done every turn.
	 */
	if (spellknow(spell) <= 0) {
	    Your("knowledge of this spell is twisted.");
	    pline("It invokes nightmarish images in your mind...");
	    spell_backfire(spell);
	    return(0);
	} else if (spellknow(spell) <= 100) {
	    You("strain to recall the spell.");
	} else if (spellknow(spell) <= 1000) {
	    Your("knowledge of this spell is growing faint.");
	}
	energy = (spellev(spell) * 5);    /* 5 <= energy <= 35 */
	if (YellowSpells || u.uprops[YELLOW_SPELLS].extrinsic || have_yellowspellstone()) energy *= 2;

	/* Some spells are just plain too powerful, and need to be nerfed. Sorry. --Amy */
	if (spellid(spell) == SPE_FINGER_OF_DEATH) energy *= 2;
	if (spellid(spell) == SPE_PETRIFY) { energy *= 5; energy /= 2;}
	if (spellid(spell) == SPE_DISINTEGRATION) energy *= 3;

	if (Role_if(PM_MAHOU_SHOUJO) && energy > 1) energy /= 2; /* Casting any sort of magic uses half power for them */

	if (u.uhunger <= 10 && spellid(spell) != SPE_DETECT_FOOD) {
		You("are too hungry to cast that spell.");
		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return(0);
	} else if (ACURR(A_STR) < 4)  {
		You("lack the strength to cast spells.");
		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return(0);
	} else if(check_capacity(
		"Your concentration falters while carrying so much stuff.")) {
		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
	    return (1);
	} else if (!freehand()) {
		Your("arms are not free to cast!");
		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return (0);
	} else if (Muteness || u.uprops[MUTENESS].extrinsic || have_mutenessstone()) {
		pline("You're muted!");
		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
		return (0);
	}


	/* Casting any sort of magic as a mahou shoujo or naga does not cause hunger */

		if (!Role_if(PM_MAHOU_SHOUJO) && !Race_if(PM_HUMANLIKE_NAGA) && (spellid(spell) != SPE_DETECT_FOOD) ) {
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

	if (u.uhave.amulet) { /* casting while you have the amulet always causes extra hunger no matter what --Amy */
		hungr += rnd(2*energy);
	}

			if (hungr > u.uhunger-3)
				hungr = u.uhunger-3;
	if (energy > u.uen)  {
		if (role_skill >= P_SKILLED) You("don't have enough energy to cast that spell.");
		else You("don't have enough energy to cast that spell. The required amount was %d.",energy);
		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
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
	if ( role_skill == P_BASIC) {hungr *= 85; hungr /= 100;}
	if ( role_skill == P_SKILLED) {hungr *= 70; hungr /= 100;}
	if ( role_skill == P_EXPERT) {hungr *= 55; hungr /= 100;}
	if ( role_skill == P_MASTER) {hungr *= 40; hungr /= 100;}
	if ( role_skill == P_GRAND_MASTER) {hungr *= 25; hungr /= 100;}

	/* casting it often (and thereby keeping it in memory) should also reduce hunger... */
	if ( spellknow(spell) >= 10000) {hungr *= 9; hungr /= 10;}
	if ( spellknow(spell) >= 20000) {hungr *= 9; hungr /= 10;}
	if ( spellknow(spell) >= 30000) {hungr *= 9; hungr /= 10;}
	if ( spellknow(spell) >= 40000) {hungr *= 9; hungr /= 10;}
	if ( spellknow(spell) >= 50000) {hungr *= 9; hungr /= 10;}
	if ( spellknow(spell) >= 60000) {hungr *= 9; hungr /= 10;}
	if ( spellknow(spell) >= 70000) {hungr *= 9; hungr /= 10;}

	if (hungr < 0) hungr = 0; /* fail safe */

	morehungry(hungr);

	chance = percent_success(spell);
	if ( (confused && spellid(spell) != SPE_CURE_CONFUSION && rn2(10) ) || (rnd(100) > chance)) {
		pline("You fail to cast the spell correctly.");
		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */

#ifdef ALLEG_FX
                if (iflags.usealleg) alleg_aura(u.ux, u.uy, P_ATTACK_SPELL-1);
#endif

		/* Higher spellcasting skills mean failure takes less mana. --Amy */

		u.uen -= ((energy * 50 / ((role_skill == P_GRAND_MASTER) ? 220 : (role_skill == P_MASTER) ? 200 : (role_skill == P_EXPERT) ? 180 : (role_skill == P_SKILLED) ? 160 : (role_skill == P_BASIC) ? 140 : 120)) + 1) ;
		flags.botl = 1;
		return(1);
	}

	/* Players could cheat if they had just barely enough mana for casting a spell without the increased drain.
	 * They'd just need to keep trying until the extra mana costs are randomly very low.
	 * Prevent players from abusing this by calculating the extra drain _after_ the other checks. --Amy */
	if (u.uhave.amulet) {
		You_feel("the amulet draining your energy away.");
		energy += rnd(2*energy);
	}

	u.uen -= energy;
	/* And if the amulet drained it below zero, set it to zero and just make the spell fail now. */
	if (u.uhave.amulet && u.uen < 0) {
		pline("You are exhausted, and fail to cast the spell due to the amulet draining all your energy away.");
		display_nhwindow(WIN_MESSAGE, TRUE);    /* --More-- */
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
	pseudo = mksobj(spellid(spell), FALSE, FALSE);
	pseudo->blessed = pseudo->cursed = 0;
	pseudo->quan = 20L;			/* do not let useup get it */

	/* WAC -- If skilled enough,  will act like a blessed version */
	if (role_skill >= P_SKILLED) { /* made it depend on skill level --Amy */
		if (!rn2(4) && role_skill == P_SKILLED) pseudo->blessed = 1;
		if (!rn2(3) && role_skill == P_EXPERT) pseudo->blessed = 1;
		if (!rn2(2) && role_skill == P_MASTER) pseudo->blessed = 1;
		if (role_skill == P_GRAND_MASTER) pseudo->blessed = 1;
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
	case SPE_POISON_BLAST:
		if (tech_inuse(T_SIGIL_TEMPEST)) {
		    weffects(pseudo);
		    break;
		} /* else fall through... */
	/* these spells are all duplicates of wand effects */
	case SPE_FORCE_BOLT:
	case SPE_SLEEP:
	case SPE_KNOCK:
	case SPE_SLOW_MONSTER:
	case SPE_WIZARD_LOCK:
	case SPE_DIG:
	case SPE_TURN_UNDEAD:
	case SPE_POLYMORPH:
	case SPE_MUTATION:
	case SPE_TELEPORT_AWAY:
	case SPE_CANCELLATION:
	case SPE_FINGER_OF_DEATH:
	case SPE_LIGHT:
	case SPE_DARKNESS:
	case SPE_DETECT_UNSEEN:
	case SPE_HEALING:
	case SPE_EXTRA_HEALING:
	case SPE_FULL_HEALING:
	case SPE_DRAIN_LIFE:
	case SPE_STONE_TO_FLESH:
	case SPE_FINGER:
	case SPE_DISINTEGRATION:
	case SPE_PETRIFY:
	case SPE_PARALYSIS:
		if (!(objects[pseudo->otyp].oc_dir == NODIR)) {
			if (atme) u.dx = u.dy = u.dz = 0;
			else if (!getdir((char *)0)) {
			    /* getdir cancelled, re-use previous direction */
			    pline_The("magical energy is released!");
			}
			if(!u.dx && !u.dy && !u.dz) {
			    if ((damage = zapyourself(pseudo, TRUE)) != 0) {
				char buf[BUFSZ];
				Sprintf(buf, "zapped %sself with a spell", uhim());
				losehp(damage, buf, NO_KILLER_PREFIX);
			    }
			} else weffects(pseudo);
		} else weffects(pseudo);
		update_inventory();	/* spell may modify inventory */
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
		if (role_skill >= P_GRAND_MASTER) n = 8;
		else if (role_skill >= P_MASTER) n = 9;
		else if (role_skill >= P_EXPERT) n = 10;
		else if (role_skill >= P_SKILLED) n = 11;
		else if (role_skill >= P_BASIC) n = 12;
		else n = 14;	/* Unskilled or restricted */
		if (!rn2(n)) {
		    pseudo->blessed = 0;
		    (void) seffects(pseudo);
		} else
		    Your("enchantment failed!");
		break;

	case SPE_ENTRAPPING:

		trap_detect((struct obj *)0);
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
		aggravate();
		break;
	case SPE_REMOVE_BLESSING:
		{

		    if (Hallucination)
			You_feel("the power of the Force against you!");
		    else
			You_feel("like you need some help.");

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
	case SPE_CURE_BURN:
		make_burned(0L,TRUE);
		break;
	case SPE_CURE_FREEZE:
		make_frozen(0L,TRUE);
		break;
	case SPE_CURE_NUMBNESS:
		make_numbed(0L,TRUE);
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
		(void) create_gas_cloud(cc.x, cc.y, 3, 8);
		break;
		}
		break;
	case SPE_TIME_STOP:
		pline((Role_if(PM_SAMURAI) || Role_if(PM_NINJA)) ? "Jikan ga teishi shimashita." : "Time has stopped.");
		if (rn2(3)) TimeStopped += (rnd(3) + 1);
		else TimeStopped += rnd(3 + spell_damage_bonus(spellid(spell)) );
		break;
	case SPE_LEVELPORT:
	      if (!flags.lostsoul && !flags.uberlostsoul && !(u.uprops[STORM_HELM].extrinsic)) level_tele();
		else pline("Hmm... that level teleport spell didn't do anything.");

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
	case SPE_CREATE_FAMILIAR:
		(void) make_familiar((struct obj *)0, u.ux, u.uy, FALSE);
		break;
	case SPE_CLAIRVOYANCE:
		if (!BClairvoyant)
		    do_vicinity_map();
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
			You("feel healthy ..... for the moment at least.");
			incr_itimeout(&HPoison_resistance, rn1(1000, 500) +
				spell_damage_bonus(spellid(spell))*100);
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_ANTI_DISINTEGRATION:
		if(!(HDisint_resistance & INTRINSIC)) {
			You("feel quite firm for a while.");
			incr_itimeout(&HDisint_resistance, rn1(1000, 500) +
				spell_damage_bonus(spellid(spell))*100);
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_MAGICTORCH:
		if(!(HSight_bonus & INTRINSIC)) {
			You("can see in the dark!");
			incr_itimeout(&HSight_bonus, rn1(200, 100) +
				spell_damage_bonus(spellid(spell))*20);
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_DISPLACEMENT:
		if(!(HDisplaced & INTRINSIC)) {
			pline("Your image is displaced!");
			incr_itimeout(&HDisplaced, rn1(200, 100) +
				spell_damage_bonus(spellid(spell))*20);
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_BOTOX_RESIST:
		if(!(HSick_resistance & INTRINSIC)) {
			You("feel resistant to sickness.");
			incr_itimeout(&HSick_resistance, rn1(1000, 500) +
				spell_damage_bonus(spellid(spell))*100);
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_GENOCIDE:

		if (role_skill >= P_GRAND_MASTER) n = 15;
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

		if (role_skill >= P_GRAND_MASTER) n = 40;
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

	case SPE_MAP_LEVEL:

		if (role_skill >= P_GRAND_MASTER) n = 5;
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
			if (!t->hiddentrap) t->tseen = 1;
			map_trap(t, TRUE);
		    }
		    do_mapping();
		    HConfusion = save_Hconf;
		    HHallucination = save_Hhallu;
		    pline("You feel knowledgable!");
		    object_detect((struct obj *)0, 0);
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
			incr_itimeout(&HAcid_resistance, rn1(1000, 500) +
				spell_damage_bonus(spellid(spell))*100);
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_RESIST_PETRIFICATION:
		if(!(HStone_resistance & INTRINSIC)) {
			You("feel more limber. Let's eat some cockatrice meat!");
			incr_itimeout(&HStone_resistance, rn1(200, 100) +
				spell_damage_bonus(spellid(spell))*100);
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_RESIST_SLEEP:
		if(!(HSleep_resistance & INTRINSIC)) {
			if (Hallucination)
				pline("Too much coffee!");
			else
				You("no longer feel tired.");
			incr_itimeout(&HSleep_resistance, rn1(1000, 500) +
				spell_damage_bonus(spellid(spell))*100);
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_ENDURE_COLD:
		if(!(HCold_resistance & INTRINSIC)) {
			You("feel warmer.");
			incr_itimeout(&HCold_resistance, rn1(1000, 500) +
				spell_damage_bonus(spellid(spell))*100);
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_ENDURE_HEAT:
		if(!(HFire_resistance & INTRINSIC)) {
			if (Hallucination)
				pline("Excellent! You feel, like, totally cool!");
			else
				You("feel colder.");
			incr_itimeout(&HFire_resistance, rn1(1000, 500) +
				spell_damage_bonus(spellid(spell))*100);
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;
	case SPE_INSULATE:
		if(!(HShock_resistance & INTRINSIC)) {
			if (Hallucination)
				pline("Bummer! You've been grounded!");
			else
				You("are not at all shocked by this feeling.");
			incr_itimeout(&HShock_resistance, rn1(1000, 500) +
				spell_damage_bonus(spellid(spell))*100);
		} else pline(nothing_happens);	/* Already have as intrinsic */
		break;

	case SPE_FORBIDDEN_KNOWLEDGE:
		if(!(HHalf_spell_damage & INTRINSIC)) {
			if (Hallucination)
				pline("Let the casting commence!");
			else
				You("feel a sense of spell knowledge.");
			incr_itimeout(&HHalf_spell_damage, rn1(100, 50) +
				spell_damage_bonus(spellid(spell))*10);
		}
		if(!(HHalf_physical_damage & INTRINSIC)) {
			if (Hallucination)
				pline("You feel like a tough motherfucker!");
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
		You("feel self-knowledgeable...");
		display_nhwindow(WIN_MESSAGE, FALSE);
		enlightenment(FALSE);
		pline("The feeling subsides.");
		exercise(A_WIS, TRUE);
		break;

	/* WAC -- new spells */
	case SPE_FLAME_SPHERE:
	case SPE_FREEZE_SPHERE:
	{	register int cnt = 1;
		struct monst *mtmp;


		if (role_skill >= P_SKILLED) cnt += (role_skill - P_BASIC);
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


		if (role_skill >= P_SKILLED) cnt += (role_skill - P_BASIC);
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


		if (role_skill >= P_SKILLED) cnt += (role_skill - P_BASIC);
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
		incr_itimeout(&HPasses_walls, rn1(100, 50));
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
			if (greatest_erosion(otmp) > 0) {
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

	default:
		/*impossible("Unknown spell %d attempted.", spell);*/
		pline("You attempted to cast a spell that either doesn't exist in this game, or it has been genocided.");
		obfree(pseudo, (struct obj *)0);
		return(0);
	}

	/* gain skill for successful cast */
	use_skill(skill, spellev(spell));
	if (rn2(2)) use_skill(skill, spellev(spell)); /* let's make gaining spellcasting skill a little bit easier --Amy */
	if (!rn2(50)) use_skill(skill, (spellev(spell) * 10) ); /* jackpot! */

	/* WAC successful casting increases solidity of knowledge */
	boostknow(spell,CAST_BOOST);
	if ((rnd(spellev(spell) + 5)) > 5) boostknow(spell,CAST_BOOST); /* higher-level spells boost more --Amy */
	if (!rn2(52 - (spellev(spell) * 2) ) ) { /* jackpot! */
		boostknow(spell, (CAST_BOOST * 5) );
		boostknow(spell, (CAST_BOOST * spellev(spell) ) );
	}

	if (Role_if(PM_MAHOU_SHOUJO)) boostknow(spell,CAST_BOOST);

	if (spell && pseudo && pseudo->otyp == SPE_ALTER_REALITY) {

		boostknow(spell, -(rnd(20000)));
		if (spellknow(spell) < 0) spl_book[spell].sp_know = 0;

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
	    while (dospellmenu( (YellowSpells || u.uprops[YELLOW_SPELLS].extrinsic || have_yellowspellstone() ) ? "Your spells are yellow." : "Currently known spells",
			       SPELLMENU_VIEW, &splnum)) {
		Sprintf(qbuf, "Reordering spells; swap '%s' with",
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
		Sprintf(buf, "%-20s     Level  %-12s Fail  Memory", "    Name", "Category");
	else
		Sprintf(buf, "Name\tLevel\tCategory\tFail");
	if (flags.menu_style == MENU_TRADITIONAL)
		Strcat(buf, iflags.menu_tab_sep ? "\tKey" : "  Key");
	add_menu(tmpwin, NO_GLYPH, &any, 0, 0, ATR_BOLD, buf, MENU_UNSELECTED);
	if (!SpellLoss && !u.uprops[SPELLS_LOST].extrinsic && !have_spelllossstone()) {for (i = 0; i < MAXSPELL && spellid(i) != NO_SPELL; i++) {
		Sprintf(buf, iflags.menu_tab_sep ?
			"%s\t%-d%s\t%s\t%-d%%" : "%-20s  %2d%s   %-12s %3d%%"
			"   %3d%%",
			spellname(i), spellev(i),
			(spellknow(i) > 1000) ? " " : (spellknow(i) ? "!" : "*"),
			spelltypemnemonic(spell_skilltype(spellid(i))),
			100 - percent_success(i),

	/* "Spell memory percentage no longer shows up in menu - Very little reason to have this considering the timeout is so long to begin with, and spellbooks are much more common." In Soviet Russia, people aren't a fan of games with good interfaces, and rather like it if there's as little information conveyed to the player as possible. The next things they'll remove are those obsolete things displayed on the bottom status line, like HP, AC and experience level; there's very little reason to display them either. Ah wait, they might just borrow the spell loss trap and display loss trap code from Slash'EM Extended and make their effect permanent - that gets rid of all the useless information displays at once, and players will be forever grateful! --Amy */

			issoviet ? 0 : (spellknow(i) * 100 + (KEEN-1)) / KEEN);
		if (flags.menu_style == MENU_TRADITIONAL)
			Sprintf(eos(buf), iflags.menu_tab_sep ?
				"\t%c" : "%4c ", spellet(i) ? spellet(i) : ' ');

		any.a_int = i+1;	/* must be non-zero */
		add_menu(tmpwin, NO_GLYPH, &any,
			 0, 0, ATR_NONE, buf,
			 (i == splaction) ? MENU_SELECTED : MENU_UNSELECTED);
	      }
	}
	else {for (i = 0; i < MAXSPELL && spellid(i) != NO_SPELL; i++)	{		
				Sprintf(buf, iflags.menu_tab_sep ?
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
		free((genericptr_t)selected);
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

	Sprintf(buf, "%-20s   Level    %-12s Fail", "    Name", "Category");
	dump("  ",buf);
	for (i = 0; i < MAXSPELL && spellid(i) != NO_SPELL; i++) {
		Sprintf(buf, "%c - %-20s  %2d%s   %-12s %3d%%",
			spellet(i), spellname(i), spellev(i),
			spellknow(i) ? " " : "*",
			spelltypemnemonic(spell_skilltype(spellid(i))),
			100 - percent_success(i));
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

	splcaster = urole.spelbase;
	special = urole.spelheal;
	statused = ACURR(urole.spelstat);

	/* Calculate armor penalties */
	if (uarm && !(uarm->otyp == ROBE ||
		      uarm->otyp == ROBE_OF_POWER ||
		      uarm->otyp == ROBE_OF_PROTECTION)) 
	    splcaster += 2;

	/* Robes are body armour in SLASH'EM */
	if (uarm && is_metallic(uarm))
	    splcaster += /*(uarmc && uarmc->otyp == ROBE) ?
		urole.spelarmr/2 : */urole.spelarmr;
	else if (uarmc && uarmc->otyp == ROBE)
	    splcaster -= urole.spelarmr;
	if (uarms) splcaster += urole.spelshld;

	if (uarmh && is_metallic(uarmh) && uarmh->otyp != HELM_OF_BRILLIANCE)
		splcaster += uarmhbon;
	if (uarmg && is_metallic(uarmg)) splcaster += uarmgbon;
	if (uarmf && is_metallic(uarmf)) splcaster += uarmfbon;

	if (spellid(spell) == urole.spelspec)
		splcaster += urole.spelsbon;

	/* `healing spell' bonus */
	if (spell_skilltype(spellid(spell)) == P_HEALING_SPELL)
		splcaster += special;

	if (uarm && uarm->otyp == ROBE_OF_POWER) splcaster -= 3;

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
	if (AllSkillsUnskilled || u.uprops[SKILL_DEACTIVATED].extrinsic || have_unskilledstone()) skill = P_ISRESTRICTED;

	/* come on, you should be able to cast better if your skill is higher! --Amy */
	if ( skill == P_BASIC) splcaster -= 3;
	if ( skill == P_SKILLED) splcaster -= 6;
	if ( skill == P_EXPERT) splcaster -= 9;
	if ( skill == P_MASTER) splcaster -= 12;
	if ( skill == P_GRAND_MASTER) splcaster -= 15;

	/* casting it often (and thereby keeping it in memory) should also improve chances... */
	if ( spellknow(spell) >= 20000) splcaster -= 1;
	if ( spellknow(spell) >= 23333) splcaster -= 1;
	if ( spellknow(spell) >= 26666) splcaster -= 1;
	if ( spellknow(spell) >= 30000) splcaster -= 1;
	if ( spellknow(spell) >= 33333) splcaster -= 1;
	if ( spellknow(spell) >= 36666) splcaster -= 1;
	if ( spellknow(spell) >= 40000) splcaster -= 1;
	if ( spellknow(spell) >= 43333) splcaster -= 1;
	if ( spellknow(spell) >= 46666) splcaster -= 1;
	if ( spellknow(spell) >= 50000) splcaster -= 1;
	if ( spellknow(spell) >= 53333) splcaster -= 1;
	if ( spellknow(spell) >= 56666) splcaster -= 1;
	if ( spellknow(spell) >= 60000) splcaster -= 1;
	if ( spellknow(spell) >= 63333) splcaster -= 1;
	if ( spellknow(spell) >= 66666) splcaster -= 1;
	if ( spellknow(spell) >= 70000) splcaster -= 1;

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

	if ( (CasterProblem || u.uprops[CASTER_PROBLEM].extrinsic || have_antimagicstone() ) && chance > 0) {

		chance = (chance / u.uhpmax * u.uhp);

	}

	/* Clamp to percentile */
	if (chance > 100) chance = 100;
	if (Role_if(PM_FAILED_EXISTENCE)) chance /= 2; /* at least 50% fail for all spells */
	if (chance < (issoviet ? 0 : 10) ) chance = (issoviet ? 0 : 10); /* used to be 0, but that was just stupid in my opinion --Amy */

	return chance;
}

/* Learn a spell during creation of the initial inventory */
void
initialspell(obj)
struct obj *obj;
{
	int i;

	for (i = 0; i < MAXSPELL; i++) {
	    if (spellid(i) == obj->otyp) {
	         pline("Error: Spell %s already known.",
	         		OBJ_NAME(objects[obj->otyp]));
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

/*spell.c*/
