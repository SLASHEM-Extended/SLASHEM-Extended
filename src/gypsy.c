/*** gypsy.c ***/

#include "hack.h"
#include "egyp.h"
#include "qtext.h"


/* To do:
 *	fortune_lev()
 *	Fourtunes for suited cards
 *	On-line help
 */


/*** Money-related functions ***/

static void
gypsy_charge (mtmp, amount)
	struct monst *mtmp;
	long amount;
{
#ifdef GOLDOBJ
	struct obj *gypgold;
#endif

	/* Take from credit first */
	if (amount > EGYP(mtmp)->credit) {
		/* Do in several steps, for broken compilers */
		amount -= EGYP(mtmp)->credit;
		EGYP(mtmp)->credit = 0;
#ifdef GOLDOBJ
		money2mon(mtmp, amount);
#else
		u.ugold -= amount;
#endif
		flags.botl = 1;
	} else
		EGYP(mtmp)->credit -= amount;

	/* The gypsy never carries cash; it might get stolen! */
#ifdef GOLDOBJ
	gypgold = findgold(mtmp->minvent);
	if (gypgold)
		m_useup(mtmp, gypgold);
#endif
	return;
}

static boolean
gypsy_offer (mtmp, cost, txt)
	struct monst *mtmp;
	long cost;
	char *txt;
{
#ifdef GOLDOBJ
	long umoney;
	umoney = money_cnt(invent);
#endif
	verbalize("For %ld credit I will %s!", cost, txt);
	if (EGYP(mtmp)->credit >= cost) {
		if (yn("Accept this offer?") == 'y') {
			EGYP(mtmp)->credit -= cost;
			return (TRUE);
		}
#ifndef GOLDOBJ
	} else if (EGYP(mtmp)->credit + u.ugold >= cost)
		verbalize("What a pity that I can't accept gold!");
#else
	} else if (EGYP(mtmp)->credit + umoney >= cost)
		verbalize("What a pity that I can't accept money!");
#endif
		/* Maybe you could try gambling some of it for credit... */
	else
		verbalize("What a pity that you don't have enough!");
	return (FALSE);
}

static long
gypsy_bet (mtmp, minimum)
	struct monst *mtmp;
	long minimum;
{
	char prompt[BUFSZ], buf[BUFSZ];
	long bet = 0L;
#ifdef GOLDOBJ
	long umoney;
	umoney = money_cnt(invent);
#endif

	if (minimum > EGYP(mtmp)->credit + 
#ifndef GOLDOBJ
													u.ugold) {
#else
 													umoney) {		
#endif
		You("don't have enough money for the minimum bet.");
		return (0L);
	}

	/* Prompt for an amount */
	Sprintf(prompt, "Bet how much (%ld to %ld)?", minimum,
			EGYP(mtmp)->credit + 
#ifndef GOLDOBJ
													u.ugold);
#else
													umoney);													
#endif
	getlin(prompt, buf);
	(void) sscanf(buf, "%ld", &bet);

	/* Validate the amount */
	if (bet == 0L) {
		pline("Never mind.");
		return (0L);
	}
	if (bet < minimum) {
		You("must bet at least %ld.", minimum);
		return (0L);
	}
	if (bet > EGYP(mtmp)->credit +
#ifndef GOLDOBJ
								u.ugold) {
#else
								umoney) {												
#endif
		You("don't have that much money to bet!");
		return (0L);
	}
	return (bet);
}


/*** Card-related functions ***/

static const char *suits[CARD_SUITS] =
{ "swords", "wands",     "shields",  "rings" };          /* Special */
/* swords    wands/rods  roses/cups  pentacles/disks/coins  Tarot */
/* spade     bastoni     coppe       denari                 Italian */
/* swords    batons      cups        coins                  (translated) */
/* spades    clubs       hearts      diamonds               French */


