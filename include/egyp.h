/*** egyp.h ***/

#ifndef EGYP__H
#define EGYP__H


/*** Card definitions ***/
#define CARD_SUITS		4		/* Number of suits */
#define CARD_RANKS		13		/* Number of cards in each suit */
#define CARD_TRUMPS		20		/* Number of trump cards */
#define CARD_SUITED		(CARD_SUITS * CARD_RANKS)
#define CARD_FOOL		CARD_SUITED
#define CARD_TOTAL		(CARD_SUITED + CARD_TRUMPS)

#define card_istrump(c)	((c) >= CARD_SUITED)
#define card_suit(c)	((c) / CARD_RANKS)
#define card_rank(c)	((c) % CARD_RANKS)
#define card_trump(c)	((c) - CARD_SUITED)


/*** Monster definitions ***/
struct egyp {
	long credit;				/* Amount credited to player */
	int top;					/* Index of top of the deck */
	xchar cards[CARD_TOTAL];	/* Shuffled cards */
};
#define EGYP(mon)	((struct egyp *)&(mon)->mextra[0])


#endif /* EGYP__H */


