/*	SCCS Id: @(#)botl.c	3.4	1996/07/15	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

#ifdef OVL0
extern const char *hu_stat[];	/* defined in eat.c */

const char *hu_abbrev_stat[] = {	/* must be kept consistent with eat.c */
	"Sat",
	"",
	"Hun",
	"Wea",
	"Ftg",
	"Ftd",
	"Sta"
};

const char * const enc_stat[] = {
	"",
	"Burdened",
	"Stressed",
	"Strained",
	"Overtaxed",
	"Overloaded"
};

const char *enc_abbrev_stat[] = {
	"",
	"Brd",
	"Ssd",
	"Snd",
	"Otd",
	"Old"
};

STATIC_DCL void NDECL(bot1);
STATIC_DCL void NDECL(bot2);
#ifdef ALLEG_FX
STATIC_DCL void FDECL(set_botl_warn, (int));
#endif
#endif /* OVL0 */

/* MAXCO must hold longest uncompressed status line, and must be larger
 * than COLNO
 *
 * longest practical second status line at the moment is
 *	Astral Plane $:12345 HP:700(700) Pw:111(111) AC:-127 Xp:30/123456789
 *      Wt:5000/1000 T:123456 Satiated Lev Conf FoodPois Ill Blind Stun Hallu
 *      Slime Held Overloaded
 * -- or somewhat over 160 characters
 */
#if COLNO <= 170
#define MAXCO 190
#else
#define MAXCO (COLNO+20)
#endif

#ifndef OVLB
STATIC_DCL int mrank_sz;
#else /* OVLB */
STATIC_OVL NEARDATA int mrank_sz = 0; /* loaded by max_rank_sz (from u_init) */
#endif /* OVLB */

STATIC_DCL const char *NDECL(rank);

#ifdef OVL1

#ifdef ALLEG_FX
static int botl_warn = 0;

static void
set_botl_warn(level)
int level;
{
        botl_warn = level;
        flags.botl = 1;
}
#endif

/* convert experience level (1..30) to rank index (0..8) */
int
xlev_to_rank(xlev)
int xlev;
{
	return (xlev <= 2) ? 0 : (xlev <= 30) ? ((xlev + 2) / 4) : 8;
}

#if 0	/* not currently needed */
/* convert rank index (0..8) to experience level (1..30) */
int
rank_to_xlev(rank)
int rank;
{
	return (rank <= 0) ? 1 : (rank <= 8) ? ((rank * 4) - 2) : 30;
}
#endif

const char *
rank_of(lev, monnum, female)
int lev;
	short monnum;
boolean female;
{
	register struct Role *role;
	register int i;


	/* Find the role */
	for (role = (struct Role *) roles; role->name.m; role++)
	    if (monnum == role->malenum || monnum == role->femalenum)
	    	break;
	if (!role->name.m)
	    role = &urole;

	/* Find the rank */
	for (i = xlev_to_rank((int)lev); i >= 0; i--) {
	    if (female && role->rank[i].f) return (role->rank[i].f);
	    if (role->rank[i].m) return (role->rank[i].m);
	}

	/* Try the role name, instead */
	if (female && role->name.f) return (role->name.f);
	else if (role->name.m) return (role->name.m);
	return ("Player");
}


STATIC_OVL const char *
rank()
{
	return(rank_of(u.ulevel, Role_switch, flags.female));
}

int
title_to_mon(str, rank_indx, title_length)
const char *str;
int *rank_indx, *title_length;
{
	register int i, j;


	/* Loop through each of the roles */
	for (i = 0; roles[i].name.m; i++)
	    for (j = 0; j < 9; j++) {
	    	if (roles[i].rank[j].m && !strncmpi(str,
	    			roles[i].rank[j].m, strlen(roles[i].rank[j].m))) {
	    	    if (rank_indx) *rank_indx = j;
	    	    if (title_length) *title_length = strlen(roles[i].rank[j].m);
	    	    return roles[i].malenum;
	    	}
	    	if (roles[i].rank[j].f && !strncmpi(str,
	    			roles[i].rank[j].f, strlen(roles[i].rank[j].f))) {
	    	    if (rank_indx) *rank_indx = j;
	    	    if (title_length) *title_length = strlen(roles[i].rank[j].f);
	    	    return ((roles[i].femalenum != NON_PM) ?
	    	    		roles[i].femalenum : roles[i].malenum);
	    	}
	    }
	return NON_PM;
}

