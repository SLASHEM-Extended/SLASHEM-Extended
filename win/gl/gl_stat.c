/* Copyright (C) 2002 Andrew Apted <ajapted@users.sourceforge.net> */
/* NetHack may be freely redistributed.  See license for details.  */

/*
 * SDL/GL window port for NetHack & Slash'EM.
 *
 * Status line (bottom of the screen).
 */

#include "hack.h"

#if defined(GL_GRAPHICS) || defined(SDL_GRAPHICS)

#define WINGL_INTERNAL
#include "winGL.h"


extern const char *hu_stat[];  /* from eat.c */
extern const char *enc_stat[]; /* from botl.c */


#define TURNS_TO_HIGHLIGHT  4
#define STAT_INVALID  -999999

struct BottomStat
{
  /* current value, which tracks the actual value */
  int cur;

  /* last value (for coloring purposed) */
  int last;

  /* Whenever the `cur' field changes, we set this to
   * TURNS_TO_HIGHLIGHT, which then counts down.  When it reaches
   * zero, last := cur, and thus the value stops being highlighted.
   */
  int turns;
};

static struct BottomStat 
  st_STR    = { STAT_INVALID, STAT_INVALID, 0 },
  st_DEX    = { STAT_INVALID, STAT_INVALID, 0 },
  st_CON    = { STAT_INVALID, STAT_INVALID, 0 },
  st_INT    = { STAT_INVALID, STAT_INVALID, 0 },
  st_WIS    = { STAT_INVALID, STAT_INVALID, 0 },
  st_CHA    = { STAT_INVALID, STAT_INVALID, 0 },

  st_DEPTH  = { STAT_INVALID, STAT_INVALID, 0 },
  st_GOLD   = { STAT_INVALID, STAT_INVALID, 0 },
  st_HP     = { STAT_INVALID, STAT_INVALID, 0 },
  st_HPMAX  = { STAT_INVALID, STAT_INVALID, 0 },
  st_EN     = { STAT_INVALID, STAT_INVALID, 0 },
  st_ENMAX  = { STAT_INVALID, STAT_INVALID, 0 },
  st_AC     = { STAT_INVALID, STAT_INVALID, 0 },
  st_LEVEL  = { STAT_INVALID, STAT_INVALID, 0 };
 
/* ForestGreen (looks better than just pure green) */
#define POS_COLOR_B  TXT_MAKE(1,4,1)
#define NEG_COLOR_B  TXT_MAKE(4,0,0)
#define POS_COLOR    TXT_MAKE(1,3,1)
#define NEG_COLOR    TXT_MAKE(3,0,0)

#define AFFLICT_COLOR  B_YELLOW

/* spaces on the status window where we can put afflictions.  Ranges
 * are inclusive.  Order of the array is bottom up.
 */
static struct AfflictSpace
{
  int x1, x2;
}
afflict_spaces[3];


/* ---------------------------------------------------------------- */


static void add_str(struct TextWindow *win, const char *str)
{
  int len = strlen(str);

  if (win->write_x + len >= win->show_w)
    len = win->show_w - 1 - win->write_x;

  for (; len > 0; len--, str++)
    sdlgl_putc(win, *str);
}

static void add_val(struct TextWindow *win, int val, int old, 
    int dark, int reverse)
{
  char buffer[BUFSZ];

  if (val == STAT_INVALID)
  {
    add_str(win, "??");
    return;
  }

  sprintf(buffer, "%-1d", val);

  /* choose color based on any change */
  if (old != STAT_INVALID && val != old)
  {
    if ((reverse ? 0 : 1) == (val < old ? 1 : 0))
      win->write_col = dark ? NEG_COLOR : NEG_COLOR_B;
    else
      win->write_col = dark ? POS_COLOR : POS_COLOR_B;
  }
    
  add_str(win, buffer);

  win->write_col = dark ? GREY : L_GREY;
}

static void do_player(struct TextWindow *win, int *len)
{
  char buf[BUFSZ];

  win->write_col = GREY;

  strncpy(buf, plname, 10);
  buf[10] = 0;

  if ('a' <= buf[0] && buf[0] <= 'z') 
    buf[0] += 'A' - 'a';

  add_str(win, buf);
  add_str(win, " the ");

  if (Upolyd) 
  {
    char mbot[BUFSZ];
    int k;

    Strcpy(mbot, mons[u.umonnum].mname);
    
    for (k=0; mbot[k] != 0; k++)
    {
      if ((k == 0 || (k > 0 && mbot[k-1] == ' ')) &&
          'a' <= mbot[k] && mbot[k] <= 'z')
      {
        mbot[k] += 'A' - 'a';
      }
    }
    add_str(win, mbot);
  } 
  else
    add_str(win, rank_of(u.ulevel, Role_switch, flags.female));

  if (In_quest(&u.uz))
  {
    add_str(win, " at Home ");
    add_val(win, dunlev(&u.uz), STAT_INVALID, 1, 0);
  }
  else if (! In_endgame(&u.uz))
  {
    add_str(win, " in ");
    add_str(win, dungeons[u.uz.dnum].dname);
  }
  else
  {
    add_str(win, Is_astralevel(&u.uz) ? " on Astral Plane" : 
        " at End Game");
  }

  (*len) = win->write_x;
}