static const char *ranks[CARD_RANKS] =
{ "ace", "2", "3", "4", "5", "6", "7", "8", "9", "10",
   /*none*/       "jack",       "queen", "king" }; /* French */
/* page/princess  knight/prince  queen    king        Tarot */


static const char *trumps[CARD_TRUMPS] =
{	"the Fool",               /* This is NOT a Joker */
	"the Magician",           /* same as the Magus */
	"the High Priestess",     /* sometimes placed after the Emperor */
#if 0
	"the Empress",            /* not included here */
	"the Emperor",            /* not included here */
#endif
	"the Oracle",             /* same as the Hierophant */
	"the Lovers",
	"the Chariot",
	"Strength",               /* sometimes Adjustment */
	"the Hermit",
	"the Wheel of Fortune",   /* sometimes Fortune */
	"Justice",                /* sometimes Lust */
	"Punishment",             /* replaces the Hanged Man */
	"the Devil",              /* normally #15 */
	"Sorcery",                /* replaces Art or Temperance */
	"Death",                  /* swapped with the Devil so it remains #13 */
	"the Tower",              /* really! */
	"the Star",
	"the Moon",
	"the Sun",
	"Judgement",              /* sometimes Aeon */
	"Infinity"                /* replaces the World or the Universe */
};


static void
card_shuffle (mtmp)
	struct monst *mtmp;
{
	xchar *cards = &EGYP(mtmp)->cards[0];
	int i, j, k;


	pline("%s shuffles the cards.", Monnam(mtmp));
	for (i = 0; i < CARD_TOTAL; i++)
		/* Initialize the value */
		cards[i] = i;
	for (i = 0; i < CARD_TOTAL; i++) {
		/* Swap this value with another randomly chosen one */
		j = rn2(CARD_TOTAL);
		k = cards[j];
		cards[j] = cards[i];
		cards[i] = k;
	}
	EGYP(mtmp)->top = CARD_TOTAL;
}

static xchar
card_draw (mtmp)
	struct monst *mtmp;
{
	if (EGYP(mtmp)->top <= 0)
		/* The deck is empty */
		return (-1);
	return (EGYP(mtmp)->cards[--EGYP(mtmp)->top]);
}

static void
card_name (num, buf)
	xchar num;
	char *buf;
{
	int r, s;


	if (!buf) return;
	if (Hallucination) num = rn2(CARD_TOTAL);
	if (num < 0 || num >= CARD_TOTAL) {
		/* Invalid card */
		impossible("no such card %d", num);
		Strcpy(buf, "a card");
	} else if (card_istrump(num)) {
		/* Handle trump cards */
		r = card_trump(num);
		if (!r)
			Sprintf(buf, "the zero of trumps (%s)", trumps[r]);
		else
			Sprintf(buf, "the %d of trumps (%s)", r, trumps[r]);
	} else {
		/* Handle suited cards */
		r = card_rank(num);
		s = card_suit(num);
		Sprintf(buf, "the %s of %s", ranks[r], suits[s]);
	}
	return;
}


/*** Fortunes ***/

#define FORTUNE_COST	50			/* Cost to play */

static short birthstones[12] =
{
	/* Jan */  GARNET,      /* Feb */  AMETHYST,
	/* Mar */  AQUAMARINE,  /* Apr */  DIAMOND,
	/* May */  EMERALD,     /* Jun */  OPAL,
	/* Jul */  RUBY,        /* Aug */  CHRYSOBERYL,
	/* Sep */  SAPPHIRE,    /* Oct */  BLACK_OPAL,
	/* Nov */  TOPAZ,       /* Dec */  TURQUOISE
};


static void
fortune_lev (mtmp, name, txt)
	struct monst *mtmp;
	char *name, *txt;
{
	/*** FIXME -- still very buggy ***/
/*	d_level *lev;*/
	schar dep;


	dep = lev_by_name(name);
	if (!dep) {
		/* Perhaps the level doesn't exist? */
		verbalize("The vision is hazy.");
		return;
	}

	if (dep == depth(&u.uz))
		verbalize("I see %s here.", txt);
	else {
		verbalize("I see %s on level %d.", txt, (int)dep);
/*		if (gypsy_offer(mtmp, 5000L, "teleport you there"))
			;*/
	}
	return;
}

