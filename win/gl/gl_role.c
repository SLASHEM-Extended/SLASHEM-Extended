/* Copyright (C) 2002 Andrew Apted <ajapted@users.sourceforge.net> */
/* NetHack may be freely redistributed.  See license for details.  */

/*
 * SDL/GL window port for NetHack & Slash'EM.
 *
 * Character selection code (Role, Race, Gender, Alignment).
 */

#include "hack.h"
#include "dlb.h"
#include "patchlevel.h"

#if defined(GL_GRAPHICS) || defined(SDL_GRAPHICS)

#define WINGL_INTERNAL
#include "winGL.h"

#include <string.h>
#include <ctype.h>


/* value chosen not to conflict with existing ROLE values */
#define ROLE_QUIT  (-7)


#define HEIGHT_PICKER  7
#define DEPTH_PICKER   8


/* callback functions used to create menus for player_selection().
 * Each one returns: 1 for a valid name, 0 for an invalid name, and -1
 * when the list is exhausted.  For 1 and 0, the name is copied into
 * the buffer provided.
 */
static int query_role_name(int what, char *buf)
{
  if (roles[what].name.m == NULL)
    return -1;

  if (flags.initgend >= 0 && flags.female && roles[what].name.f)
    strcpy(buf, roles[what].name.f);
  else
    strcpy(buf, roles[what].name.m);

  return ok_role(what, flags.initrace, flags.initgend,
      flags.initalign) ? 1 : 0;
}

static int query_race_name(int what, char *buf)
{
  if (races[what].noun == NULL)
    return -1;

  strcpy(buf, races[what].noun);

  return ok_race(flags.initrole, what, flags.initgend,
      flags.initalign) ? 1 : 0;
}

static int query_gender_name(int what, char *buf)
{
  if (what >= ROLE_GENDERS)
    return -1;

  strcpy(buf, genders[what].adj);
  
  return ok_gend(flags.initrole, flags.initrace, what,
      flags.initalign) ? 1 : 0;
}

static int query_align_name(int what, char *buf)
{
  if (what >= ROLE_ALIGNS)
    return -1;

  strcpy(buf, aligns[what].adj);

  return ok_align(flags.initrole, flags.initrace, flags.initgend,
      what) ? 1 : 0;
}

/* print a string with line splitting.  Based on sdlgl_putstr(), but
 * has some significant differences now.
 */
static void raw_puts_split(struct TextWindow *win, const char *str)
{
  int len = strlen(str);

  while (len > 0)
  {
    int use_len;
    int width = win->show_w;
    
    /* does it fit yet ? */
    if (len < width)
    {
      sdlgl_puts_nolf(win, str);
      return;
    }

    /* choose a good place to break the line */
    for (use_len = width-1; use_len > width/2; use_len--)
      if (str[use_len] == ' ')
        break;

    /* no space in latter half of string ?  Must be a very long token,
     * so we might as well split it at the screen edge.
     */
    if (str[use_len] != ' ')
      use_len = width-1;

    for (; use_len > 0; use_len--, str++, len--)
      if (*str != '\n' && *str != '\r')
        sdlgl_putc(win, *str);

    sdlgl_putc(win, '\n');

    /* remove leading spaces */
    for (; *str == ' '; str++, len--) { }
  }
}

/* may return ROLE_RANDOM, or ROLE_NONE when the list is empty (which
 * can only mean that the role/race/gender/alignment combination that
 * the user supplied on the command line was not possible), or
 * ROLE_QUIT if the user pressed escape.
 */
