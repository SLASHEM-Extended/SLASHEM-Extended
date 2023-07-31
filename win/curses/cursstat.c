/* vim:set cin ft=c sw=4 sts=4 ts=8 et ai cino=Ls\:0t0(0 : -*- mode:c;fill-column:80;tab-width:8;c-basic-offset:4;indent-tabs-mode:nil;c-file-style:"k&r" -*-*/

#include "curses.h"
#include "hack.h"
#include "wincurs.h"
#include "cursstat.h"

/* Status window functions for curses interface */

/* Private declarations */

/* Used to track previous value of things, to highlight changes. */
typedef struct nhs {
    long value;
    int highlight_turns;
    int highlight_color;
} nhstat;

static attr_t get_trouble_color(const char *);
static void draw_trouble_str(const char *);
static void print_statdiff(const char *append, nhstat *, int, int);
static void get_playerrank(char *);
static int hpen_color(boolean, int, int);
static void draw_bar(boolean, int, int, const char *);
static void draw_horizontal(int, int, int, int);
static void draw_horizontal_new(int, int, int, int);
static void draw_vertical(int, int, int, int);
static void curses_add_statuses(WINDOW *, boolean, boolean, int *, int *);
static void curses_add_status(WINDOW *, boolean, boolean, int *, int *,
                              const char *, int);
static int decrement_highlight(nhstat *, boolean);
int magic_negationX(struct monst *);

#ifdef STATUS_COLORS
static attr_t hpen_color_attr(boolean, int, int);
static attr_t flickercolorattr(int);
extern struct color_option text_color_of(const char *text,
                                         const struct text_color_option *color_options);
struct color_option percentage_color_of(int value, int max,
                                        const struct percent_color_option *color_options);

extern const struct text_color_option *text_colors;
extern const struct percent_color_option *hp_colors;
extern const struct percent_color_option *pw_colors;
#endif

/* Whether or not we have printed status window content at least once.
   Used to ensure that prev* doesn't end up highlighted on game start. */
static boolean first = TRUE;
static nhstat prevdepth;
static nhstat prevstr;
static nhstat prevint;
static nhstat prevwis;
static nhstat prevdex;
static nhstat prevcon;
static nhstat prevcha;
static nhstat prevau;
static nhstat prevlevel;
static nhstat prevac;
static nhstat prevexp;
static nhstat prevtime;

static nhstat prevscore;

extern const char *hu_stat[];   /* from eat.c */
extern const char *enc_stat[];  /* from botl.c */

/* If the statuscolors patch isn't enabled, have some default colors for status problems
   anyway */

struct statcolor {
    const char *txt; /* For status problems */
    int color; /* Default color assuming STATUS_COLORS isn't enabled */
};

static const struct statcolor default_colors[] = {
    {"Oversatiated", CLR_YELLOW},
    {"Satiated", CLR_YELLOW},
    {"Hungry", CLR_YELLOW},
    {"Weak", CLR_ORANGE},
    {"Fainted", CLR_BRIGHT_MAGENTA},
    {"Fainting", CLR_BRIGHT_MAGENTA},
    {"Starved", CLR_BRIGHT_MAGENTA},
    {"Burdened", CLR_RED},
    {"Stressed", CLR_RED},
    {"Strained", CLR_ORANGE},
    {"Overtaxed", CLR_ORANGE},
    {"Overloaded", CLR_BRIGHT_MAGENTA},
    {"Burn", CLR_ORANGE},
    {"XBurn", CLR_ORANGE},
    {"Glib", CLR_ORANGE},
    {"Legs", CLR_ORANGE},
    {"Choke", CLR_ORANGE},
    {"Lev", CLR_CYAN},
    {"Vomit", CLR_CYAN},
    {"Held", CLR_BRIGHT_CYAN},
    {"Elbereth", CLR_BRIGHT_GREEN},
    {"Conf", CLR_BRIGHT_BLUE},
    {"XConf", CLR_BRIGHT_BLUE},
    {"Numb", CLR_BRIGHT_BLUE},
    {"XNumb", CLR_BRIGHT_BLUE},
    {"Paralyzed", CLR_ORANGE},
    {"Vibration", CLR_ORANGE},
    {"Freeze", CLR_BRIGHT_BLUE},
    {"XFreeze", CLR_BRIGHT_BLUE},
    {"Blind", CLR_BRIGHT_BLUE},
    {"XBlind", CLR_BRIGHT_BLUE},
    {"Stun", CLR_BRIGHT_BLUE},
    {"XStun", CLR_BRIGHT_BLUE},
    {"Hallu", CLR_BRIGHT_BLUE},
    {"XHallu", CLR_BRIGHT_BLUE},
    {"Ill", CLR_BRIGHT_MAGENTA},
    {"FoodPois", CLR_BRIGHT_MAGENTA},
    {"Slime", CLR_BRIGHT_MAGENTA},
    {"Stoned", CLR_BRIGHT_MAGENTA},
    {"Fear", CLR_BRIGHT_MAGENTA},
    {"XFear", CLR_BRIGHT_MAGENTA},
    {"Triggered", CLR_BRIGHT_MAGENTA},
    {"XTriggered", CLR_BRIGHT_MAGENTA},
    {"Dim", CLR_BRIGHT_MAGENTA},
    {"XDim", CLR_BRIGHT_MAGENTA},
    {NULL, NO_COLOR},
};