static void
fortune (mtmp)
	struct monst *mtmp;
{
	xchar card;
	char buf[BUFSZ];
	short otyp;
	struct obj *otmp;


	/* Shuffle the deck, if neccessary, and draw a card */
	gypsy_charge(mtmp, FORTUNE_COST);
	if (EGYP(mtmp)->top <= 0)
		card_shuffle(mtmp);
	card = card_draw(mtmp);
#ifdef WIZARD
	if (wizard) {
		long t = -1;

		getlin("Which trump?", buf);
		(void) sscanf(buf, "%ld", &t);
		if (t >= 0) card = t + CARD_SUITED;
	}
#endif
	card_name(card, buf);
	verbalize("You have drawn %s.", buf);

	if (card_istrump(card))
		switch (card_trump(card)) {
		case 0:	/* the Fool */
			adjattrib(A_WIS, -1, 0);
			change_luck(-3);
			break;
		case 1:	/* the Magician */
			if (u.uevent.udemigod)
				resurrect();
			else
				fortune_lev(mtmp, "fakewiz1",
					"an entrance to the Wizard's tower");
				/*fortune_lev(mtmp, &portal_level);*/
			break;
		case 2: /* the High Priestess */
			if (u.uhave.amulet)
				verbalize("I see a high altar in the heavens.");
				/* Can only get there by ascending... */
			else
				verbalize("I see a high altar on level %d.",
						depth(&sanctum_level));
				/* Can only get there by invocation... */
			break;
		case 3: /* the Oracle */
			fortune_lev(mtmp, "oracle", "the Oracle");
			/*fortune_lev(mtmp, &oracle_level);*/
			break;
		case 4: /* the Lovers */
			makemon(&mons[flags.female ? PM_INCUBUS : PM_SUCCUBUS],
				u.ux, u.uy, 0);
			break;
		case 5: /* the Chariot */
			if (gypsy_offer(mtmp, 5000L,
					"teleport you to a level of your choosing")) {
				incr_itimeout(&HTeleport_control, 1);
				level_tele();
			}
			break;
		case 6: /* Strength */
			adjattrib(A_STR, 1, 0);
			incr_itimeout(&HHalf_physical_damage, rn1(500, 500));
			break;
		case 7: /* the Hermit */
			You_feel("like hiding!");
			incr_itimeout(&HTeleportation, rn1(300, 300));
			incr_itimeout(&HInvis, rn1(500, 500));
			newsym(u.ux, u.uy);
			break;
		case 8: /* the Wheel of Fortune */
			if (Hallucination)
				pline("Where is Vanna?");
			else
				You_feel("lucky!");
			if (u.uluck < 0)
				u.uluck = 0;
			else
				change_luck(3);
			break;
		case 9: /* Justice */
			makemon(&mons[PM_ERINYS], u.ux, u.uy, 0);
			break;
		case 10: /* Punishment */
			if (!Punished)
				punish((struct obj *)0);
			else
				rndcurse();
			break;
		case 11: /* the Devil */
			summon_minion(A_NONE, TRUE);
			break;
		case 12: /* Sorcery */
			adjattrib(urole.spelstat, 1, 0);
			incr_itimeout(&HHalf_spell_damage, rn1(500, 500));
			break;
		case 13: /* Death */
			if (nonliving(youmonst.data) || is_demon(youmonst.data) 
					|| Antimagic)
				shieldeff(u.ux, u.uy);
			else if(Hallucination)
				You("have an out of body experience.");
			else  {
				killer_format = KILLED_BY;
				killer = "the card of Death";
				done(DIED);
			}
			break;
		case 14: /* the Tower */
			fortune_lev(mtmp, "vlad\'s tower", "Vlad the Impaler");
			/* fortune_lev(mtmp, &vlad_level); */
			break;
		case 15: /* the Star */
			otyp = birthstones[getmonth()];
			makeknown(otyp);
			if ((otmp = mksobj(otyp, TRUE, FALSE)) != (struct obj *)0) {
				pline("%s reaches behind your %s and pulls out %s.",
						Monnam(mtmp), body_part(HEAD), doname(otmp));
				if (pickup_object(otmp, otmp->quan, FALSE) <= 0) {
					obj_extract_self(otmp);
					place_object(otmp, u.ux, u.uy);
					newsym(u.ux, u.uy);
				}
			}
			break;
		case 16: /* the Moon */
			/* Reset the old moonphase */
			if (flags.moonphase == FULL_MOON)
				change_luck(-1);

			/* Set the new moonphase */
			flags.moonphase = phase_of_the_moon();
			switch (flags.moonphase) {
				case NEW_MOON:
					pline("Be careful!  New moon tonight.");
					break;
				case 1:	case 2:	case 3:
					pline_The("moon is waxing tonight.");
					break;
				case FULL_MOON:
					You("are lucky!  Full moon tonight.");
					change_luck(1);
					break;
				case 5:	case 6:	case 7:
					pline_The("moon is waning tonight.");
					break;
				default:
					impossible("wierd moonphase %d", flags.moonphase);
					break;
			}
			break;
		case 17: /* the Sun */
			if (midnight())
				verbalize("It is the witching hour.  Beware of the undead!");
			else if (night())
				verbalize("It is nighttime.  Beware of creatures of the night!");
			else
				verbalize("It is daytime.  Shouldn't you be working?");
			break;
		case 18: /* Judgement */
			fortune_lev(mtmp, "portal to quest",
				"a portal to a quest");
			/* fortune_lev(mtmp, &quest_level); */
			break;
		case 19: /* Infinity */
			if (mtmp->mcan) {
				verbalize("I wish I wasn't here!");
				mongone(mtmp);
			} else if (gypsy_offer(mtmp, 10000L, "grant you a wish")) {
				mtmp->mcan = TRUE;
				makewish();
			}
			break;
		default:
			impossible("unknown trump %d", card_trump(card));
			break;
		}	/* End trumps */
	else
		/* Suited card */
		com_pager(QT_GYPSY + card);