#endif /* OVL1 */
#ifdef OVLB

void
max_rank_sz()
{
	register int i, r, maxr = 0;


	for (i = 0; i < 9; i++) {
	    if (urole.rank[i].m && (r = strlen(urole.rank[i].m)) > maxr) maxr = r;
	    if (urole.rank[i].f && (r = strlen(urole.rank[i].f)) > maxr) maxr = r;
	}
	mrank_sz = maxr;
	return;
}

#endif /* OVLB */
#ifdef OVL0

#ifdef SCORE_ON_BOTL
long
botl_score()
{
    int deepest = deepest_lev_reached(FALSE);
#ifndef GOLDOBJ
    long ugold = u.ugold + hidden_gold();

    if ((ugold -= u.ugold0) < 0L) ugold = 0L;
    return ugold + u.urexp + (long)(50 * (deepest - 1))
#else
    long umoney = money_cnt(invent) + hidden_gold();

    if ((umoney -= u.umoney0) < 0L) umoney = 0L;
    return umoney + u.urexp + (long)(50 * (deepest - 1))
#endif
			  + (long)(deepest > 30 ? 10000 :
				   deepest > 20 ? 1000*(deepest - 20) : 0);
}
#endif

static char *
botl_player()
{
    static char player[MAXCO];
	register char *nb;
    char mbot[MAXCO - 15];
    int k = 0;

    Strcpy(player, plname);
    if ('a' <= player[0] && player[0] <= 'z') player[0] += 'A'-'a';
    player[10] = 0;
    Sprintf(nb = eos(player)," the ");

	if (Upolyd) {
	(void) strncpy(mbot, mons[u.umonnum].mname, SIZE(mbot) - 1);
	mbot[SIZE(mbot) - 1] = 0;
		while(mbot[k] != 0) {
		    if ((k == 0 || (k > 0 && mbot[k-1] == ' ')) &&
					'a' <= mbot[k] && mbot[k] <= 'z')
			mbot[k] += 'A' - 'a';
		    k++;
		}
	Sprintf(eos(nb), mbot);
	} else
	Sprintf(eos(nb), rank());
    return player;
}

static char *
botl_strength()
{
    static char strength[6];
	if (ACURR(A_STR) > 18) {
		if (ACURR(A_STR) > STR18(100))
	    Sprintf(strength, "%2d", ACURR(A_STR)-100);
		else if (ACURR(A_STR) < STR18(100))
	    Sprintf(strength, "18/%02d", ACURR(A_STR)-18);
		else
	    Sprintf(strength, "18/**");
	} else
	Sprintf(strength, "%-1d", ACURR(A_STR));
    return strength;
}

STATIC_OVL void
bot1()
{
	char newbot1[MAXCO];
	register char *nb;
	register int i,j;

	Strcpy(newbot1, botl_player());
	Sprintf(nb = eos(newbot1),"  ");
	i = mrank_sz + 15;
	j = (nb + 2) - newbot1; /* aka strlen(newbot1) but less computation */
	if((i - j) > 0)
		Sprintf(nb = eos(nb),"%*s", i-j, " ");  /* pad with spaces */
        
	Sprintf(nb = eos(nb), "St:%s ", botl_strength());
	Sprintf(nb = eos(nb),
		"Dx:%-1d Co:%-1d In:%-1d Wi:%-1d Ch:%-1d",
		ACURR(A_DEX), ACURR(A_CON), ACURR(A_INT), ACURR(A_WIS), ACURR(A_CHA));
	Sprintf(nb = eos(nb), (u.ualign.type == A_CHAOTIC) ? "  Chaotic" :
			(u.ualign.type == A_NEUTRAL) ? "  Neutral" : "  Lawful");
#ifdef SCORE_ON_BOTL
	if (flags.showscore)
	    Sprintf(nb = eos(nb), " S:%ld", botl_score());
#endif
	curs(WIN_STATUS, 1, 0);
	putstr(WIN_STATUS, 0, newbot1);
}