static void do_characteristics(struct TextWindow *win, int *len)
{
  win->write_col = GREY;

  add_str(win, "St:");

  if (st_STR.cur > 18) 
  {
    if (st_STR.cur > STR18(100))
      add_val(win, st_STR.cur-100, st_STR.last-100, 1, 0);
    else if (st_STR.cur < STR18(100))
    {
      add_str(win, "18/");
      add_val(win, st_STR.cur-18, st_STR.last-18, 1, 0);
    }
    else
      add_str(win, "18/**");
  } 
  else
    add_val(win, st_STR.cur, st_STR.last, 1, 0);

  add_str(win, " Dx:");
  add_val(win, st_DEX.cur, st_DEX.last, 1, 0);

  add_str(win, " Co:");
  add_val(win, st_CON.cur, st_CON.last, 1, 0);

  add_str(win, " In:");
  add_val(win, st_INT.cur, st_INT.last, 1, 0);

  add_str(win, " Wi:");
  add_val(win, st_WIS.cur, st_WIS.last, 1, 0);

  add_str(win, " Ch:");
  add_val(win, st_CHA.cur, st_CHA.last, 1, 0);

#ifdef SCORE_ON_BOTL
  if (flags.showscore)
  {
    add_str(win, " S:");
    add_val(win, (int)botl_score(), STAT_INVALID, 1, 0);
  }
#endif

  add_str(win, (u.ualign.type == A_CHAOTIC) ? "  Chaotic" :
      (u.ualign.type == A_NEUTRAL) ? "  Neutral" : "  Lawful");

  (*len) = win->write_x;
}

static void do_money_hp(struct TextWindow *win, int *len)
{
  char buf[BUFSZ];

  win->write_col = L_GREY;

  /* show depth */
  if (!In_quest(&u.uz) && !In_endgame(&u.uz))
  {
    add_str(win, "Dlvl:");
    add_val(win, st_DEPTH.cur, st_DEPTH.last, 0, 0);
  }
  
  sprintf(buf, " %c:", oc_syms[COIN_CLASS]);
  add_str(win, buf);
  add_val(win, st_GOLD.cur, st_GOLD.last, 0, 0);

  add_str(win, " HP:");
  add_val(win, st_HP.cur, st_HP.last, 0, 0);
  add_str(win, "(");
  add_val(win, st_HPMAX.cur, st_HPMAX.last, 0, 0);

  add_str(win, ") Pw:");
  add_val(win, st_EN.cur, st_EN.last, 0, 0);
  add_str(win, "(");
  add_val(win, st_ENMAX.cur, st_ENMAX.last, 0, 0);

  /* this one is reversed (lower is better) */
  add_str(win, ") AC:");
  add_val(win, st_AC.cur, st_AC.last, 0, 1);

  if (Upolyd)
  {
    add_str(win, " HD:");
    add_val(win, mons[u.umonnum].mlevel, STAT_INVALID, 0, 0);
  }
#ifdef EXP_ON_BOTL
  else if (flags.showexp)
  {
    add_str(win, " Xp:");
    add_val(win, st_LEVEL.cur, st_LEVEL.last, 0, 0);
    add_str(win, "/");
    add_val(win, (int)u.uexp, STAT_INVALID, 0, 0);
  }
#endif
  else
  {
    add_str(win, " Exp:");
    add_val(win, st_LEVEL.cur, st_LEVEL.last, 0, 0);
  }

#ifdef SHOW_WEIGHT
  if (flags.showweight)
  {
    add_str(win, " Wt:");
    add_val(win, inv_weight()+weight_cap(), STAT_INVALID, 0, 0); 
    add_str(win, "/");
    add_val(win, weight_cap(), STAT_INVALID, 0, 0); 
  }
#endif
 
  if (flags.time)
  {
    add_str(win, " T:");
    add_val(win, moves, STAT_INVALID, 0, 0);
  }

  (*len) = win->write_x;
}

