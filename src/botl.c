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
#define MAXCO 240
#else
#define MAXCO (COLNO+70)
#endif

#if defined(STATUS_COLORS) && defined(TEXTCOLOR)

extern const struct percent_color_option *hp_colors;
extern const struct percent_color_option *pw_colors;
extern const struct text_color_option *text_colors;

struct color_option
text_color_of(text, color_options)
const char *text;
const struct text_color_option *color_options;
{
	if (color_options == NULL) {
		struct color_option result = {NO_COLOR, 0};
		return result;
	}
	if (strstri(color_options->text, text)
	 || strstri(text, color_options->text))
		return color_options->color_option;
	return text_color_of(text, color_options->next);
}

struct color_option
percentage_color_of(value, max, color_options)
int value, max;
const struct percent_color_option *color_options;
{
	if (color_options == NULL) {
		struct color_option result = {NO_COLOR, 0};
		return result;
	}
	if (100 * value <= color_options->percentage * max)
		return color_options->color_option;
	return percentage_color_of(value, max, color_options->next);
}

void
start_color_option(color_option)
struct color_option color_option;
{
	int i;
	if (color_option.color != NO_COLOR)
		term_start_color(color_option.color);
	for (i = 0; (1 << i) <= color_option.attr_bits; ++i)
		if (i != ATR_NONE && color_option.attr_bits & (1 << i))
			term_start_attr(i);
}

void
end_color_option(color_option)
struct color_option color_option;
{
	int i;
	if (color_option.color != NO_COLOR)
		term_end_color(color_option.color);
	for (i = 0; (1 << i) <= color_option.attr_bits; ++i)
		if (i != ATR_NONE && color_option.attr_bits & (1 << i))
			term_end_attr(i);
}

static
void
apply_color_option(color_option, newbot2, statusline)
struct color_option color_option;
const char *newbot2;
int statusline; /* apply color on this statusline: 1 or 2 */
{
	if (!iflags.use_status_colors) return;
	curs(WIN_STATUS, 1, statusline-1);
	start_color_option(color_option);
	putstr(WIN_STATUS, 0, newbot2);
	end_color_option(color_option);
}

void
add_colored_text(text, newbot2)
const char *text;
char *newbot2;
{
	char *nb;
	struct color_option color_option;

	if (*text == '\0') return;

	if (!iflags.use_status_colors) {
		Sprintf(nb = eos(newbot2), " %s", text);
                return;
        }

	Strcat(nb = eos(newbot2), " ");
	curs(WIN_STATUS, 1, 1);
	putstr(WIN_STATUS, 0, newbot2);

	Strcat(nb = eos(nb), text);
	curs(WIN_STATUS, 1, 1);
       	color_option = text_color_of(text, text_colors);
	start_color_option(color_option);
	putstr(WIN_STATUS, 0, newbot2);
	end_color_option(color_option);
}

#endif

#ifndef OVLB
STATIC_DCL int mrank_sz;
#else /* OVLB */
STATIC_OVL NEARDATA int mrank_sz = 0; /* loaded by max_rank_sz (from u_init) */
#endif /* OVLB */

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


const char *
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