static attr_t
get_trouble_color(const char *stat)
{
    attr_t res = curses_color_attr(CLR_GRAY, 0);
    const struct statcolor *clr;
    for (clr = default_colors; clr->txt; clr++) {
        if (stat && !strcmp(clr->txt, stat)) {
#ifdef STATUS_COLORS
            /* Check if we have a color enabled with statuscolors */
            if (!iflags.use_status_colors)
                return curses_color_attr(CLR_GRAY, 0); /* no color configured */

            struct color_option stat_color;

            stat_color = text_color_of(clr->txt, text_colors);
            if (stat_color.color == NO_COLOR && !stat_color.attr_bits)
                return curses_color_attr(CLR_GRAY, 0);

            if (stat_color.color != NO_COLOR)
                res = curses_color_attr(stat_color.color, 0);

            res = curses_color_attr(stat_color.color, 0);
            int count;
            for (count = 0; (1 << count) <= stat_color.attr_bits; count++) {
                if (count != ATR_NONE &&
                    (stat_color.attr_bits & (1 << count)))
                    res |= curses_convert_attr(count);
            }

            return res;
#else
            return curses_color_attr(clr->color, 0);
#endif
        }
    }

    return res;
}

/* TODO: This is in the wrong place. */
void
get_playerrank(char *rank)
{
    char buf[BUFSZ];
    if (Upolyd) {
        int k = 0;

        strcpy(buf, mons[u.umonnum].mname);
        while(buf[k] != 0) {
            if ((k == 0 || (k > 0 && buf[k-1] == ' ')) &&
                'a' <= buf[k] && buf[k] <= 'z')
                buf[k] += 'A' - 'a';
            k++;
        }
        strcpy(rank, buf);
    } else
        strcpy(rank, rank_of(u.ulevel, Role_switch, flags.female));
}

/* Handles numerical stat changes of various kinds.
   type is generally STAT_OTHER (generic "do nothing special"),
   but is used if the stat needs to be handled in a special way. */
static void
print_statdiff(const char *append, nhstat *stat, int new, int type)
{
    char buf[BUFSZ];
    WINDOW *win = curses_get_nhwin(STATUS_WIN);

    int color = CLR_GRAY;

    /* Turncount isn't highlighted, or it would be highlighted constantly. */
    if (type != STAT_TIME && new != stat->value) {
        /* Less AC is better */
        if ((type == STAT_AC && new < stat->value) ||
            (type != STAT_AC && new > stat->value)) {
            color = STAT_UP_COLOR;
            if (type == STAT_GOLD)
                color = HI_GOLD;
        } else
            color = STAT_DOWN_COLOR;

        stat->value = new;
        stat->highlight_color = color;
        stat->highlight_turns = 5;
    } else if (stat->highlight_turns)
        color = stat->highlight_color;

    attr_t attr = curses_color_attr(color, 0);
    wattron(win, attr);
    wprintw(win, "%s", append);
    if (type == STAT_STR && new > 18) {
        if (new > 118)
            wprintw(win, "%d", new - 100);
        else if (new == 118)
            wprintw(win, "18/**");
        else
            wprintw(win, "18/%02d", new - 18);
    } else
        wprintw(win, "%d", new);

    wattroff(win, attr);
}

static void
draw_trouble_str(const char *str)
{
    WINDOW *win = curses_get_nhwin(STATUS_WIN);

    attr_t attr = get_trouble_color(str);
    wattron(win, attr);
    wprintw(win, "%s", str);
    wattroff(win, attr);
}

/* Returns a ncurses attribute for foreground and background.
   This should probably be in cursinit.c or something. */
attr_t
curses_color_attr(int nh_color, int bg_color)
{
    int color = nh_color + 1;
    attr_t cattr = A_NORMAL;

    if (!nh_color) {
#ifdef USE_DARKGRAY
        if (iflags.wc2_darkgray) {
            if (!can_change_color() || COLORS <= 16)
                cattr |= A_BOLD;
        } else
#endif
            color = COLOR_BLUE;
    }

    if (COLORS < 16 && color > 8) {
        color -= 8;
        cattr = A_BOLD;
    }

    /* Can we do background colors? We can if we have more than
       16*7 colors (more than 8*7 for terminals with bold) */
    if (COLOR_PAIRS > (COLORS >= 16 ? 16 : 8) * 7) {
        /* NH3 has a rather overcomplicated way of defining
           its colors past the first 16:
           Pair    Foreground  Background
           17      Black       Red
           18      Black       Blue
           19      Red         Red
           20      Red         Blue
           21      Green       Red
           ...
           (Foreground order: Black, Red, Green, Yellow, Blue,
           Magenta, Cyan, Gray/White)

           To work around these oddities, we define backgrounds
           by the following pairs:

           16 COLORS
           49-64: Green
           65-80: Yellow
           81-96: Magenta
           97-112: Cyan
           113-128: Gray/White

           8 COLORS
           9-16: Green
           33-40: Yellow
           41-48: Magenta
           49-56: Cyan
           57-64: Gray/White */

        if (bg_color == nh_color)
            color = 1; /* Make foreground black if fg==bg */

        if (bg_color == CLR_RED || bg_color == CLR_BLUE) {
            /* already defined before extension */
            color *= 2;
            color += 16;
            if (bg_color == CLR_RED)
                color--;
        } else {
            boolean hicolor = FALSE;
            if (COLORS >= 16)
                hicolor = TRUE;

            switch (bg_color) {
            case CLR_GREEN:
                color = (hicolor ? 48 : 8) + color;
                break;
            case CLR_BROWN:
                color = (hicolor ? 64 : 32) + color;
                break;
            case CLR_MAGENTA:
                color = (hicolor ? 80 : 40) + color;
                break;
            case CLR_CYAN:
                color = (hicolor ? 96 : 48) + color;
                break;
            case CLR_GRAY:
                color = (hicolor ? 112 : 56) + color;
                break;
            default:
                break;
            }
        }
    }
    cattr |= COLOR_PAIR(color);

    return cattr;
}