	return;
}


/*** Three-card monte ***/

#define MONTE_COST	1			/* Minimum bet */
#define MONTE_MAX	10			/* Maximum value of monteluck */


static void
monte (mtmp)
	struct monst *mtmp;
{
	long bet, n;
	char buf[BUFSZ];
	winid win;
	anything any;
	menu_item *selected;
	int delta;


	/* Get the bet */
	bet = gypsy_bet(mtmp, MONTE_COST);
	if (!bet) return;

	/* Shuffle and pick */
	if (flags.verbose)
		pline("%s places three cards and rearranges them.", Monnam(mtmp));
	any.a_void = 0;	/* zero out all bits */
	win = create_nhwindow(NHW_MENU);
	start_menu(win);
	any.a_char = 'l';
	add_menu(win, NO_GLYPH, &any , 'l', 0, ATR_NONE,
			"Left card", MENU_UNSELECTED);
	any.a_char = 'c';
	add_menu(win, NO_GLYPH, &any , 'c', 0, ATR_NONE,
			"Center card", MENU_UNSELECTED);
	any.a_char = 'r';
	add_menu(win, NO_GLYPH, &any , 'r', 0, ATR_NONE,
			"Right card", MENU_UNSELECTED);
	end_menu(win, "Pick a card:");
	while (select_menu(win, PICK_ONE, &selected) != 1) ;
	destroy_nhwindow(win);