/* provide the name of the current level for display by various ports */
int
describe_level(buf, verbose)
char *buf;
int verbose;
{
	int ret = 1;

	/* TODO:	Add in dungeon name */
	if (Is_knox(&u.uz))
		Sprintf(buf, "%s ", dungeons[u.uz.dnum].dname);
	else if (In_quest(&u.uz))
		Sprintf(buf, "Home %d ", dunlev(&u.uz));
	else if (In_endgame(&u.uz))
		Sprintf(buf,
			Is_astralevel(&u.uz) ? "Astral Plane " : "End Game ");
	else {
		if (verbose)
			Sprintf(buf, "%s, level %d ",
				dungeons[u.uz.dnum].dname, depth(&u.uz));
		else
		Sprintf(buf, "Dlvl:%-2d ", depth(&u.uz));
		ret = 0;
	}
	return ret;
}

/* [ALI] Line 2 abbreviation levels:
 *	0 - No abbreviation
 *	1 - Omit gold
 *	2 - Abbreviated status tags
 *	3 - Disable show options
 *	4 - Omit dungeon level
 *
 * We omit gold first since the '$' command is always available.
 *
 * While the abbreviated status tags are very difficult to interpret, we use
 * these before disabling the show options on the basis that the user always
 * has the choice of turning the show options off if that would be preferable.
 *
 * Last to go is the dungeon level on the basis that there is no way of
 * finding this information other than via the status line.
 */

static int bot2_abbrev = 0;	/* Line 2 abbreviation level (max 4) */

STATIC_OVL void
bot2str(char *newbot2)
{
	register char *nb;
	int hp, hpmax;
	int cap = near_capacity();
#ifdef ALLEG_FX
	int w;
#endif

	hp = Upolyd ? u.mh : u.uhp;
	hpmax = Upolyd ? u.mhmax : u.uhpmax;

	if(hp < 0) hp = 0;
	if (bot2_abbrev < 4)
		(void) describe_level(newbot2, FALSE);
	else
		newbot2[0] = '\0';
	if (bot2_abbrev < 1)
		Sprintf(nb = eos(newbot2), "%c:%-2ld ",
		  oc_syms[COIN_CLASS],
#ifndef GOLDOBJ
		u.ugold
#else
		money_cnt(invent)
#endif
		  );
	else
		nb = newbot2;
	Sprintf(nb = eos(nb), "HP:%d(%d) Pw:%d(%d) AC:%-2d",
		hp, hpmax, u.uen, u.uenmax, u.uac);

	if (Upolyd)
		Sprintf(nb = eos(nb), " HD:%d", ((u.ulycn == u.umonnum) ? 
						u.ulevel : mons[u.umonnum].mlevel));
#ifdef EXP_ON_BOTL
	else if(flags.showexp && bot2_abbrev < 3)
		Sprintf(nb = eos(nb), " Xp:%u/%-1ld", u.ulevel,u.uexp);
#endif
	else
		Sprintf(nb = eos(nb), " Exp:%u", u.ulevel);

#ifdef SHOW_WEIGHT
	if (flags.showweight && bot2_abbrev < 3)
		Sprintf(nb = eos(nb), "  Wt:%ld/%ld", (long)(inv_weight()+weight_cap()),
				(long)weight_cap());
#endif

	if(flags.time && bot2_abbrev < 3)
	        Sprintf(nb = eos(nb), "  T:%ld ", moves);

#ifdef ALLEG_FX
        if(iflags.usealleg && botl_warn && !Hallucination)
        {
            Sprintf(nb = eos(nb), " ");
            for(w = 0; w < botl_warn; w++)
                Sprintf(nb = eos(nb), "!");
        }
#endif
	        
        if (bot2_abbrev >= 2) {
		if (hu_abbrev_stat[u.uhs][0]!='\0') {
			Sprintf(nb = eos(nb), " ");
			Strcat(newbot2, hu_abbrev_stat[u.uhs]);
		}
	}
	else if(strcmp(hu_stat[u.uhs], "        ")) {
		Sprintf(nb = eos(nb), " ");
		Strcat(newbot2, hu_stat[u.uhs]);
	}

/* WAC further Up
#ifdef SCORE_ON_BOTL
	if (flags.showscore)
                Sprintf(nb,"%c:%-2ld  Score:%ld", oc_syms[COIN_CLASS],
                   u.ugold, botl_score());
#endif
*/
	/* KMH -- changed to Lev */
	if (Levitation)    Sprintf(nb = eos(nb), " Lev");
	if(Confusion)
		Sprintf(nb = eos(nb), bot2_abbrev >= 2 ? " Cnf" : " Conf");
	if(Sick) {
		if (u.usick_type & SICK_VOMITABLE)
			   Sprintf(nb = eos(nb),
			     bot2_abbrev >= 2 ? " FPs" : " FoodPois");
		if (u.usick_type & SICK_NONVOMITABLE)
			   Sprintf(nb = eos(nb), " Ill");
	}

	if(Blind)
		Sprintf(nb = eos(nb), bot2_abbrev >= 2 ? " Bnd" : " Blind");
	if(Stunned)
		Sprintf(nb = eos(nb), bot2_abbrev >= 2 ? " Stn" : " Stun");
	if(Hallucination)
		Sprintf(nb = eos(nb), bot2_abbrev >= 2 ? " Hal" : " Hallu");
	if(Slimed)
		Sprintf(nb = eos(nb), bot2_abbrev >= 2 ? " Slm" : " Slime");
	if(u.ustuck && !u.uswallow && !sticks(youmonst.data))
		Sprintf(nb = eos(nb), " Held");
	if(cap > UNENCUMBERED)
		Sprintf(nb = eos(nb), " %s",
		  bot2_abbrev >= 2 ? enc_abbrev_stat[cap] : enc_stat[cap]);
}