/* Returns a complete curses attribute. Used to possibly bold/underline/etc HP/Pw. */
#ifdef STATUS_COLORS
static attr_t
hpen_color_attr(boolean is_hp, int cur, int max)
{
    struct color_option stat_color;
    int count;
    attr_t attr = 0;
    if (!iflags.use_status_colors)
        return curses_color_attr(CLR_GRAY, 0);

    stat_color = percentage_color_of(cur, max, is_hp ? hp_colors : pw_colors);

    if (stat_color.color != NO_COLOR)
        attr |= curses_color_attr(stat_color.color, 0);

    for (count = 0; (1 << count) <= stat_color.attr_bits; count++) {
        if (count != ATR_NONE && (stat_color.attr_bits & (1 << count)))
            attr |= curses_convert_attr(count);
    }

    return attr;
}

static attr_t
flickercolorattr(int color)
{
	attr_t attr = 0;
      attr |= curses_color_attr(color, 0);
      return attr;

}
#endif

/* Return color for the HP bar.
   With status colors ON, this respect its configuration (defaulting to gray), but
   only obeys the color (no weird attributes for the HP bar).
   With status colors OFF, this returns reasonable defaults which are also used
   for the HP/Pw text itself. */
static int
hpen_color(boolean is_hp, int cur, int max)
{
#ifdef STATUS_COLORS
    if (iflags.use_status_colors) {
        struct color_option stat_color;
        stat_color = percentage_color_of(cur, max, is_hp ? hp_colors : pw_colors);

        if (stat_color.color == NO_COLOR)
            return CLR_GRAY;
        else
            return stat_color.color;
    } else
        return CLR_GRAY;
#endif

    int color = CLR_GRAY;
    if (cur == max)
        color = CLR_GRAY;
    else if (cur * 3 > max * 2) /* >2/3 */
        color = is_hp ? CLR_GREEN : CLR_CYAN;
    else if (cur * 3 > max) /* >1/3 */
        color = is_hp ? CLR_YELLOW : CLR_BLUE;
    else if (cur * 7 > max) /* >1/7 */
        color = is_hp ? CLR_RED : CLR_MAGENTA;
    else
        color = is_hp ? CLR_ORANGE : CLR_BRIGHT_MAGENTA;

    return color;
}

/* Draws a bar
   is_hp: TRUE if we're drawing HP, Pw otherwise (determines colors)
   cur/max: Current/max HP/Pw
   title: Not NULL if we are drawing as part of an existing title.
   Otherwise, the format is as follows: [   11 / 11   ] */
static void
draw_bar(boolean is_hp, int cur, int max, const char *title)
{
    WINDOW *win = curses_get_nhwin(STATUS_WIN);

#ifdef STATUS_COLORS
    if (!flags.hitpointbar) {
        wprintw(win, "%s", !title ? "---" : title);
        return;
    }
#endif

    char buf[BUFSZ];
    if (title)
        strcpy(buf, title);
    else {
        int len = 5;
        sprintf(buf, "%*d / %-*d", len, cur, len, max);
    }

    /* Colors */
    attr_t fillattr, attr;
    int color = hpen_color(is_hp, cur, max);
    int invcolor = color & 7;

    fillattr = curses_color_attr(color, invcolor);
    attr = curses_color_attr(color, 0);

    /* Figure out how much of the bar to fill */
    int fill = 0;
    int len = strlen(buf);
    if (cur > 0 && max > 0)
        fill = len * cur / max;
    if (fill > len)
        fill = len;

    waddch(win, '[');
    wattron(win, fillattr);
    wprintw(win, "%.*s", fill, buf);
    wattroff(win, fillattr);
    wattron(win, attr);
    wprintw(win, "%.*s", len - fill, &buf[fill]);
    wattroff(win, attr);
    waddch(win, ']');
}

/* Update the status win - this is called when NetHack would normally
   write to the status window, so we know somwthing has changed.  We
   override the write and update what needs to be updated ourselves. */
void
curses_update_stats(void)
{
    WINDOW *win = curses_get_nhwin(STATUS_WIN);
    int orient = curses_get_window_orientation(STATUS_WIN);

    boolean horiz = FALSE;
    if ((orient != ALIGN_RIGHT) && (orient != ALIGN_LEFT))
        horiz = TRUE;

    boolean border = curses_window_has_border(STATUS_WIN);

    werase(win);

    /* Figure out if we have proper window dimensions for horizontal statusbar. */
    if (horiz) {
        /* correct y */
        int cy = 3;
        /*if (iflags.classic_status)
            cy = 2;*/

        /* actual y (and x) */
        int ax = 0;
        int ay = 0;
        getmaxyx(win, ay, ax);
        if (border)
            ay -= 2;

        if (cy != ay) {
            curses_create_main_windows();
            curses_last_messages();
            doredraw();

            /* Reset XP highlight (since classic_status and new show different numbers) */
            prevexp.highlight_turns = 0;
            curses_update_stats();
            return;
        }
    }

    /* Starting x/y. Passed to draw_horizontal/draw_vertical to keep track of
       window positioning. */
    int x = 0;
    int y = 0;

    /* Don't start at border position if applicable */
    if (border) {
        x++;
        y++;
    }

    /* Get HP values. */
    int hp = u.uhp;
    int hpmax = u.uhpmax;
    if (Upolyd) {
        hp = u.mh;
        hpmax = u.mhmax;
    }

    if (orient != ALIGN_RIGHT && orient != ALIGN_LEFT)
        draw_horizontal(x, y, hp, hpmax);
    else
        draw_vertical(x, y, hp, hpmax);

    if (border)
        box(win, 0, 0);

    wrefresh(win);

    if (first) {
        first = FALSE;

        /* Zero highlight timers. This will call curses_update_status again if needed */
        curses_decrement_highlights(TRUE);
    }
}