	/* Calculate the change in odds for next time */
	/* Start out easy, but get harder once the player is suckered */
	delta = rnl(4) - 3;	/* Luck helps */
	if (u.umontelast == selected[0].item.a_char)
		/* Only suckers keep picking the same card */
		delta++;
	u.umontelast = selected[0].item.a_char;
	for (n = bet; n > 0; n /= 10L)
		/* Penalize big bets */
		delta++;
/*	pline("luck = %d; delta = %d", u.umonteluck, delta);*/

	/* Did we win? */
	if (u.umonteluck <= rn2(MONTE_MAX)) {
		if (u.umonteluck == 0)
			verbalize("You win!  Wasn't that easy?");
		else
			verbalize("You win!");
		EGYP(mtmp)->credit += bet;

		/* Make it harder for next time */
		if (delta > 0) u.umonteluck += delta;
		if (u.umonteluck > MONTE_MAX) u.umonteluck = MONTE_MAX;
	} else {
		card_name(rn1(2, 1), buf);
		verbalize("Sorry, you picked %s.  Try again.", buf);
		gypsy_charge(mtmp, bet);

		/* Make it a little easier for next time */
		if (delta < 0) u.umonteluck += delta;
		if (u.umonteluck < 0) u.umonteluck = 0;
	}
	return;
}


/*** Ninety-nine ***/

#define NINETYNINE_COST		1	/* Minimum bet */
#define NINETYNINE_HAND		3	/* Number of cards in hand */
#define NINETYNINE_GOAL		99	/* Limit of the total */

static boolean
nn_playable (card, total)
	xchar card;
	int total;
{
	if (card_istrump(card))
		/* The fool always loses; other trumps are always playable */
		return (card != CARD_SUITED);
	switch (card_rank(card)+1) {
		case 11:	/* Jack */
		case 12:	/* Queen */
			return (total >= 10);
		case 13:	/* King */
			return (TRUE);
		default:	/* Ace through 10 */
			return ((total + card_rank(card) + 1) <= NINETYNINE_GOAL);
	}
}

static int
nn_play (card, total)
	xchar card;
	int total;
{
	if (card_istrump(card)) {
		if (card == CARD_SUITED)
			/* The Fool always loses */
			return (NINETYNINE_GOAL+1);
		else
			/* Other trumps leave the total unchanged */
			return (total);
	}
	switch (card_rank(card)+1) {
		case 11:	/* Jack */
		case 12:	/* Queen */
			return (total - 10);
		case 13:	/* King */
			return (NINETYNINE_GOAL);
		default:	/* Ace through 10 */
			return (total + card_rank(card) + 1);
	}
}

static int
nn_pref (card)
	xchar card;
{
	/* Computer's preferences for playing cards:
	 * 3.  Get rid of Ace through 10 whenever we can.  Highest priority.
	 * 2.  King will challenge the player.  High priority.
	 * 1.  Jack and queen may help us, or the hero.  Low priority. 
	 * 0.  Trumps can always be played (except the fool).  Lowest priority.
	 */
	if (card_istrump(card))
		/* The fool always loses; other trumps are always playable */
		return (0);
	switch (card_rank(card)+1) {
		case 11:	/* Jack */
		case 12:	/* Queen */
			return (1);
		case 13:	/* King */
			return (2);
		default:	/* Ace through 10 */
			return (3);
	}
}


static void
ninetynine (mtmp)
	struct monst *mtmp;
{
	long bet;
	int i, n, which, total = 0;
	xchar uhand[NINETYNINE_HAND], ghand[NINETYNINE_HAND];
	char buf[BUFSZ];
	winid win;
	anything any;
	menu_item *selected;


	/* Get the bet */
	bet = gypsy_bet(mtmp, NINETYNINE_COST);
	if (!bet) return;

	/* Shuffle the deck and deal */
	card_shuffle(mtmp);
	for (i = 0; i < NINETYNINE_HAND; i++) {
		uhand[i] = card_draw(mtmp);
		ghand[i] = card_draw(mtmp);
	}