static void add_one_affliction(struct TextWindow *win, const char *name)
{
  int x, y;
  int size = strlen(name) + 1;  /* 1 == a padding space */
  int space;

  for (y=0; y < 3; y++)
  {
    space = afflict_spaces[y].x2 - afflict_spaces[y].x1 + 1;

    if (size < space)
      break;
  }

  if (y >= 3)
  {
    /* NO ROOM !  Too bad... */
    return;
  }

  afflict_spaces[y].x2 -= size;

  x = afflict_spaces[y].x2 + 1;

  /* draw affliction name */
  assert(win->base->is_text);

  sdlgl_store_str(win->base, x, y, name, 99, AFFLICT_COLOR);
}

static void do_afflictions(struct TextWindow *win)
{
  int cap = near_capacity();

#ifndef VANILLA_GLHACK  /* Slash'EM only */
  if (Invulnerable)
    add_one_affliction(win, "Invul");
#endif
  
  if (Levitation) 
    add_one_affliction(win, "Lev");
  else if (Flying) 
    add_one_affliction(win, "Fly");

  /* add Hungry (etc) */
  if (hu_stat[u.uhs][0] != ' ')
    add_one_affliction(win, hu_stat[u.uhs]);

  if (Blind) 
    add_one_affliction(win, "Blind");
  
  if (Sick) 
  {
    if (u.usick_type & SICK_VOMITABLE)
      add_one_affliction(win, "FoodPois");
    else
      add_one_affliction(win, "Ill");
  }

  if (Confusion) 
    add_one_affliction(win, "Conf");

  if (Stunned) 
    add_one_affliction(win, "Stun");
  
  if (Hallucination)  
    add_one_affliction(win, "Hallu");
  
  if (Slimed) 
    add_one_affliction(win, "Slime");
 
  /* -AJA- this idea by Roderick Schertler */
  if (u.ustuck && !u.uswallow && !sticks(youmonst.data))
    add_one_affliction(win, "Held");

  /* display Burdened (etc) */
  if (cap > UNENCUMBERED)
    add_one_affliction(win, enc_stat[cap]);
}

void sdlgl_write_status(struct TextWindow *win)
{
  int dummy;

  assert(win->base);

  sdlgl_clear(win);
  sdlgl_home(win);

  do_player(win, &dummy);
  sdlgl_putc(win, '\n');

  do_characteristics(win, &afflict_spaces[1].x1);
  sdlgl_putc(win, '\n');

  do_money_hp(win, &afflict_spaces[0].x1);

  /* set up the spaces for placing afflictions.  We clobber the first
   * line (player and dungeon name) as a last resort.
   */
  afflict_spaces[2].x1 = 0;
  
  afflict_spaces[0].x2 = afflict_spaces[1].x2 =
  afflict_spaces[2].x2 = win->show_w - 1;
 
  do_afflictions(win);
}

/* ---------------------------------------------------------------- */

static void do_bottom_stat(int step, struct BottomStat *bot, int value)
{
  /* setting initial value ? */
  if (bot->last == STAT_INVALID)
  {
    bot->last = bot->cur = value;
    return;
  }
  
  if (value != bot->cur)
  {
    if (bot->turns > 0)
      bot->last = bot->cur;

    bot->cur = value;
    bot->turns = TURNS_TO_HIGHLIGHT;
    return;
  }

  if (step > 0 && bot->turns > 0)
  {
    bot->turns = max(0, bot->turns - step);

    if (bot->turns == 0)
      bot->last = bot->cur;
  }
}

void sdlgl_update_status(int step)
{
  int hp = max(0, Upolyd ? u.mh : u.uhp);
  int hpmax = Upolyd ? u.mhmax : u.uhpmax;

  do_bottom_stat(step, &st_STR, ACURR(A_STR));
  do_bottom_stat(step, &st_DEX, ACURR(A_DEX));
  do_bottom_stat(step, &st_CON, ACURR(A_CON));
  do_bottom_stat(step, &st_INT, ACURR(A_INT));
  do_bottom_stat(step, &st_WIS, ACURR(A_WIS));
  do_bottom_stat(step, &st_CHA, ACURR(A_CHA));

  do_bottom_stat(step, &st_DEPTH, depth(&u.uz));
  do_bottom_stat(step, &st_GOLD,  (int)u.ugold);
  do_bottom_stat(step, &st_HP, hp);
  do_bottom_stat(step, &st_HPMAX, hpmax);
  do_bottom_stat(step, &st_EN, u.uen);
  do_bottom_stat(step, &st_ENMAX, u.uenmax);
  do_bottom_stat(step, &st_AC, u.uac);
  do_bottom_stat(step, &st_LEVEL, u.ulevel);
}


#endif  /* GL_GRAPHICS */
/*gl_stat.c*/