static int do_player_selection_menu(const char *type,
    int (* func)(int what, char *buf))
{
  int i, kind;
  int valid = 0, total = 0, last = -1;

  winid window;
  anything any;
  int attr, choice;

  menu_item *selected = NULL;

  int used_accs[52] = { 0, };
  char accel;
  
  char name[BUFSZ];
  char prompt[BUFSZ];
  char randstr[BUFSZ];

  /* count valid entries */
  for (i=0; ; i++)
  {
    kind = (* func)(i, name);

    if (kind < 0)
      break;

    total++;

    if (kind > 0)
    {
      valid++;
      last = i;
    }
  }
  
  if (valid == 0)
    return ROLE_NONE;

  assert(last >= 0);

  /* -AJA- this is disabled -- it's better IMHO to show the user the
   *       single choice, letting them press ESC and pick some other
   *       role/race/gender if they're not satisfied.
   */
#if 0
  if (valid == 1)
    return last;
#endif
 
  /* create menu */

  sprintf(prompt,  "Pick your %s", type);
  sprintf(randstr, "Random %s", type);
 
  window = Sdlgl_create_nhwindow(NHW_MENU);
  assert(window != WIN_ERR);

  Sdlgl_start_menu(window);

  for (i=0; i < total; i++)
  {
    kind = (* func)(i, name);
    any.a_void = 0;
    attr = ATR_NONE;

    /* choose accelerator key */
    accel = lowc(name[0]);

    if (accel < 'a' || accel > 'z')
      accel = 0;
    else
    {
      if (! used_accs[accel - 'a'])
        used_accs[accel - 'a'] = 1;
      else
      {
        accel = highc(accel);

        if (! used_accs[accel - 'A' + 26])
          used_accs[accel - 'A' + 26] = 1;
        else
          accel = 0;
      }
    }

    if (kind > 0)
    {
      /* must add one, zero is not allowed */
      any.a_int = 1 + i;
    }
    else
    {
      /* shift name right (to align), and make dim */
      memmove(name + 5, name, strlen(name) + 1);
      memcpy(name, "     ", 5);
      attr = ATR_DIM;
    }

    Sdlgl_add_menu(window, NO_GLYPH, &any, accel,
        0 /* no groupacc */, attr, name, MENU_UNSELECTED);
  }

  /* add `Random' line (unless there's only one choice) */
  if (valid > 1)
  {
    any.a_int = 1 + total;

    Sdlgl_add_menu(window, NO_GLYPH, &any, '*',
          0 /* no groupacc */, ATR_NONE, randstr, MENU_UNSELECTED);
  }

  Sdlgl_end_menu(window, prompt);

  /* get result back from menu */

  i = Sdlgl_select_menu(window, PICK_ONE, &selected);
  Sdlgl_destroy_nhwindow(window);

  if (i <= 0)
    return ROLE_QUIT;

  assert(i == 1 && selected);

  choice = selected[0].item.a_int - 1;
  free(selected);

  assert(0 <= choice && choice <= total);

  if (choice == total)
    return ROLE_RANDOM;

  return choice;
}

/* These return -1 if cancelled, otherwise 0.
 */
static int select_auto_pick(int *pick4u)
{
  struct TextWindow *win;
  int pixel_h;
  int ch;

  /* handle the fully random (-@) option */
  if (flags.randomall)
  {
    (* pick4u) = 1;
    return 0;
  }
  
  /* create text & tile windows */
  win = sdlgl_new_textwin(NHW_TEXT);  /* type unimportant */
   
  win->show_w = sdlgl_width / sdlgl_font_message->tile_w;
  win->show_h = HEIGHT_PICKER;

  pixel_h = sdlgl_font_message->tile_h * win->show_h;
  
  win->base = sdlgl_new_tilewin(sdlgl_font_message, win->show_w,
      win->show_h, 1,0);
   
  sdlgl_map_tilewin(win->base, 0, sdlgl_height - pixel_h,
      sdlgl_width, pixel_h, DEPTH_PICKER);

  /* do the query */
  {
    char pbuf[QBUFSZ];
    const char *prompt = "Shall I pick a character for you? [ynq] ";
    
    if (flags.initrole != ROLE_NONE || flags.initrace  != ROLE_NONE ||
        flags.initgend != ROLE_NONE || flags.initalign != ROLE_NONE)
    {
      prompt = build_plselection_prompt(pbuf, QBUFSZ, flags.initrole,
        flags.initrace, flags.initgend, flags.initalign);
    }

    sdlgl_home(win);

    /* if the copyright wasn't shown on the splash screen (because it
     * was disabled), then show it here.
     */
    if (! iflags.wc_splash_screen)
    {
      sdlgl_puts(win, COPYRIGHT_BANNER_A "\n");
      sdlgl_puts(win, COPYRIGHT_BANNER_B "\n");
      sdlgl_puts(win, COPYRIGHT_BANNER_C "\n");
      sdlgl_puts(win, "\n");
    }
    
    raw_puts_split(win, prompt);
  }

  do
  {
    sdlgl_flush();
    ch = sdlgl_get_key(0);
  }
  while (strchr("ynq\033", ch) == NULL);

  sdlgl_unmap_tilewin(win->base);
  sdlgl_free_textwin(win);

  if (ch == 'q' || ch == '\033')
    return -1;

  /* should we randomly pick for the player ? */
  (* pick4u) = (ch == 'y') ? 1 : 0;

  return 0;
}

static int select_a_role(int pick4u)
{
  int choice;

  while (flags.initrole < 0)
  {
    if (pick4u || flags.initrole == ROLE_RANDOM || flags.randomall)
    {
      flags.initrole = pick_role(flags.initrace, flags.initgend,
          flags.initalign, PICK_RANDOM);
      break;
    }

    /* select a role */
    for (;;)
    {
      choice = do_player_selection_menu("role", query_role_name);
      
      if (choice != ROLE_NONE)
        break;

      /* reset */
      if (flags.initalign >= 0) flags.initalign = ROLE_NONE;
      else if (flags.initgend >= 0) flags.initgend = ROLE_NONE;
      else if (flags.initrace >= 0) flags.initrace = ROLE_NONE;
      else
        return 0;
    }

    if (choice == ROLE_QUIT)
    {
      flags.initrole = ROLE_NONE;
      return -1;
    }

    flags.initrole = choice;
  }

  return 0;
}