static void
draw_horizontal(int x, int y, int hp, int hpmax)
{

	if (youmonst.data && DisplayDoesNotGo) return;

    if (!iflags.classic_status) {
        /* Draw new-style statusbar */
        draw_horizontal_new(x, y, hp, hpmax);
        return;
    }
    char buf[BUFSZ];
    char rank[BUFSZ];
    WINDOW *win = curses_get_nhwin(STATUS_WIN);

    /* Line 1 */
    wmove(win, y, x);

	if (youmonst.data && FlimmeringStrips) {
		attr_t flickertextA;
		int flickercolor = rn2(CLR_MAX);
		while (flickercolor == NO_COLOR) flickercolor = rn2(CLR_MAX);
		flickertextA = flickercolorattr(flickercolor);
		wprintw(win, generate_garbage_string());
		wattron(win, flickertextA);

		goto linetwo;
	}

    get_playerrank(rank);
    sprintf(buf, "%s the %s", playeraliasname, rank);

    /* Use the title as HP bar (similar to hitpointbar) */
    draw_bar(TRUE, hp, hpmax, buf);

    /* Attributes */
    print_statdiff(" St", &prevstr, ACURR(A_STR), STAT_STR);
    print_statdiff(" Dx", &prevdex, ACURR(A_DEX), STAT_OTHER);
    print_statdiff(" Co", &prevcon, ACURR(A_CON), STAT_OTHER);
    print_statdiff(" In", &prevint, ACURR(A_INT), STAT_OTHER);
    print_statdiff(" Wi", &prevwis, ACURR(A_WIS), STAT_OTHER);
    print_statdiff(" Ch", &prevcha, ACURR(A_CHA), STAT_OTHER);

	if (!TheInfoIsFucked) wprintw(win, " ");
	if (!TheInfoIsFucked) wprintw(win, urole.filecode);
	if (!TheInfoIsFucked) wprintw(win, urace.filecode);
	if (!TheInfoIsFucked) wprintw(win, flags.female ? "Fem" : "Mal");
	if (!TheInfoIsFucked) wprintw(win, (u.ualign.type == A_CHAOTIC) ? "Cha" : (u.ualign.type == A_NEUTRAL) ? "Neu" : "Law");

	if (flags.hybridization && !TheInfoIsFucked) {
		wprintw(win, "+");
		if (flags.hybridcurser) wprintw(win, "C");
		if (flags.hybridhaxor) wprintw(win, "H");
		if (flags.hybridangbander) wprintw(win, "A");
		if (flags.hybridhomicider) wprintw(win, "O");
		if (flags.hybridsuxxor) wprintw(win, "S");
		if (flags.hybridaquarian) wprintw(win, "Q");
		if (flags.hybridwarper) wprintw(win, "W");
		if (flags.hybridrandomizer) wprintw(win, "R");
		if (flags.hybridnullrace) wprintw(win, "N");
		if (flags.hybridmazewalker) wprintw(win, "M");
		if (flags.hybridsoviet) wprintw(win, "V");
		if (flags.hybridxrace) wprintw(win, "X");
		if (flags.hybridspecialist) wprintw(win, "P");
		if (flags.hybridminimalist) wprintw(win, "L");
		if (flags.hybridamerican) wprintw(win, "I");
		if (flags.hybridsokosolver) wprintw(win, "K");
		if (flags.hybridheretic) wprintw(win, "E");
		if (flags.hybridnastinator) wprintw(win, "Y");
		if (flags.hybridrougelike) wprintw(win, "U");
		if (flags.hybridsegfaulter) wprintw(win, "F");
		if (flags.hybridironman) wprintw(win, "J");
		if (flags.hybridamnesiac) wprintw(win, "Z");
		if (flags.hybridproblematic) wprintw(win, "B");
		if (flags.hybridwindinhabitant) wprintw(win, "D");
		if (flags.hybridaggravator) wprintw(win, "G");
		if (flags.hybridevilvariant) wprintw(win, "T");
		if (flags.hybridlevelscaler) wprintw(win, "s");
		if (flags.hybridhallucinator) wprintw(win, "h");
		if (flags.hybridbossrusher) wprintw(win, "b");
		if (flags.hybriddorian) wprintw(win, "d");
		if (flags.hybridtechless) wprintw(win, "t");
		if (flags.hybridblait) wprintw(win, "l");
	}

    if (flags.showscore)
        print_statdiff(" S:", &prevscore, botl_score(), STAT_OTHER);

linetwo:
    /* Line 2 */
    y++;
    wmove(win, y, x);

	if (youmonst.data && FlimmeringStrips) {
		attr_t flickertextB;
		int flickercolor = rn2(CLR_MAX);
		while (flickercolor == NO_COLOR) flickercolor = rn2(CLR_MAX);
		flickertextB = flickercolorattr(flickercolor);
		wprintw(win, generate_garbage_string());
		wattron(win, flickertextB);
		flickercolor = rn2(CLR_MAX);
		while (flickercolor == NO_COLOR) flickercolor = rn2(CLR_MAX);
		flickertextB = flickercolorattr(flickercolor);
		wprintw(win, "%d/%d", (hp < 0) ? 0 : hp, hpmax);
		wattron(win, flickertextB);
		flickercolor = rn2(CLR_MAX);
		while (flickercolor == NO_COLOR) flickercolor = rn2(CLR_MAX);
		flickertextB = flickercolorattr(flickercolor);
		wprintw(win, "%d/%d", u.uen, u.uenmax);
		wattron(win, flickertextB);
		return;
	}

    describe_level(buf, FALSE);

    wprintw(win, "%s", buf);

#ifndef GOLDOBJ
    print_statdiff(
#ifdef GMMODE
	flags.supergmmode ? "S" : flags.gmmode ? "G" : 
#endif
	"$", &prevau, u.ugold, STAT_GOLD);
#else
    print_statdiff(
#ifdef GMMODE
	flags.supergmmode ? "S" : flags.gmmode ? "G" :
#endif
	"$", &prevau, money_cnt(invent), STAT_GOLD);
#endif

    /* HP/Pw use special coloring rules */
    attr_t hpattr, pwattr;
#ifdef STATUS_COLORS
    hpattr = hpen_color_attr(TRUE, hp, hpmax);
    pwattr = hpen_color_attr(FALSE, u.uen, u.uenmax);
#else
    int hpcolor, pwcolor;
    hpcolor = hpen_color(TRUE, hp, hpmax);
    pwcolor = hpen_color(FALSE, u.uen, u.uenmax);
    hpattr = curses_color_attr(hpcolor, 0);
    pwattr = curses_color_attr(pwcolor, 0);
#endif
    wprintw(win, " HP:");
    wattron(win, hpattr);
    wprintw(win, "%d(%d)", (hp < 0) ? 0 : hp, hpmax);
    wattroff(win, hpattr);

    wprintw(win, " Pw:");
    wattron(win, pwattr);
    wprintw(win, "%d(%d)", u.uen, u.uenmax);
    wattroff(win, pwattr);

	if (flags.showmc)
		wprintw(win, " MC%d", magic_negationX(&youmonst));

	if (flags.showmovement && youmonst.data)
		wprintw(win, " Mov%d", youmonst.data->mmove);

	if (flags.showsanity && youmonst.data)
		wprintw(win, " Sn%d", u.usanity);

    print_statdiff(" AC:", &prevac, u.uac, STAT_AC);

    if (Upolyd) {
        print_statdiff(" HD:", &prevlevel, mons[u.umonnum].mlevel, STAT_OTHER);
    }
    /*else*/ if (flags.showexp) {
        print_statdiff(" Xp:", &prevlevel, u.ulevel, STAT_OTHER);
        /* use waddch, we don't want to highlight the '/' */
        waddch(win, '/');
        print_statdiff("", &prevexp, u.uexp, STAT_OTHER);
    }

    if (flags.showsymbiotehp && uinsymbiosis) {
	  wprintw(win, " SH:%d(%d)", u.usymbiote.mhp, u.usymbiote.mhpmax);
	  if (u.usymbiote.cursed) {
		wprintw(win, "%s", u.usymbiote.stckcurse ? "S" : "C");
		wprintw(win, "%s", (u.usymbiote.evilcurse || u.usymbiote.bbcurse || u.usymbiote.morgcurse) ? "XXX" : u.usymbiote.prmcurse ? "XX" : u.usymbiote.hvycurse ? "X" : "");
		if (u.shutdowntime) wprintw(win, "sd");
	  }
    }

#ifdef SHOW_WEIGHT
	if (flags.showweight && youmonst.data && !WeightDisplayIsArbitrary)
		wprintw(win, " Wt%ld/%ld", (long)(inv_weight()+weight_cap()), (long)weight_cap());
#endif

    if (flags.time)
        print_statdiff(" T:", &prevtime, moves, STAT_TIME);

    if (youmonst.data) curses_add_statuses(win, FALSE, FALSE, NULL, NULL);
}