STATIC_OVL void
bot2()
{
	char  newbot2[MAXCO];

	bot2str(newbot2);
	curs(WIN_STATUS, 1, 1);

	putstr(WIN_STATUS, 0, newbot2);
	return;
}

/* WAC -- Shorten bot1 to fit in len spaces.
 * Not currently used
 * Longest string past Str: is
 * ". Str:18/99 Dx:11 Co:13 In:12 Wi:14 Ch:14 Neutral" or 49 Chars long.
 */
#if 0
const char*
shorten_bot1(str, len)
const char *str;
int len;
{
    static char cbuf[BUFSZ];

    register const char *bp0 = str;
    register char *bp1 = cbuf;
    int k = 0;

    do {
            *bp1++ = *bp0;
            k++;
    } while(*bp0++ && k < (len - 49));
    
    cbuf[k] = '.';
    bp1++;
    
    bp0 = index(str, ':') - 3;
    do {
            *bp1++ = *bp0;
    } while(*bp0++);
    return cbuf;
}
#endif /* 0 */

/* ALI -- Shorten bot2 to fit in len spaces.
 * Currently only used by tty port
 * After the forth attempt the longest practical bot2 becomes:
 *      HP:700(700) Pw:111(111) AC:-127 Exp:30
 *      Sat Lev Cnf FPs Ill Bnd Stn Hal Slm Old
 * -- or just under 80 characters
 */
#ifdef TTY_GRAPHICS
const char*
shorten_bot2(str, len)
const char *str;
unsigned int len;
{
    static char cbuf[MAXCO];
    for(bot2_abbrev = 1; bot2_abbrev <= 4; bot2_abbrev++) {
	bot2str(cbuf);
	if (strlen(cbuf) <= len)
	    break;
    }
    if (bot2_abbrev > 4)
	cbuf[len] = '\0';	/* If all else fails, truncate the line */
    bot2_abbrev = 0;
    return cbuf;
}
#endif /* TTY_GRAPHICS */

static void (*raw_handler)();