static char *
botl_player()
{
    static char player[MAXCO];
    char *nb;
    int i=0,k;
    char mbot[MAXCO - 15];

    Strcpy(player, "");
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
    if (flags.hitpointbar) {
        flags.botlx = 0;
        curs(WIN_STATUS, 1, 0);
        putstr(WIN_STATUS, 0, player);
        Strcat(player, "[");
        i = 1; /* don't overwrite the string in front */
        curs(WIN_STATUS, 1, 0);
        putstr(WIN_STATUS, 0, player);
    }
#endif
       Strcat(player, plname);
       if('a' <= player[i] && player[i] <= 'z') player[i] += 'A'-'a';
       player[10] = '\0';
    Sprintf(nb = eos(player)," the ");

	if (Upolyd && !missingnoprotect) {

	(void) strncpy(mbot, mons[u.umonnum].mname, SIZE(mbot) - 1);

	mbot[/*SIZE(mbot) - 1*/80] = 0;
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

#ifdef DUMP_LOG
void bot1str(char *newbot1)
#else
STATIC_OVL void
bot1()
#endif
{
#ifndef DUMP_LOG
	char newbot1[MAXCO];
#endif
	register char *nb;
	register int i,j;

	Strcpy(newbot1, botl_player());
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
        if (flags.hitpointbar) {
            int bar_length = strlen(newbot1)-1;
            char tmp[MAXCO];
            char *p = tmp;
            /* filledbar >= 0 and < MAXCO */
            int hp = (uhp() < 0) ? 0 : uhp();
            int filledbar = (uhpmax() > 0) ? (hp * bar_length / uhpmax()) : 0;
            if (filledbar >= MAXCO) { filledbar = MAXCO-1; }
            Strcpy(tmp, newbot1);
            p++;

            /* draw hp bar */
            if (iflags.use_inverse) term_start_attr(ATR_INVERSE);
            p[filledbar] = '\0';
            apply_color_option(percentage_color_of(uhp(), uhpmax(), hp_colors), tmp, 1);
            term_end_color();
            if (iflags.use_inverse) term_end_attr(ATR_INVERSE);

            Strcat(newbot1, "]");
    }
#endif

	Sprintf(nb = eos(newbot1),"  ");
	i = mrank_sz + 15;
	j = (nb + 2) - newbot1; /* aka strlen(newbot1) but less computation */
	/*if((i - j) > 0)
		Sprintf(nb = eos(nb),"%*s", i-j, " ");   pad with spaces */
        
	Sprintf(nb = eos(nb), "St%s ", botl_strength());
	Sprintf(nb = eos(nb),
		"Dx%-1d Co%-1d In%-1d Wi%-1d Ch%-1d ",
		ACURR(A_DEX), ACURR(A_CON), ACURR(A_INT), ACURR(A_WIS), ACURR(A_CHA));
	Sprintf(nb = eos(nb), urole.filecode); /* fully disclosing what character you're playing */
	Sprintf(nb = eos(nb), urace.filecode); /* abbreviated so the line doesn't roll over --Amy */
	Sprintf(nb = eos(nb), flags.female ? "Fem" : "Mal"); /* allowing you to always know what you are */
	Sprintf(nb = eos(nb), (u.ualign.type == A_CHAOTIC) ? "Cha" :
			(u.ualign.type == A_NEUTRAL) ? "Neu" : "Law");

	/* abbreviate hybridizations since we don't have infinite space on the status line --Amy */
	if (flags.hybridization) {Sprintf(nb = eos(nb), "+");
		if (flags.hybridcurser) Sprintf(nb = eos(nb), "C");
		if (flags.hybridhaxor) Sprintf(nb = eos(nb), "H");
		if (flags.hybridangbander) Sprintf(nb = eos(nb), "A");
		if (flags.hybridhomicider) Sprintf(nb = eos(nb), "O");
		if (flags.hybridsuxxor) Sprintf(nb = eos(nb), "S");
		/* sorry but I just couldn't resist allowing the "CHAOS" string. :D */
		if (flags.hybridaquarian) Sprintf(nb = eos(nb), "Q");
		if (flags.hybridwarper) Sprintf(nb = eos(nb), "W");
		if (flags.hybridrandomizer) Sprintf(nb = eos(nb), "R");
		if (flags.hybridnullrace) Sprintf(nb = eos(nb), "N");
		if (flags.hybridmazewalker) Sprintf(nb = eos(nb), "M");
		if (flags.hybridsoviet) Sprintf(nb = eos(nb), "V");
		if (flags.hybridxrace) Sprintf(nb = eos(nb), "X");
		if (flags.hybridspecialist) Sprintf(nb = eos(nb), "P");
		if (flags.hybridminimalist) Sprintf(nb = eos(nb), "L");
		if (flags.hybridamerican) Sprintf(nb = eos(nb), "I");
		if (flags.hybridsokosolver) Sprintf(nb = eos(nb), "K");
		if (flags.hybridheretic) Sprintf(nb = eos(nb), "E");
		if (flags.hybridnastinator) Sprintf(nb = eos(nb), "Y");
		if (flags.hybridrougelike) Sprintf(nb = eos(nb), "U");
	}

	if (flags.showscore)
	    Sprintf(nb = eos(nb), " S%ld", botl_score());
#ifdef DUMP_LOG
}
STATIC_OVL void
bot1()
{
	char newbot1[MAXCO];
	int save_botlx = flags.botlx;

	bot1str(newbot1);
#endif
	curs(WIN_STATUS, 1, 0);
	putstr(WIN_STATUS, 0, newbot1);
	flags.botlx = save_botlx;
}

/* provide the name of the current level for display by various ports */
int
describe_level(buf, verbose)
char *buf;
int verbose;
{
	int ret = 1;

	/* TODO:	Add in dungeon name */
	/* done by Amy */
	if (Is_knox(&u.uz))
		Sprintf(buf, "%s ", dungeons[u.uz.dnum].dname);
	else if (In_quest(&u.uz))
		Sprintf(buf, flags.showlongstats ? "Quest %d " : "Ques %d ", dunlev(&u.uz)); /* used to be called home --Amy */
	else if (Is_astralevel(&u.uz)) /* why the heck is there a "l" missing in "astra_l_level"? */
		Sprintf(buf, flags.showlongstats ? "Astral Plane " : "Astral ");
	else if (Is_earthlevel(&u.uz))
		Sprintf(buf, flags.showlongstats ? "Earth Plane " : "Earth ");
	else if (Is_firelevel(&u.uz))
		Sprintf(buf, flags.showlongstats ? "Fire Plane " : "Fire ");
	else if (Is_waterlevel(&u.uz))
		Sprintf(buf, flags.showlongstats ? "Water Plane " : "Water ");
	else if (Is_airlevel(&u.uz))
		Sprintf(buf, flags.showlongstats ? "Air Plane " : "Air ");
	else if (!strcmp(dungeons[u.uz.dnum].dname, "One-eyed Sam's Market"))
		Sprintf(buf, flags.showlongstats ? "Blackmarket:%d " : "Blk:%d ", depth(&u.uz));
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Sokoban"))
		Sprintf(buf, flags.showlongstats ? "Sokoban:%d " : "Sok:%d ", depth(&u.uz));
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Gehennom"))
		Sprintf(buf, flags.showlongstats ? "Gehennom:%d " : "Geh:%d ", depth(&u.uz));
	else if (!strcmp(dungeons[u.uz.dnum].dname, "The Gnomish Mines"))
		Sprintf(buf, flags.showlongstats ? "Mines:%d " : "Min:%d ", depth(&u.uz));
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Town"))
		Sprintf(buf, flags.showlongstats ? "Town:%d " : "Tow:%d ", depth(&u.uz));
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Grund's Stronghold"))
		Sprintf(buf, flags.showlongstats ? "Stronghold:%d " : "Str:%d ", depth(&u.uz));
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Lawful Quest"))
		Sprintf(buf, flags.showlongstats ? "Nightmare:%d " : "Nig:%d ", depth(&u.uz));
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Neutral Quest"))
		Sprintf(buf, flags.showlongstats ? "Beholder:%d " : "Beh:%d ", depth(&u.uz));
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Chaotic Quest"))
		Sprintf(buf, flags.showlongstats ? "Vecna:%d " : "Vec:%d ", depth(&u.uz));
	else if (!strcmp(dungeons[u.uz.dnum].dname, "The Temple of Moloch"))
		Sprintf(buf, flags.showlongstats ? "Temple:%d " : "Tem:%d ", depth(&u.uz));
	else if (!strcmp(dungeons[u.uz.dnum].dname, "The Giant Caverns"))
		Sprintf(buf, flags.showlongstats ? "Giants:%d " : "Gia:%d ", depth(&u.uz));
	else if (!strcmp(dungeons[u.uz.dnum].dname, "The Sunless Sea"))
		Sprintf(buf, "Sea:%d ", depth(&u.uz));
	else if (!strcmp(dungeons[u.uz.dnum].dname, "The Spider Caves"))
		Sprintf(buf, flags.showlongstats ? "Spider:%d " : "Spi:%d ", depth(&u.uz));
	else if (!strcmp(dungeons[u.uz.dnum].dname, "The Lost Tomb"))
		Sprintf(buf, flags.showlongstats ? "Lost Tomb:%d " : "Los:%d ", depth(&u.uz));
	else if (!strcmp(dungeons[u.uz.dnum].dname, "The Wyrm Caves"))
		Sprintf(buf, flags.showlongstats ? "Wyrm Caves:%d " : "Wyr:%d ", depth(&u.uz));
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Fort Ludios"))
		Sprintf(buf, flags.showlongstats ? "Ludios:%d " : "Lud:%d ", depth(&u.uz));
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Vlad's Tower"))
		Sprintf(buf, flags.showlongstats ? "Vlad's Tower:%d " : "Vla:%d ", depth(&u.uz));
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Frankenstein's Lab"))
		Sprintf(buf, "Lab:%d ", depth(&u.uz));
	else if (!strcmp(dungeons[u.uz.dnum].dname, "Sheol"))
		Sprintf(buf, flags.showlongstats ? "Sheol:%d " : "She:%d ", depth(&u.uz));
	else {
		if (verbose)
			Sprintf(buf, "%s, level %d ",
				dungeons[u.uz.dnum].dname, depth(&u.uz));
		else
		Sprintf(buf, "Dlvl:%d ", depth(&u.uz));
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

/* armor that sufficiently covers the body might be able to block magic */
int
magic_negationX(mon)
struct monst *mon;
{
	struct obj *armor;
	int armpro = 0;

	armor = (mon == &youmonst) ? uarm : which_armor(mon, W_ARM);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
	armor = (mon == &youmonst) ? uarmc : which_armor(mon, W_ARMC);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
	armor = (mon == &youmonst) ? uarmh : which_armor(mon, W_ARMH);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;

	/* armor types for shirt, gloves, shoes, and shield don't currently
	   provide any magic cancellation but we might as well be complete */
#ifdef TOURIST
	armor = (mon == &youmonst) ? uarmu : which_armor(mon, W_ARMU);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
#endif
	armor = (mon == &youmonst) ? uarmg : which_armor(mon, W_ARMG);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
	armor = (mon == &youmonst) ? uarmf : which_armor(mon, W_ARMF);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
	armor = (mon == &youmonst) ? uarms : which_armor(mon, W_ARMS);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;

#ifdef STEED
	/* this one is really a stretch... */
	armor = (mon == &youmonst) ? 0 : which_armor(mon, W_SADDLE);
	if (armor && armpro < objects[armor->otyp].a_can)
	    armpro = objects[armor->otyp].a_can;
#endif
	if (MCReduction && mon == &youmonst) armpro -= (1 + (MCReduction / 5000));
	if (armpro < 0) armpro = 0;

	return armpro;
}


#ifdef DUMP_LOG
void
#else
STATIC_OVL void
#endif
bot2str(char *newbot2)
{
	register char *nb;
	int hp, hpmax;
	int cap = near_capacity();
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	struct color_option color_option;
	int save_botlx = flags.botlx;
#endif
#ifdef ALLEG_FX
	int w;
#endif

	hp = Upolyd ? u.mh : u.uhp;
	hpmax = Upolyd ? u.mhmax : u.uhpmax;

	/*if(hp < 0) hp = 0;*/ /* show by how much you have been overkilled --Amy */
	if (bot2_abbrev < 4)
		(void) describe_level(newbot2, FALSE);
	else
		newbot2[0] = '\0';
	if (bot2_abbrev < 1)
		Sprintf(nb = eos(newbot2), "%c%d ",
		  oc_syms[COIN_CLASS],
#ifndef GOLDOBJ
		u.ugold
#else
		money_cnt(invent)
#endif
		  );
	else
		nb = newbot2;

#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	Strcat(nb = eos(newbot2), "HP");
	curs(WIN_STATUS, 1, 1);
	putstr(WIN_STATUS, 0, newbot2);
	flags.botlx = 0;

	Sprintf(nb = eos(nb), "%d(%d)", hp, hpmax);
	apply_color_option(percentage_color_of(hp, hpmax, hp_colors), newbot2, 2);
#else
	Sprintf(nb = eos(nb), "HP%d(%d)", hp, hpmax);
#endif
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	Strcat(nb = eos(nb), " Pw");
	curs(WIN_STATUS, 1, 1);
	putstr(WIN_STATUS, 0, newbot2);

	Sprintf(nb = eos(nb), "%d(%d)", u.uen, u.uenmax);
	apply_color_option(percentage_color_of(u.uen, u.uenmax, pw_colors), newbot2, 2);
#else
	Sprintf(nb = eos(nb), " Pw%d(%d)", u.uen, u.uenmax);
#endif
	Sprintf(nb = eos(nb), " AC%d", u.uac);

	if (flags.showmc)
		Sprintf(nb = eos(nb), " MC%d", magic_negationX(&youmonst));

	if (flags.showmovement)
		Sprintf(nb = eos(nb), " Mov%d", youmonst.data->mmove);

	if (Upolyd)
		Sprintf(nb = eos(nb), " HD%d", ((u.ulycn == u.umonnum) ? 
						u.ulevel : mons[u.umonnum].mlevel));
	/*else*/ if(flags.showexp && bot2_abbrev < 3) /* show this when polymorphed, too --Amy */
		Sprintf(nb = eos(nb), " Xp%u/%-1ld", u.ulevel,u.uexp);
	else
		Sprintf(nb = eos(nb), " Exp%u", u.ulevel);

#ifdef SHOW_WEIGHT
	if (flags.showweight && bot2_abbrev < 3)
		Sprintf(nb = eos(nb), " Wt%ld/%ld", (long)(inv_weight()+weight_cap()),
				(long)weight_cap());
#endif

	if(flags.time && bot2_abbrev < 3)
	        Sprintf(nb = eos(nb), " T%ld", moves);

#ifdef REALTIME_ON_BOTL
  if(iflags.showrealtime) {
    time_t currenttime = get_realtime();
    Sprintf(nb = eos(nb), " %d:%2.2d", currenttime / 3600, 
                                       (currenttime % 3600) / 60);
  }
#endif

#ifdef ALLEG_FX
        if(iflags.usealleg && botl_warn && !Hallucination)
        {
            Sprintf(nb = eos(nb), " ");
            for(w = 0; w < botl_warn; w++)
                Sprintf(nb = eos(nb), "!");
        }
#endif
	        
/*        if (bot2_abbrev >= 2) {
		if (hu_abbrev_stat[u.uhs][0]!='\0') {
			Sprintf(nb = eos(nb), " ");
			Strcat(newbot2, hu_abbrev_stat[u.uhs]);
		}
	}
	else if(strcmp(hu_stat[u.uhs], "        ")) {
		Sprintf(nb = eos(nb), " ");
		Strcat(newbot2, hu_stat[u.uhs]);
	}
*/

	if (!Thirst && !u.uprops[THIRST].extrinsic && !have_thirststone() && u.urealedibility && u.uhunger >= 4500) 
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	     	add_colored_text(flags.showlongstats ? "Oversatiated" : "Ovs", newbot2);
#else
		Strcat(nb = eos(nb), flags.showlongstats ? " Oversatiated" : " Ovs");
#endif

	else if(!Thirst && !u.uprops[THIRST].extrinsic && !have_thirststone() && strcmp(hu_stat[u.uhs], "        "))
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	     	add_colored_text(flags.showlongstats ? hu_stat[u.uhs] : hu_abbrev_stat[u.uhs], newbot2);
#else
		Sprintf(nb = eos(nb), " %s", flags.showlongstats ? hu_stat[u.uhs] : hu_abbrev_stat[u.uhs]);
#endif

/* WAC further Up
	if (flags.showscore)
                Sprintf(nb,"%c%d Score%ld", oc_syms[COIN_CLASS],
                   u.ugold, botl_score());
*/
	/* KMH -- changed to Lev */
	if (Levitation)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	     	add_colored_text("Lev", newbot2);
#else
		Strcat(nb = eos(nb), " Lev");
#endif
	if (IsGlib)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	     	add_colored_text(flags.showlongstats ? "Glib" : "Glb", newbot2);
#else
		Strcat(nb = eos(nb), flags.showlongstats ? " Glib" : " Glb");
#endif
	if (Wounded_legs)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	     	add_colored_text(flags.showlongstats ? "Legs" : "Leg", newbot2);
#else
		Strcat(nb = eos(nb), flags.showlongstats ? " Legs" : " Leg");
#endif
	if (Strangled)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	     	add_colored_text(flags.showlongstats ? "Choke" : "Chk", newbot2);
#else
		Strcat(nb = eos(nb), flags.showlongstats ? " Choke" : " Chk");
#endif
	if (Vomiting)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	     	add_colored_text(flags.showlongstats ? "Vomit" : "Vmt", newbot2);
#else
		Strcat(nb = eos(nb), flags.showlongstats ? " Vomit" : " Vmt");
#endif
	if(Confusion && !HeavyConfusion)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	     	add_colored_text(flags.showlongstats ? "Conf" : "Cnf", newbot2);
#else
		Strcat(nb = eos(nb), flags.showlongstats ? " Conf" : " Cnf");
#endif
	if(Confusion && HeavyConfusion)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	     	add_colored_text(flags.showlongstats ? "XConf" : "XCnf", newbot2);
#else
		Strcat(nb = eos(nb), flags.showlongstats ? " XConf" : " XCnf");
#endif
	if(Sick) {
		if (u.usick_type & SICK_VOMITABLE)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
			add_colored_text(flags.showlongstats ? "FoodPois" : "FPo", newbot2);
#else
			Strcat(nb = eos(nb), flags.showlongstats ? " FoodPois" : " FPo");
#endif
		if (u.usick_type & SICK_NONVOMITABLE)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
			add_colored_text("Ill", newbot2);
#else
			Strcat(nb = eos(nb), " Ill");
#endif
	}

	if(Blind && !HeavyBlind)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	     	add_colored_text(flags.showlongstats ? "Blind" : "Bli", newbot2);
#else
		Strcat(nb = eos(nb), flags.showlongstats ? " Blind" : " Bli");
#endif
	if(Blind && HeavyBlind)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	     	add_colored_text(flags.showlongstats ? "XBlind" : "XBli", newbot2);
#else
		Strcat(nb = eos(nb), flags.showlongstats ? " XBlind" : " XBli");
#endif
	if(sengr_at("Elbereth", u.ux, u.uy))
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	     	add_colored_text(flags.showlongstats ? "Elbereth" : "Elb", newbot2);
#else
		Strcat(nb = eos(nb), flags.showlongstats ? " Elbereth" : " Elb");
#endif
	/* Yes I know, this should have a "is the player blind?" check. But I'm lenient. --Amy */

	if(Feared && !HeavyFeared)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	     	add_colored_text(Race_if(PM_TUMBLRER) ? (flags.showlongstats ? "Triggered" : "Trg") : (flags.showlongstats ? "Fear" : "Fea"), newbot2);
#else
		Strcat(nb = eos(nb), Race_if(PM_TUMBLRER) ? (flags.showlongstats ? " Triggered" : " Trg") : (flags.showlongstats ? " Fear" : " Fea") );
#endif
	if(Feared && HeavyFeared)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	     	add_colored_text(Race_if(PM_TUMBLRER) ? (flags.showlongstats ? "XTriggered" : "XTrg") : (flags.showlongstats ? "XFear" : "XFea"), newbot2);
#else
		Strcat(nb = eos(nb), Race_if(PM_TUMBLRER) ? (flags.showlongstats ? " XTriggered" : " XTrg") : (flags.showlongstats ? " XFear" : " XFea"));
#endif
	if(Numbed && !HeavyNumbed)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	     	add_colored_text(flags.showlongstats ? "Numb" : "Nmb", newbot2);
#else
		Strcat(nb = eos(nb), flags.showlongstats ? " Numb" : " Nmb");
#endif
	if(Numbed && HeavyNumbed)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	     	add_colored_text(flags.showlongstats ? "XNumb" : "XNmb", newbot2);
#else
		Strcat(nb = eos(nb), flags.showlongstats ? " XNumb" : " XNmb");
#endif
	if(Frozen && !HeavyFrozen)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	     	add_colored_text(flags.showlongstats ? "Freeze" : "Frz", newbot2);
#else
		Strcat(nb = eos(nb), flags.showlongstats ? " Freeze" : " Frz");
#endif
	if(Frozen && HeavyFrozen)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	     	add_colored_text(flags.showlongstats ? "XFreeze" : "XFrz", newbot2);
#else
		Strcat(nb = eos(nb), flags.showlongstats ? " XFreeze" : " XFrz");
#endif
	if(Burned && !HeavyBurned)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	     	add_colored_text(flags.showlongstats ? "Burn" : "Brn", newbot2);
#else
		Strcat(nb = eos(nb), flags.showlongstats ? " Burn" : " Brn");
#endif
	if(Burned && HeavyBurned)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	     	add_colored_text(flags.showlongstats ? "XBurn" : "XBrn", newbot2);
#else
		Strcat(nb = eos(nb), flags.showlongstats ? " XBurn" : " XBrn");
#endif
	if(Stunned && !HeavyStunned)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	     	add_colored_text(flags.showlongstats ? "Stun" : "Stn", newbot2);
#else
		Strcat(nb = eos(nb), flags.showlongstats ? " Stun" : " Stn");
#endif
	if(Stunned && HeavyStunned)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	     	add_colored_text(flags.showlongstats ? "XStun" : "XStn", newbot2);
#else
		Strcat(nb = eos(nb), flags.showlongstats ? " XStun" : " XStn");
#endif
	if(Hallucination && !HeavyHallu)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	     	add_colored_text(flags.showlongstats ? "Hallu" : "Hal", newbot2);
#else
		Strcat(nb = eos(nb), flags.showlongstats ? " Hallu" : " Hal");
#endif
	if(Hallucination && HeavyHallu)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	     	add_colored_text(flags.showlongstats ? "XHallu" : "XHal", newbot2);
#else
		Strcat(nb = eos(nb), flags.showlongstats ? " XHallu" : " XHal");
#endif
	if(Slimed)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	     	add_colored_text(flags.showlongstats ? "Slime" : "Sli", newbot2);
#else
		Strcat(nb = eos(nb), flags.showlongstats ? " Slime" : " Sli");
#endif
	if(Stoned)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	     	add_colored_text(flags.showlongstats ? "Stone" : "Sto", newbot2);
#else
		Strcat(nb = eos(nb), flags.showlongstats ? " Stone" : " Sto");
#endif
	if(u.ustuck && !u.uswallow && !sticks(youmonst.data))
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
	     	add_colored_text(flags.showlongstats ? "Held" : "Hld", newbot2);
#else
		Strcat(nb = eos(nb), flags.showlongstats ? " Held" : " Hld");
#endif
	if(cap > UNENCUMBERED)
#if defined(STATUS_COLORS) && defined(TEXTCOLOR)
		add_colored_text(flags.showlongstats ? enc_stat[cap] : enc_abbrev_stat[cap], newbot2);
#else
		Sprintf(nb = eos(nb), " %s", enc_stat[cap]);
#endif
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
    /*for(bot2_abbrev = 1; bot2_abbrev <= 4; bot2_abbrev++) {
	bot2str(cbuf);
	if (strlen(cbuf) <= len)
	    break;
    }*/
    bot2_abbrev = 5;
    /*if (bot2_abbrev > 4)
	cbuf[len] = '\0';*/	/* If all else fails, truncate the line */
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
    if (flags.showscore)
	*rv++ = reconfig ? "score" :
		(Sprintf(score, "%ld", botl_score()), score);
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
    if (flags.showexp)
	*rv++ = reconfig ? "experience" : (Sprintf(expr, "%ld", u.uexp), expr);
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
    if (!Thirst && !u.uprops[THIRST].extrinsic && !have_thirststone()) *rv++ = reconfig ? "hunger" : strcmp(hu_stat[u.uhs], "        ") ?
	    hu_stat[u.uhs] : "";
    *rv++ = reconfig ? "encumberance" : enc_stat[near_capacity()];
    *rv++ = reconfig ? "flags" : (Sprintf(flgs, "%lX",
        (Levitation ? RAW_STAT_LEVITATION : 0) |
	(Confusion ? RAW_STAT_CONFUSION : 0) |
	(Sick && (u.usick_type & SICK_VOMITABLE) ? RAW_STAT_FOODPOIS : 0) |
	(Sick && (u.usick_type & SICK_NONVOMITABLE) ? RAW_STAT_ILL : 0) |
	(Blind ? RAW_STAT_BLIND : 0) |
	(Stunned ? RAW_STAT_STUNNED : 0) |
	(Numbed ? RAW_STAT_NUMBED : 0) |
	(Feared ? RAW_STAT_FEARED : 0) |
	(Frozen ? RAW_STAT_FROZEN : 0) |
	(Burned ? RAW_STAT_BURNED : 0) |
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
	if (!DisplayLoss && !u.uprops[DISPLAY_LOST].extrinsic && !have_displaystone()) bot1();
	if (!DisplayLoss && !u.uprops[DISPLAY_LOST].extrinsic && !have_displaystone()) bot2();
	}
	flags.botl = flags.botlx = 0;
}

#endif /* OVL0 */

/*botl.c*/