static void
draw_horizontal_new(int x, int y, int hp, int hpmax)
{
    char buf[BUFSZ];
    char rank[BUFSZ];
    WINDOW *win = curses_get_nhwin(STATUS_WIN);

	if (youmonst.data && DisplayDoesNotGo) return;

    /* Line 1 */
    wmove(win, y, x);

	if (youmonst.data && FlimmeringStrips) {
		attr_t flickertextA;
		int flickercolor = rn2(CLR_MAX);
		while (flickercolor == NO_COLOR) flickercolor = rn2(CLR_MAX);
		flickertextA = flickercolorattr(flickercolor);
		wprintw(win, generate_garbage_string());
		wattron(win, flickertextA);

		goto linetwonew;
	}

    get_playerrank(rank);
    char race[BUFSZ];
    strcpy(race, urace.adj);
    race[0] = highc(race[0]);
    wprintw(win, "%s the %s %s%s%s", playeraliasname,
            (TheInfoIsFucked ? "" :
		 u.ualign.type == A_CHAOTIC ? "Chaotic" : u.ualign.type == A_NEUTRAL ? "Neutral" : "Lawful"),
            (Upolyd || TheInfoIsFucked) ? "" : race, Upolyd ? "" : " ",
            rank);

linetwonew:

    /* Line 2 */
    y++;
    wmove(win, y, x);

	if (youmonst.data && FlimmeringStrips) {
		attr_t flickertextB;
		int flickercolor = rn2(CLR_MAX);
		while (flickercolor == NO_COLOR) flickercolor = rn2(CLR_MAX);
		flickertextB = flickercolorattr(flickercolor);
		wprintw(win, generate_garbage_string());
		wattron(win, flickertextB);
		flickercolor = rn2(CLR_MAX);
		while (flickercolor == NO_COLOR) flickercolor = rn2(CLR_MAX);
		flickertextB = flickercolorattr(flickercolor);
		wprintw(win, "%d/%d", (hp < 0) ? 0 : hp, hpmax);
		wattron(win, flickertextB);
		flickercolor = rn2(CLR_MAX);
		while (flickercolor == NO_COLOR) flickercolor = rn2(CLR_MAX);
		flickertextB = flickercolorattr(flickercolor);
		wprintw(win, "%d/%d", u.uen, u.uenmax);
		wattron(win, flickertextB);
		return;
	}

    wprintw(win, "HP:");
    draw_bar(TRUE, hp, hpmax, NULL);
    print_statdiff(" AC:", &prevac, u.uac, STAT_AC);
    if (Upolyd)
        print_statdiff(" HD:", &prevlevel, mons[u.umonnum].mlevel, STAT_OTHER);
    /*else*/ if (flags.showexp) {
        /* Ensure that Xp have proper highlight on level change. */
        int levelchange = 0;
        if (prevlevel.value != u.ulevel) {
            if (prevlevel.value < u.ulevel)
                levelchange = 1;
            else
                levelchange = 2;
        }
        print_statdiff(" Xp:", &prevlevel, u.ulevel, STAT_OTHER);
        /* use waddch, we don't want to highlight the '/' */
        waddch(win, '(');

        /* Figure out amount of Xp needed to next level */
        int xp_left = 0;
        if (u.ulevel < 30)
            xp_left = (newuexp(u.ulevel) - u.uexp);

        if (levelchange) {
            prevexp.value = (xp_left + 1);
            if (levelchange == 2)
                prevexp.value = (xp_left - 1);
        }
        print_statdiff("", &prevexp, xp_left, STAT_AC);
        waddch(win, ')');
    }

    waddch(win, ' ');
    describe_level(buf, FALSE);

    wprintw(win, "%s", buf);


    /* Line 3 */
    y++;
    wmove(win, y, x);
    wprintw(win, "Pw:");
    draw_bar(FALSE, u.uen, u.uenmax, NULL);

#ifndef GOLDOBJ
    print_statdiff(" $", &prevau, u.ugold, STAT_GOLD);
#else
    print_statdiff(" $", &prevau, money_cnt(invent), STAT_GOLD);
#endif

    if (flags.showscore)
        print_statdiff(" S:", &prevscore, botl_score(), STAT_OTHER);

    if (flags.time)
        print_statdiff(" T:", &prevtime, moves, STAT_TIME);

    if (youmonst.data) curses_add_statuses(win, TRUE, FALSE, &x, &y);

    /* Right-aligned attributes */
    int stat_length = 6; /* " Dx:xx" */
    int str_length = 6;
    if (ACURR(A_STR) > 18 && ACURR(A_STR) < 119)
        str_length = 9;

    getmaxyx(win, y, x);

    /* We want to deal with top line of y. getmaxx would do what we want, but it only
       exist for compatibility reasons and might not exist at all in some versions. */
    y = 0;
    if (curses_window_has_border(STATUS_WIN)) {
        x--;
        y++;
    }

    x -= stat_length;
    int orig_x = x;
    wmove(win, y, x);
    print_statdiff(" Co", &prevcon, ACURR(A_CON), STAT_OTHER);
    x -= stat_length;
    wmove(win, y, x);
    print_statdiff(" Dx", &prevdex, ACURR(A_DEX), STAT_OTHER);
    x -= str_length;
    wmove(win, y, x);
    print_statdiff(" St", &prevstr, ACURR(A_STR), STAT_STR);

    x = orig_x;
    y++;
    wmove(win, y, x);
    print_statdiff(" Ch", &prevcha, ACURR(A_CHA), STAT_OTHER);
    x -= stat_length;
    wmove(win, y, x);
    print_statdiff(" Wi", &prevwis, ACURR(A_WIS), STAT_OTHER);
    x -= str_length;
    wmove(win, y, x);
    print_statdiff(" In", &prevint, ACURR(A_INT), STAT_OTHER);
}