static int select_a_race(int pick4u)
{
  int choice;

  while (!validrace(flags.initrole, flags.initrace))
  {
    if (pick4u || flags.initrace == ROLE_RANDOM || flags.randomall)
    {
      flags.initrace = pick_race(flags.initrole, flags.initgend,
          flags.initalign, PICK_RANDOM);
      break;
    }

    /* select a race */
    for (;;)
    {
      choice = do_player_selection_menu("race", query_race_name);
    
      if (choice != ROLE_NONE)
        break;

      /* reset */
      if (flags.initalign >= 0) flags.initalign = ROLE_NONE;
      else if (flags.initgend >= 0) flags.initgend = ROLE_NONE;
      else
        return 0;
    }

    if (choice == ROLE_QUIT)
    {
      flags.initrole = ROLE_NONE;
      flags.initrace = ROLE_NONE;
      return -1;
    }

    flags.initrace = choice;
  }

  return 0;
}

static int select_a_gender(int pick4u)
{
  int choice;

  while (!validgend(flags.initrole, flags.initrace, flags.initgend))
  {
    if (pick4u || flags.initgend == ROLE_RANDOM || flags.randomall)
    {
      flags.initgend = pick_gend(flags.initrole, flags.initrace,
          flags.initalign, PICK_RANDOM);
      break;
    }
    
    /* select a gender */
    for (;;)
    {
      choice = do_player_selection_menu("gender", query_gender_name);
    
      if (choice != ROLE_NONE)
        break;

      /* reset */
      if (flags.initalign >= 0)
        flags.initalign = ROLE_NONE;
      else
        return 0;
    }

    if (choice == ROLE_QUIT)
    {
      flags.initrace = ROLE_NONE;
      flags.initgend = ROLE_NONE;
      return -1;
    }

    flags.initgend = choice;
  }

  return 0;
}

static int select_an_alignment(int pick4u)
{
  int choice;

  while (!validalign(flags.initrole, flags.initrace, flags.initalign))
  {
    if (pick4u || flags.initalign == ROLE_RANDOM || flags.randomall)
    {
      /* pick_align */
      flags.initalign = pick_align(flags.initrole, flags.initrace,
          flags.initgend, PICK_RANDOM);
      break;
    }

    /* select an alignment */
    for (;;)
    {
      choice = do_player_selection_menu("alignment", query_align_name);
    
      if (choice != ROLE_NONE)
        break;

      /* nothing to reset ! */
      return 0;
    }

    if (choice == ROLE_QUIT)
    {
      flags.initgend  = ROLE_NONE;
      flags.initalign = ROLE_NONE;
      return -1;
    }

    flags.initalign = choice;
  }

  return 0;
}

#define INIT_IS_RANDOM(val)  \
    ((val) == ROLE_RANDOM || \
     (flags.randomall && (val) == ROLE_NONE))
 
static void do_random_role_checks(void)
{
  if (INIT_IS_RANDOM(flags.initrole))
  {
    flags.initrole = pick_role(flags.initrace, flags.initgend,
        flags.initalign, PICK_RANDOM);
  }

  if (INIT_IS_RANDOM(flags.initrace))
  {
    flags.initrace = pick_race(flags.initrole, flags.initgend,
        flags.initalign, PICK_RANDOM);
  }

  if (INIT_IS_RANDOM(flags.initalign))
  {
    flags.initalign = pick_align(flags.initrole, flags.initrace,
        flags.initgend, PICK_RANDOM);
  }

  if (INIT_IS_RANDOM(flags.initgend))
  {
    flags.initgend = pick_gend(flags.initrole, flags.initrace,
        flags.initalign, PICK_RANDOM);
  }
}

void Sdlgl_player_selection(void)
{
  /* -AJA- Note that the initrole, initrace, initgend and initalign
   *       fields of the `flag' global have been set to ROLE_NONE in
   *       the initoptions() routine in src/options.c.  Those values
   *       may then be updated by the system code (sys/unixmain.c)
   *       depending on command line options.
   */
 
  int pick4u;

  /* avoid unnecessary prompts further down */
  do_random_role_checks();
  rigid_role_checks();

  if (select_auto_pick(&pick4u) < 0)
    sdlgl_error("Quit from pick-for-you prompt.\n");

  for (;;)
  {
    if (select_a_role(pick4u) < 0)
      sdlgl_error("Quit from player selection menu.\n");

    if (select_a_race(pick4u) < 0)
      continue;

    if (select_a_gender(pick4u) < 0)
      continue;

    if (select_an_alignment(pick4u) < 0)
      continue;

    break;
  }

  if (flags.initrole < 0 || flags.initrace < 0 ||
      flags.initgend < 0 || flags.initalign < 0)
  {
    raw_printf("WARNING: failed to select a valid character !  "
        "(%d,%d,%d,%d)\n", flags.initrole, flags.initrace,
        flags.initgend, flags.initalign);

    /* do nothing -- let init_role() deal with it */
  }
}

#endif /* GL_GRAPHICS */
/*gl_role.c*/