	while (1) {
		/* Let the user pick a card */
		any.a_void = 0;	/* zero out all bits */
		win = create_nhwindow(NHW_MENU);
		start_menu(win);
		for (i = 0; i < NINETYNINE_HAND; i++) {
			any.a_int = (nn_playable(uhand[i], total) ? i+1 : 0);
			card_name(uhand[i], buf);
			add_menu(win, NO_GLYPH, &any , 0, 0, ATR_NONE,
					buf, MENU_UNSELECTED);
		}
		any.a_int = NINETYNINE_HAND + 1;
		add_menu(win, NO_GLYPH, &any , 'q', 0, ATR_NONE,
				"Forfeit", MENU_UNSELECTED);
		end_menu(win, "Play a card:");
		while (select_menu(win, PICK_ONE, &selected) != 1) ;
		destroy_nhwindow(win);

		/* Play the card */
		which = selected[0].item.a_int-1;
		if (which >= NINETYNINE_HAND) {
			You("forfeit.");
			gypsy_charge(mtmp, bet);
			return;
		}
		card_name(uhand[which], buf);
		total = nn_play(uhand[which], total);
		You("play %s for a total of %d.", buf, total);
		if (total < 0 || total > NINETYNINE_GOAL) {
			You("lose!");
			gypsy_charge(mtmp, bet);
			return;
		}

		/* Draw a new card */
		uhand[which] = card_draw(mtmp);
		if (uhand[which] < 0) {
			pline_The("deck is empty.  You win!");
			EGYP(mtmp)->credit += bet;
			return;
		}

		/* Let the gypsy pick a card */
		n = 0;
		for (i = 0; i < NINETYNINE_HAND; i++)
			if (nn_playable(ghand[i], total)) {
				/* The card is playable, but is it the best? */
				if (!n++ || nn_pref(ghand[i]) > nn_pref(ghand[which]))
					which = i;
			}
		if (!n) {
			/* No playable cards */
			pline("%s forfeits.  You win!", Monnam(mtmp));
			EGYP(mtmp)->credit += bet;
			return;
		}

		/* Play the card */
		card_name(ghand[which], buf);
		total = nn_play(ghand[which], total);
		pline("%s plays %s for a total of %d.", Monnam(mtmp), buf, total);

		/* Draw a new card */
		ghand[which] = card_draw(mtmp);
		if (ghand[which] < 0) {
			pline_The("deck is empty.  You win!");
			EGYP(mtmp)->credit += bet;
			return;
		}
	}

	return;
}



/*** Pawn gems ***/

STATIC_OVL NEARDATA const char pawnables[] = { ALLOW_COUNT, GEM_CLASS, 0 };

static void
pawn (mtmp)
	struct monst *mtmp;
{
	struct obj *otmp;
	long value;


	/* Prompt for an item */
	otmp = getobj((const char *)pawnables, "pawn");

	/* Is the item valid? */
	if (!otmp) return;
	if (!objects[otmp->otyp].oc_name_known) {
		/* Reject unknown objects */
		verbalize("Is this merchandise authentic?");
		return;
	}
	if (otmp->otyp < DILITHIUM_CRYSTAL || otmp->otyp > LAST_GEM) {
		/* Reject glass */
		verbalize("Don\'t bother with that junk!");
		return;
	}

	/* Give the credit */
	value = otmp->quan * objects[otmp->otyp].oc_cost;
	pline("%s gives you %ld zorkmid%s credit.", Monnam(mtmp),
			value, plur(value));
	EGYP(mtmp)->credit += value;

	/* Gypsies don't keep merchandise; it could get stolen! */
	otmp->quan = 1L;
	useup(otmp);
	return;
}


/*** Yendorian Tarocchi ***/

#define TAROCCHI_COST	500		/* Cost to play */
#define TAROCCHI_HAND	10		/* Number of cards in hand */