/* Personally I never understood the point of a vertical status bar. But removing the
   option would be silly, so keep the functionality. */
static void
draw_vertical(int x, int y, int hp, int hpmax)
{
	if (youmonst.data && DisplayDoesNotGo) return;

    char buf[BUFSZ];
    char rank[BUFSZ];
    WINDOW *win = curses_get_nhwin(STATUS_WIN);

    /* Clear the window */
    werase(win);

    /* Print title and dungeon branch */
    wmove(win, y++, x);

	if (youmonst.data && FlimmeringStrips) {
		attr_t flickertextA;
		int flickercolor = rn2(CLR_MAX);
		while (flickercolor == NO_COLOR) flickercolor = rn2(CLR_MAX);
		flickertextA = flickercolorattr(flickercolor);
		wprintw(win, generate_garbage_string());
		wattron(win, flickertextA);

		goto linetwovert;
	}


    get_playerrank(rank);
    int ranklen = strlen(rank);
    int namelen = strlen(playeraliasname);
    int maxlen = 19;
#ifdef STATUS_COLORS
    if (!flags.hitpointbar)
        maxlen += 2; /* With no hitpointbar, we can fit more since there's no "[]" */
#endif

    if ((ranklen + namelen) > maxlen) {
        /* The result doesn't fit. Strip name if >10 characters, then strip title */
        if (namelen > 10) {
            while (namelen > 10 && (ranklen + namelen) > maxlen)
                namelen--;
        }

        while ((ranklen + namelen) > maxlen)
            ranklen--; /* Still doesn't fit, strip rank */
    }
    sprintf(buf, "%-*s the %-*s", namelen, playeraliasname, ranklen, rank);
    draw_bar(TRUE, hp, hpmax, buf);
    wmove(win, y++, x);
    wprintw(win, "%s", dungeons[u.uz.dnum].dname);

linetwovert:

    y++; /* Blank line inbetween */
    wmove(win, y++, x);

	if (youmonst.data && FlimmeringStrips) {
		attr_t flickertextB;
		int flickercolor = rn2(CLR_MAX);
		while (flickercolor == NO_COLOR) flickercolor = rn2(CLR_MAX);
		flickertextB = flickercolorattr(flickercolor);
		wprintw(win, generate_garbage_string());
		wattron(win, flickertextB);
		flickercolor = rn2(CLR_MAX);
		while (flickercolor == NO_COLOR) flickercolor = rn2(CLR_MAX);
		flickertextB = flickercolorattr(flickercolor);
		wprintw(win, "%d/%d", (hp < 0) ? 0 : hp, hpmax);
		wattron(win, flickertextB);
		flickercolor = rn2(CLR_MAX);
		while (flickercolor == NO_COLOR) flickercolor = rn2(CLR_MAX);
		flickertextB = flickercolorattr(flickercolor);
		wprintw(win, "%d/%d", u.uen, u.uenmax);
		wattron(win, flickertextB);
		return;
	}

    /* Attributes. Old  vertical order is preserved */
    print_statdiff("Strength:      ", &prevstr, ACURR(A_STR), STAT_STR);
    wmove(win, y++, x);
    print_statdiff("Intelligence:  ", &prevint, ACURR(A_INT), STAT_OTHER);
    wmove(win, y++, x);
    print_statdiff("Wisdom:        ", &prevwis, ACURR(A_WIS), STAT_OTHER);
    wmove(win, y++, x);
    print_statdiff("Dexterity:     ", &prevdex, ACURR(A_DEX), STAT_OTHER);
    wmove(win, y++, x);
    print_statdiff("Constitution:  ", &prevcon, ACURR(A_CON), STAT_OTHER);
    wmove(win, y++, x);
    print_statdiff("Charisma:      ", &prevcha, ACURR(A_CHA), STAT_OTHER);
    wmove(win, y++, x);
    wprintw(win,   "Alignment:     ");
    wprintw(win, (u.ualign.type == A_CHAOTIC ? "Chaotic" :
                  u.ualign.type == A_NEUTRAL ? "Neutral" : "Lawful"));
    wmove(win, y++, x);
    wprintw(win,   "Dungeon Level: ");

    /* Astral Plane doesn't fit */
    if (In_endgame(&u.uz))
        wprintw(win, "%s", Is_astralevel(&u.uz) ? "Astral" : "End Game");
    else
        wprintw(win, "%d", depth(&u.uz));
    wmove(win, y++, x);

#ifndef GOLDOBJ
    print_statdiff("Gold:          ", &prevau, u.ugold, STAT_GOLD);
#else
    print_statdiff("Gold:          ", &prevau, money_cnt(invent), STAT_GOLD);
#endif
    wmove(win, y++, x);

    /* HP/Pw use special coloring rules */
    attr_t hpattr, pwattr;
#ifdef STATUS_COLORS
    hpattr = hpen_color_attr(TRUE, hp, hpmax);
    pwattr = hpen_color_attr(FALSE, u.uen, u.uenmax);
#else
    int hpcolor, pwcolor;
    hpcolor = hpen_color(TRUE, hp, hpmax);
    pwcolor = hpen_color(FALSE, u.uen, u.uenmax);
    hpattr = curses_color_attr(hpcolor, 0);
    pwattr = curses_color_attr(pwcolor, 0);
#endif

    wprintw(win,   "Hit Points:    ");
    wattron(win, hpattr);
    wprintw(win, "%d/%d", (hp < 0) ? 0 : hp, hpmax);
    wattroff(win, hpattr);
    wmove(win, y++, x);

    wprintw(win,   "Magic Power:   ");
    wattron(win, pwattr);
    wprintw(win, "%d/%d", u.uen, u.uenmax);
    wattroff(win, pwattr);
    wmove(win, y++, x);

    print_statdiff("Armor Class:   ", &prevac, u.uac, STAT_AC);
    wmove(win, y++, x);

    if (Upolyd)
        print_statdiff("Hit Dice:      ", &prevlevel, mons[u.umonnum].mlevel, STAT_OTHER);
    if (flags.showexp) {
        print_statdiff("Experience:    ", &prevlevel, u.ulevel, STAT_OTHER);
        /* use waddch, we don't want to highlight the '/' */
        waddch(win, '/');
        print_statdiff("", &prevexp, u.uexp, STAT_OTHER);
    }
    wmove(win, y++, x);

    if (flags.time) {
        print_statdiff("Time:          ", &prevtime, moves, STAT_TIME);
        wmove(win, y++, x);
    }

    if (flags.showscore) {
        print_statdiff("Score:         ", &prevscore, botl_score(), STAT_OTHER);
        wmove(win, y++, x);
    }

    if (youmonst.data) curses_add_statuses(win, FALSE, TRUE, &x, &y);
}