static void bot_raw(reconfig)
boolean reconfig;
{
    const char *botl_raw_values[24], **rv = botl_raw_values;
    char dex[3], con[3], itl[3], wis[3], cha[3], score[21];
    int uhp;
    char dlevel[BUFSZ];
    char hp[21], hpmax[21], pw[21], pwmax[21], gold[21], ac[21], elevel[21];
    char expr[21], iweight[21], capacity[21], flgs[21], tim[21];
    *rv++ = reconfig ? "player" : botl_player();
    *rv++ = reconfig ? "strength" : botl_strength();
    *rv++ = reconfig ? "dexterity" : (Sprintf(dex, "%d", ACURR(A_DEX)), dex);
    *rv++ = reconfig ? "constitution" : (Sprintf(con, "%d", ACURR(A_CON)), con);
    *rv++ = reconfig ? "intelligence" : (Sprintf(itl, "%d", ACURR(A_INT)), itl);
    *rv++ = reconfig ? "wisdom" : (Sprintf(wis, "%d", ACURR(A_WIS)), wis);
    *rv++ = reconfig ? "charisma" : (Sprintf(cha, "%d", ACURR(A_CHA)), cha);
    *rv++ = reconfig ? "alignment" : u.ualign.type == A_CHAOTIC ? "Chaotic" :
	    u.ualign.type == A_NEUTRAL ? "Neutral" : "Lawful";
#ifdef SCORE_ON_BOTL
    if (flags.showscore)
	*rv++ = reconfig ? "score" :
		(Sprintf(score, "%ld", botl_score()), score);
#endif
    uhp = Upolyd ? u.mh : u.uhp;
    if (uhp < 0) uhp = 0;
    (void) describe_level(dlevel, TRUE);
    eos(dlevel)[-1] = 0;
    *rv++ = reconfig ? "dlevel" : dlevel;
    *rv++ = reconfig ? "gold" : (Sprintf(gold, "%ld",
#ifndef GOLDOBJ
    u.ugold
#else
	money_cnt(invent)
#endif
    ), gold);
    *rv++ = reconfig ? "hp" : (Sprintf(hp, "%d", uhp), hp);
    *rv++ = reconfig ? "hpmax" :
	    (Sprintf(hpmax, "%d", Upolyd ? u.mhmax : u.uhpmax), hpmax);
    *rv++ = reconfig ? "pw" : (Sprintf(pw, "%d", u.uen), pw);
    *rv++ = reconfig ? "pwmax" : (Sprintf(pwmax, "%d", u.uenmax), pwmax);
    *rv++ = reconfig ? "ac" : (Sprintf(ac, "%d", u.uac), ac);
    Sprintf(elevel, "%u",
	    Upolyd && u.ulycn != u.umonnum ? mons[u.umonnum].mlevel : u.ulevel);
    *rv++ = reconfig ? (Upolyd ? "hitdice" : "elevel") : elevel;
#ifdef EXP_ON_BOTL
    if (flags.showexp)
	*rv++ = reconfig ? "experience" : (Sprintf(expr, "%ld", u.uexp), expr);
#endif
#ifdef SHOW_WEIGHT
    if (flags.showweight) {
	*rv++ = reconfig ? "weight" : (Sprintf(iweight,
		"%ld", (long)(inv_weight() + weight_cap())), iweight);
	*rv++ = reconfig ? "capacity" : (Sprintf(capacity,
		"%ld", (long)weight_cap()), capacity);
    }
#endif
    if (flags.time)
	*rv++ = reconfig ? "time" : (Sprintf(tim, "%ld", moves), tim);
    *rv++ = reconfig ? "hunger" : strcmp(hu_stat[u.uhs], "        ") ?
	    hu_stat[u.uhs] : "";
    *rv++ = reconfig ? "encumberance" : enc_stat[near_capacity()];
    *rv++ = reconfig ? "flags" : (Sprintf(flgs, "%lX",
        (Levitation ? RAW_STAT_LEVITATION : 0) |
	(Confusion ? RAW_STAT_CONFUSION : 0) |
	(Sick && (u.usick_type & SICK_VOMITABLE) ? RAW_STAT_FOODPOIS : 0) |
	(Sick && (u.usick_type & SICK_NONVOMITABLE) ? RAW_STAT_ILL : 0) |
	(Blind ? RAW_STAT_BLIND : 0) |
	(Stunned ? RAW_STAT_STUNNED : 0) |
	(Hallucination ? RAW_STAT_HALLUCINATION : 0) |
	(Slimed ? RAW_STAT_SLIMED : 0)), flgs);
    (*raw_handler)(reconfig, rv - botl_raw_values, botl_raw_values);
}

void bot_reconfig()
{
    if (raw_handler)
	bot_raw(TRUE);
    flags.botl = 1;
}

void
bot_set_handler(handler)
void (*handler)();
{
    raw_handler = handler;
    bot_reconfig();
}

void
bot()
{
	/*
	 * ALI: Cope with the fact that u_init may not have been
	 * called yet. This happens if the player selection menus
	 * are long enough to overwite the status line. In this
	 * case we will be called when the menu is removed while
	 * youmonst.data is still NULL.
	 */
	if (!youmonst.data)
		return;
	if (raw_handler)
		bot_raw(FALSE);
	else {
	bot1();
	bot2();
	}
	flags.botl = flags.botlx = 0;
}

#endif /* OVL0 */

/*botl.c*/