static void
tarocchi (mtmp)
	struct monst *mtmp;
{
	int turn;

	/* Shuffle the deck and deal */
	gypsy_charge(mtmp, TAROCCHI_COST);
	card_shuffle(mtmp);

	/* Play the given number of turns */
	for (turn = TAROCCHI_HAND; turn > 0; turn--) {
	}

	return;
}


/*** Monster-related functions ***/

void
gypsy_init (mtmp)
	struct monst *mtmp;
{
	mtmp->isgyp = TRUE;
	mtmp->mpeaceful = TRUE;
	mtmp->msleeping = 0;
	mtmp->mtrapseen = ~0;	/* traps are known */
	EGYP(mtmp)->credit = 0L;
	EGYP(mtmp)->top = 0;
	return;
}


void
gypsy_chat (mtmp)
	struct monst *mtmp;
{
	long money;
	winid win;
	anything any;
	menu_item *selected;
#ifdef GOLDOBJ
	long umoney;
#endif
	int n;

#ifdef GOLDOBJ
	umoney = money_cnt(invent);
#endif

	/* Sanity checks */
	if (!mtmp || !mtmp->mpeaceful || !mtmp->isgyp ||
			!humanoid(mtmp->data))
		return;

	/* Add up your available money */
	You("have %ld zorkmid%s credit and are carrying %ld zorkmid%s.",
			EGYP(mtmp)->credit, plur(EGYP(mtmp)->credit),
#ifndef GOLDOBJ
			u.ugold, plur(u.ugold));
#else
			umoney, plur(umoney));			
#endif
	money = EGYP(mtmp)->credit +
#ifndef GOLDOBJ
											u.ugold;
#else
											umoney;
#endif

	/* Create the menu */
	any.a_void = 0;	/* zero out all bits */
	win = create_nhwindow(NHW_MENU);
	start_menu(win);

	/* Fortune */
	any.a_char = 'f';
	if (money >= FORTUNE_COST)
		add_menu(win, NO_GLYPH, &any , 'f', 0, ATR_NONE,
				"Read your fortune", MENU_UNSELECTED);

	/* Three-card monte */
	any.a_char = 'm';
	if (money >= MONTE_COST)
		add_menu(win, NO_GLYPH, &any , 'm', 0, ATR_NONE,
				"Three-card monte", MENU_UNSELECTED);

	/* Ninety-nine */
	any.a_char = 'n';
	if (money >= NINETYNINE_COST)
		add_menu(win, NO_GLYPH, &any , 'n', 0, ATR_NONE,
				"Ninety-nine", MENU_UNSELECTED);

	/* Pawn gems (always available) */
	any.a_char = 'p';
	add_menu(win, NO_GLYPH, &any , 'p', 0, ATR_NONE,
			"Pawn gems", MENU_UNSELECTED);

	/* Yendorian Tarocchi */
	any.a_char = 't';
/*	if (money >= TAROCCHI_COST)
		add_menu(win, NO_GLYPH, &any , 't', 0, ATR_NONE,
				"Yendorian Tarocchi", MENU_UNSELECTED);*/

	/* Help */
	any.a_char = '?';
		add_menu(win, NO_GLYPH, &any , '?', 0, ATR_NONE,
				"Help", MENU_UNSELECTED);

	/* Display the menu */
	end_menu(win, "Play which game?");
	n = select_menu(win, PICK_ONE, &selected);
	destroy_nhwindow(win);
	if (n > 0) switch (selected[0].item.a_char) {
		case 'f':
			fortune(mtmp);
			break;
		case 'm':
			monte(mtmp);
			break;
		case 'n':
			ninetynine(mtmp);
			break;
		case 'p':
			pawn(mtmp);
			break;
		case 't':
			tarocchi(mtmp);
			break;
		case '?':
			display_file_area(FILE_AREA_SHARE, "gypsy.txt", TRUE);
			break;
	}

	return;
}