static void
curses_add_statuses(WINDOW *win, boolean align_right,
                    boolean vertical, int *x, int *y)
{
    if (align_right) {
        /* Right-aligned statuses. Since add_status decrease one x more
           (to separate them with spaces), add 1 to x unless we have borders
           (which would offset what add_status does) */
        int mx = *x;
        int my = *y;
        getmaxyx(win, my, mx);
        if (!curses_window_has_border(STATUS_WIN))
            mx++;

        *x = mx;
    }

#define statprob(str, trouble)                                  \
    curses_add_status(win, align_right, vertical, x, y, str, trouble)

    /* Hunger */
    statprob(hu_stat[u.uhs], u.uhs != 1); /* 1 is NOT_HUNGRY (not defined here) */

    /* General troubles */
    statprob("Conf",     Confusion && !HeavyConfusion);
    statprob("XConf",     Confusion && HeavyConfusion);
    statprob("Blind",    Blind && !HeavyBlind);
    statprob("XBlind",    Blind && HeavyBlind);
    statprob("Stun",     Stunned && !HeavyStunned);
    statprob("XStun",     Stunned && HeavyStunned);
    statprob("Hallu",    Hallucination && !HeavyHallu);
    statprob("XHallu",    Hallucination && HeavyHallu);
    statprob("Ill",      (u.usick_type & SICK_NONVOMITABLE));
    statprob("FoodPois", (u.usick_type & SICK_VOMITABLE));
    statprob("Slime",    Slimed);
    statprob("Lev",    Levitation);
    statprob("Glib",    IsGlib);
    statprob("Legs",    Wounded_legs);
    statprob("Choke",    Strangled);
    statprob("Bleed",    PlayerBleeds);
    statprob("Vomit",    Vomiting);
    statprob("Elbereth",    sengr_at("Elbereth", u.ux, u.uy));
    statprob("Fear",    Feared && !HeavyFeared && !Race_if(PM_TUMBLRER) && !Role_if(PM_SOCIAL_JUSTICE_WARRIOR));
    statprob("XFear",    Feared && HeavyFeared && !Race_if(PM_TUMBLRER) && !Role_if(PM_SOCIAL_JUSTICE_WARRIOR));
    statprob("Triggered",    Feared && !HeavyFeared && (Race_if(PM_TUMBLRER) || Role_if(PM_SOCIAL_JUSTICE_WARRIOR)));
    statprob("XTriggered",    Feared && HeavyFeared && (Race_if(PM_TUMBLRER) || Role_if(PM_SOCIAL_JUSTICE_WARRIOR)));
    statprob("Numb",    Numbed && !HeavyNumbed);
    statprob("XNumb",    Numbed && HeavyNumbed);
    statprob("Paralyzed",    multi < 0);
    statprob("Vibration",    (isok(u.ux, u.uy) && invocation_pos(u.ux, u.uy)));
    statprob("Freeze",    Frozen && !HeavyFrozen);
    statprob("XFreeze",    Frozen && HeavyFrozen);
    statprob("Burn",    Burned && !HeavyBurned);
    statprob("XBurn",    Burned && HeavyBurned);
    statprob("Dim",    Dimmed && !HeavyDimmed);
    statprob("XDim",    Dimmed && HeavyDimmed);
    statprob("Stone",    Stoned);
    statprob("Held",    u.ustuck && !u.uswallow && youmonst.data && !sticks(youmonst.data));

    /* Encumbrance */
    int enc = near_capacity();
    statprob(enc_stat[enc], enc > UNENCUMBERED);
#undef statprob
}

static void
curses_add_status(WINDOW *win, boolean align_right, boolean vertical,
                  int *x, int *y, const char *str, int trouble)
{
    /* If vertical is TRUE here with no x/y, that's an error. But handle
       it gracefully since NH3 doesn't recover well in crashes. */
    if (!x || !y)
        vertical = FALSE;

    if (!trouble)
        return;

    if (!vertical && !align_right)
        waddch(win, ' ');

    /* For whatever reason, hunger states have trailing spaces. Get rid of them. */
    char buf[BUFSZ];
    strcpy(buf, str);
    int i;
    for (i = 0; (buf[i] != ' ' && buf[i] != '\0'); i++) ;

    buf[i] = '\0';
    if (align_right) {
        *x -= (strlen(buf) + 1); /* add spacing */
        wmove(win, *y, *x);
    }

    draw_trouble_str(buf);

    if (vertical) {
        wmove(win, *y, *x);
        *y += 1; /* ++ advances the pointer addr */
    }
}

/* Decrement a single highlight, return 1 if decremented to zero. zero is TRUE if we're
   zeroing the highlight. */
static int
decrement_highlight(nhstat *stat, boolean zero)
{
    if (stat->highlight_turns > 0) {
        if (zero) {
            stat->highlight_turns = 0;
            return 1;
        }

        stat->highlight_turns--;
        if (stat->highlight_turns == 0)
            return 1;
    }
    return 0;
}

/* Decrement the highlight_turns for all stats.  Call curses_update_stats
   if needed to unhighlight a stat */
void
curses_decrement_highlights(boolean zero)
{
    int unhighlight = 0;

    unhighlight |= decrement_highlight(&prevdepth, zero);
    unhighlight |= decrement_highlight(&prevstr, zero);
    unhighlight |= decrement_highlight(&prevdex, zero);
    unhighlight |= decrement_highlight(&prevcon, zero);
    unhighlight |= decrement_highlight(&prevint, zero);
    unhighlight |= decrement_highlight(&prevwis, zero);
    unhighlight |= decrement_highlight(&prevcha, zero);
    unhighlight |= decrement_highlight(&prevau, zero);
    unhighlight |= decrement_highlight(&prevlevel, zero);
    unhighlight |= decrement_highlight(&prevac, zero);
    unhighlight |= decrement_highlight(&prevexp, zero);
    unhighlight |= decrement_highlight(&prevtime, zero);
    unhighlight |= decrement_highlight(&prevscore, zero);

    if (unhighlight)
        curses_update_stats();
}
