/* Copyright (C) 2002 Andrew Apted <ajapted@users.sourceforge.net> */
/* NetHack may be freely redistributed.  See license for details.  */

/*
 * SDL/GL window port for NetHack & Slash'EM.
 *
 * This file contains menu (NHW_MENU) windows and other menu related
 * functions.
 */

#include "hack.h"
#include "dlb.h"
#include "patchlevel.h"

#if defined(GL_GRAPHICS) || defined(SDL_GRAPHICS)

#define WINGL_INTERNAL
#include "winGL.h"

#include <string.h>
#include <ctype.h>
#include <signal.h>


extern char mapped_menu_cmds[]; /* from options.c */


/* FORWARD DECLS */
static void draw_menu_items(struct TextWindow *win, int how);
 
/*
 * A string containing all the default commands -- to add to a list
 * of acceptable inputs.
 */
static const char default_menu_cmds[] = 
{
  MENU_FIRST_PAGE,
  MENU_LAST_PAGE,
  MENU_NEXT_PAGE,
  MENU_PREVIOUS_PAGE,
  MENU_SELECT_ALL,
  MENU_UNSELECT_ALL,
  MENU_INVERT_ALL,
  MENU_SELECT_PAGE,
  MENU_UNSELECT_PAGE,
  MENU_INVERT_PAGE,

  0  /* null terminator */
};


static void menu_set_all(struct TextWindow *win, char groupacc)
{
  struct MenuItem *item;

  for (item=win->items; item; item=item->next)
  {
    if (groupacc && groupacc != item->groupacc)
      continue;
    
    if (item->identifier.a_void && !item->selected)
    {
      item->selected = 1;
      item->count = -1;
    }
  }
}

static void menu_unset_all(struct TextWindow *win, char groupacc)
{
  struct MenuItem *item;

  for (item=win->items; item; item=item->next)
  {
    if (groupacc && groupacc != item->groupacc)
      continue;
    
    if (item->identifier.a_void && item->selected)
    {
      item->selected = 0;
      item->count = -1;
    }
  }
}

static void menu_invert_all(struct TextWindow *win, char groupacc)
{
  struct MenuItem *item;

  for (item=win->items; item; item=item->next)
  {
    if (groupacc && groupacc != item->groupacc)
      continue;
    
    if (!item->identifier.a_void)
      continue;

    item->selected = item->selected ? 0 : 1;
    item->count = -1;
  }
}

/* reverse the item list.  Can handle NULL as an input.
 */
static struct MenuItem *reverse_items(struct MenuItem *items)
{
  struct MenuItem *head = NULL;
  struct MenuItem *next = NULL;

  for (; items; items = next)
  {
    next = items->next;

    items->next = head;
    head = items;
  }

  return head;
}

/* collect group accelerators, same as in ../tty/wintty.c.
 * Ignore accelerators for PICK_NONE.  For PICK_ONE, only accept
 * accelerators that mach exactly one entry.  Grab them all for
 * PICK_ANY.
 */
static void collect_group_accelerators(int how, char *gacc,
    struct TextWindow *win)
{
  struct MenuItem *curr;

  char *rp;
  int accel;

  int n;
  int gcnt[128] = { 0, };

  for (n=0, curr=win->items; curr; curr=curr->next)
  {
    accel = (int)curr->groupacc & 0x7F; /* make it ASCII safe */

    if (! accel)
      continue;

    if (! gcnt[accel])
      gacc[n++] = accel;

    gcnt[accel]++;
  }
  
  gacc[n] = 0;
 
  if (how == PICK_ONE)
  {
    for(rp=gacc; *gacc; gacc++)
    {
      accel = (int)*gacc & 0x7F;

      if (gcnt[accel] == 1)
        *rp++ = accel;
    }

    *rp = 0;
  }
}

/* returns < 0 if cancelled */
int sdlgl_process_menu_window(int window, struct TextWindow *win, int how)
{
  struct MenuItem *item;
  struct TileWindow *kt_win = NULL;
  
  int pixel_w, pixel_h;
  int total_choices;

  int finished  = 0;
  int cancelled = 0;
  int counting  = 0;

  int count = 0;
  int ch;
  
  /* for group accelerators */
  char gacc[128] = { 0, };
  char kt_buf[16];

  assert(! win->base);

  if (! win->items)
    return -1;

  /* calc_w/h has already been calculated in end_menu(). 
   */
  if (win->is_menu != 1 || win->calc_w <= 0 || win->calc_h <= 0)
  {
    raw_printf("process_menu_window(%d): bad menu", window);
    return -1;
  }

  sdlgl_top_win = window;

  win->base = sdlgl_new_tilewin(sdlgl_font_menu, win->calc_w, 
      win->calc_h, 1,0);
  
  /* choose background color */
  win->base->background = (how == PICK_NONE) ? MENU_NONE_COL :
      (how == PICK_ONE) ? MENU_ONE_COL : MENU_ANY_COL;

  draw_menu_items(win, how);

  pixel_w = min(sdlgl_width,  win->base->scale_w * win->calc_w);
  pixel_h = min(sdlgl_height, win->base->scale_h * win->calc_h);

  win->show_w = pixel_w / win->base->scale_w;
  win->show_h = pixel_h / win->base->scale_h;

  assert(0 < win->show_w && win->show_w <= win->calc_w); 
  assert(0 < win->show_h && win->show_h <= win->calc_h);

  sdlgl_map_tilewin(win->base, sdlgl_width - pixel_w,
      sdlgl_height - pixel_h, pixel_w, pixel_h, 10+window);

  /* set focus */
  win->focus_x = 0;
  win->focus_y = win->calc_h - win->show_h;

  /* this flushes: */
  sdlgl_pan_window(window, 0, 0);

  if (how != PICK_NONE)
    collect_group_accelerators(how, gacc, win);

  total_choices = 0;
  if (how != PICK_NONE)
  {
    for (item = win->items; item; item = item->next)
      if (item->identifier.a_void)
        total_choices++;
  }
   
#if 0  /* DEBUGGING */
  fprintf(stderr, "GROUPACC: %c `%s'\n",
  (how == PICK_NONE) ? 'N' : (how == PICK_ONE) ? '1' : 'A', gacc);
#endif

  while (!finished && !cancelled)
  {
    int bx, by, bmod;

    /* close count window ? */
    if (kt_win && !counting)
    {
      sdlgl_unmap_tilewin(kt_win);
      sdlgl_free_tilewin(kt_win);
      kt_win = NULL;
    }
    
    sdlgl_flush();
    ch = sdlgl_get_poskey(0, &bx, &by, &bmod);

    if (ch == 0)  /* mouse button */
    {
      int index = win->calc_h - 1 - by;
      assert(index >= 0);

      if (how == PICK_NONE)
        continue;

      if (index == 0)  /* bottom padding line */
        continue;

      item = win->items;

      for (; index > 1 && item; index--)
        item = item->next;

      if (index <= 1 && item && item->identifier.a_void)
      {
        if (counting && count > 0)
        {
          item->selected = 1;
          item->count = count;
        }
        else
          item->selected = item->selected ? 0 : 1;
          
        /* redraw menu */
        draw_menu_items(win, how);
        sdlgl_flush();

        if (how == PICK_ONE)
          finished = 1;
      }

      counting = 0;
      continue;
    }
    
    if (ch == '\033')
    {
      if (counting)
      {
        counting = 0;
        continue;
      }

      cancelled = 1;
      break;
    }
    
    if (ch == '\n' || ch == '\r' || ch == ' ')
    {
      /* handle counting.  Many times I have simply hit enter instead
       * of an accelerator (especially when taking gold out of a bag).
       */
      if (counting && count > 0 && how == PICK_ANY && total_choices > 1)
        continue;

      if (counting && count > 0 && how == PICK_ANY && total_choices == 1)
      {
        for (item = win->items; item; item = item->next)
          if (item->identifier.a_void)
            break;

        assert(item);

        item->selected = 1;
        item->count = count;

        draw_menu_items(win, how);
        sdlgl_flush();

        counting = 0;
        continue;
      }

      finished = 1;
      break;
    }

    if (how == PICK_NONE)
      continue;

    /* counting */
    if (digit(ch) && (ch != '0' || counting))
    {
      if (!counting)
      {
        counting = 1;
        count = 0;
      }

      /* prevent overflow */
      if (count <= 99999)
        count = count * 10 + (ch - '0');

      /* adjust count window (create if needed) */
      if (! kt_win)
      {
        int x, y;
        int w, h;
        
        kt_win = sdlgl_new_tilewin(sdlgl_font_menu, 16, 1, 1,0);
        kt_win->background = win->base->background;

        w = 16 * kt_win->scale_w;
        h =  1 * kt_win->scale_h;

        /* if the menu window fits vertically on the screen with space
         * underneath for a line, place the count window underneath it
         * and touching the right edge.  Otherwise squash it onto the
         * bottom-most line.
         */
        x = sdlgl_width  - w;
        y = sdlgl_height - pixel_h;

        if (y >= h)
        {
          y -= h;
        }
         
        sdlgl_map_tilewin(kt_win, x, y, w, h, 11+window);
      }

      sdlgl_blank_area(kt_win, 0, 0, 16, 1);

      sprintf(kt_buf, "Count: %d", count);
      assert(strlen(kt_buf) <= 15);

      sdlgl_store_str(kt_win, 15 - strlen(kt_buf), 0,
          kt_buf, 16, B_CYAN);

      continue;
    }

    /* group accelerators */
    if (strchr(gacc, ch))
    {
       menu_invert_all(win, ch);

       /* if PICK_ONE, gacc only contains accels for single items */
       if (how == PICK_ONE) 
         finished = 1;

       draw_menu_items(win, how);
       counting = 0;
       continue;
    }

    if (letter(ch) || ch == '#' || ch == '$' || ch == '*')
    {
      for (item=win->items; item; item=item->next)
      {
        if (item->identifier.a_void && ch && ch == item->accelerator)
        {
          if (counting && count > 0)
          {
            item->selected = 1;
            item->count = count;
          }
          else
            item->selected = item->selected ? 0 : 1;
          
          /* redraw menu */
          draw_menu_items(win, how);
          sdlgl_flush();

          if (how == PICK_ONE)
            finished = 1;

          break;  /* from inner for loop */
        }
      }
      counting = 0;
      continue;
    }
    
    counting = 0;

    switch (ch)
    {
      case MENU_SELECT_ALL:
        if (how == PICK_ANY)
        {
          menu_set_all(win, 0);
          draw_menu_items(win, how);
        }
        break;

      case MENU_UNSELECT_ALL:
        if (how == PICK_ANY)
        {
          menu_unset_all(win, 0);
          draw_menu_items(win, how);
        }
        break;

      case MENU_INVERT_ALL:
        if (how == PICK_ANY)
        {
          menu_invert_all(win, 0);
          draw_menu_items(win, how);
        }
        break;

      default:
        /* beep ? */
        break;
    }
  }
  
  sdlgl_unmap_tilewin(win->base);
  sdlgl_free_tilewin(win->base);
  win->base = NULL;

  sdlgl_top_win = WIN_ERR;

  if (kt_win)
  {
    sdlgl_unmap_tilewin(kt_win);
    sdlgl_free_tilewin(kt_win);
    kt_win = NULL;
  }

  return cancelled ? -1 : 0;
}

void sdlgl_clear_menu_items(struct TextWindow *win)
{
  struct MenuItem *item;

  while (win->items)
  {
    item = win->items;
    win->items = item->next;

    free(item->str);
    free(item);
  }

  assert(win->items == NULL);
}

static void draw_menu_items(struct TextWindow *win, int how)
{
  struct TileWindow *base;
  struct MenuItem *item;

  int x, y;
  int maxlen;

  tilecol_t tilecol;

  char buffer[BUFSZ];

  base = win->base;
  
  assert(base);
  assert(base->is_text);
 
  for (item=win->items,y=1; item; item=item->next,y++)
  {
    assert(y >= 0);

    if (item->identifier.a_void)
    {
      Sprintf(buffer, "  %c ", item->accelerator ?
          item->accelerator : '?');

      /* -AJA- displaying the full count will disturb the nice
       *       alignment when count >= 10, but I think that showing
       *       the value to the user is more important.
       */
      if (item->selected && item->count > 0)
        Sprintf(buffer+strlen(buffer), "%d ", item->count);
      else if (item->selected)
        strcat(buffer, "+ ");
      else
        strcat(buffer, "- ");
    }
    else
    {
      /* padding character, looks better */
      strcpy(buffer, " ");
    }
     
    strncpy(buffer+strlen(buffer), item->str, BUFSZ-strlen(buffer)-1);
    buffer[BUFSZ-1] = 0;
        
    tilecol = sdlgl_attr_to_tilecol(item->attr);
    
    x = 0;

    maxlen = win->calc_w - x;

    if (maxlen > 0)
      x += sdlgl_store_str(base, x, y, buffer, maxlen, tilecol);

    maxlen = win->calc_w - x;

    if (maxlen > 0)
      sdlgl_blank_area(base, x, y, maxlen, 1);
  }
}


void Sdlgl_start_menu(winid window)
{
  struct TextWindow *win;
  
  if (window == WIN_ERR)
    return;

  assert (0 <= window && window < MAXWIN && text_wins[window]);
  win = text_wins[window];

  if (win->type != NHW_MENU || win->is_menu == 0)
    return;

  win->is_menu = 1;
  win->calc_w = win->calc_h = 0;

  sdlgl_clear_menu_items(win);

  Sdlgl_clear_nhwindow(window);
}

/*
 * Add a menu item to the menu list.
 */
void Sdlgl_add_menu(winid window, int glyph, const anything *identifier, 
    CHAR_P ch, CHAR_P groupacc, int attr, const char *str, 
    BOOLEAN_P preselected)
{
  struct TextWindow *win;
  struct MenuItem *item;
  
  if (window == WIN_ERR)
    return;

  assert (0 <= window && window < MAXWIN && text_wins[window]);
  win = text_wins[window];

  if (win->type != NHW_MENU || win->is_menu != 1)
    return;

  /* highlight non-selectable entries */
  if (attr == ATR_NONE && !identifier->a_void)
    attr = ATR_ULINE;

  /* create item record */
   
  item = (struct MenuItem *) alloc(sizeof(struct MenuItem));
  memset(item, 0, sizeof(struct MenuItem));

  item->glyph = glyph;
  item->identifier = *identifier;
  item->accelerator = ch;
  item->groupacc = groupacc;
  item->attr = attr;
  item->str = strdup(str ? str : "");

  item->selected = preselected;
  item->count = -1;

  /* link it in (to head of list) */
  
  item->next = win->items;
  win->items = item;
}

/*
 * End a menu in this window, window must a type NHW_MENU.  This routine
 * processes the string list.  We also decide on the width and height
 * of the window.  Finally, we supply accelerator keys for menus that
 * don't supply their own.
 */
void Sdlgl_end_menu(winid window, const char *prompt)
{
  struct TextWindow *win;
  struct MenuItem *item;
  
  int used_accs[52] = { 0, };
  int curr_acc = 0;

  if (window == WIN_ERR)
    return;

  assert (0 <= window && window < MAXWIN && text_wins[window]);
  win = text_wins[window];

  if (win->type != NHW_MENU || win->is_menu != 1)
    return;

  /* put the prompt at the beginning of the menu */

  if (prompt) 
  {
    anything any;
    any.a_void = 0; /* not selectable */

    win->items = reverse_items(win->items);

    Sdlgl_add_menu(window, NO_GLYPH, &any, 0,0, ATR_BOLD, "", FALSE);
    Sdlgl_add_menu(window, NO_GLYPH, &any, 0,0, ATR_BOLD, prompt, FALSE);

    /* this double reversal... bit of a hack */
    win->items = reverse_items(win->items);
  }

  /* compute total width & height */

  win->calc_w = 0;
  win->calc_h = 0;

  for (item=win->items; item; item=item->next)
  {
    int width = min(CO - 2, 5 + strlen(item->str));

    if (width > win->calc_w)
      win->calc_w = width;

    win->calc_h++;

    /* mark the used accelerators */
    if (item->identifier.a_void)
    {
      if ('a' <= item->accelerator && item->accelerator <= 'z')
        used_accs[item->accelerator - 'a'] = 0;

      if ('A' <= item->accelerator && item->accelerator <= 'Z')
        used_accs[item->accelerator - 'A' + 26] = 0;
    }
  }

  /* left & bottom padding character */
  win->calc_w += 1;
  win->calc_h += 1;

  assert(win->calc_w > 0);
  assert(win->calc_h > 0);

  /* supply keyboard accelerators, when missing */
  win->items = reverse_items(win->items);

  for (item=win->items; item; item=item->next)
  {
    if (!item->identifier.a_void || item->accelerator)
      continue;

    for (; curr_acc < 52 && used_accs[curr_acc]; curr_acc++)
    { /* nothing needed */ }

    if (curr_acc >= 52)  /* none left */
      continue;
    
    item->accelerator = (curr_acc < 26) ? ('a' + curr_acc) : 
      ('A' + curr_acc - 26);
    used_accs[curr_acc++] = 1;
  }

  /* some hackitude here, to get right order */
  win->items = reverse_items(win->items);
}

int Sdlgl_select_menu(winid window, int how, menu_item **menu_list)
{
  struct TextWindow *win;
  struct MenuItem *item;
  
  menu_item *mi;
  int num;

  *menu_list = NULL;

  if (window == WIN_ERR)
    return -1;

  assert (0 <= window && window < MAXWIN && text_wins[window]);
  win = text_wins[window];

  if (win->type != NHW_MENU || win->is_menu != 1)
    return -1;

  if (sdlgl_process_menu_window(window, win, how) < 0)
    return -1;

  /* see how many items were selected */
  for (item=win->items, num=0; item; item=item->next)
    if (item->selected && item->count != 0)
      num++;

  if (num == 0)
    return 0;

  /* create record of selected items */
  *menu_list = (menu_item *) alloc(num * sizeof(menu_item));

  for (item=win->items, mi = *menu_list; item; item=item->next)
  {
    if (!item->selected || item->count == 0)
      continue;

    mi->item  = item->identifier;
    mi->count = item->count;
    mi++;
  }

  return num;
}

char Sdlgl_message_menu(CHAR_P let, int how, const char *mesg)
{
  struct TextWindow *win;
  
  if (WIN_MESSAGE == WIN_ERR)
    return 0;

  win = text_wins[WIN_MESSAGE];
  assert(win);

  if (sdlgl_alt_prev)
    sdlgl_remove_scrollback();

  win->more_escaped = 0;

  /* "menu" without selection; use ordinary method */
  if (how != PICK_ONE || let == 0)
  {
    return genl_message_menu(let, how, mesg);
  }

  win->dismiss_more = let;
  win->more_ch = 0;

  Sdlgl_putstr(WIN_MESSAGE, 0, mesg);

  /* force the [MORE] if the message didn't wrap */
  if (win->fresh_lines > 0 && win->more_ch != let)
    sdlgl_more(win);
  
  win->dismiss_more = 0;

  if (win->more_ch == let || win->more_ch == '\033')
    return win->more_ch;
 
  return 0;
}

/*
 *   Generic yes/no function. 'def' is the default (returned by space or
 *   return; 'esc' returns 'q', or 'n', or the default, depending on
 *   what's in the string. The 'query' string is printed before the
 *   user is asked about the string.
 *   
 *   If resp is NULL, any single character is accepted and returned.
 *   If not-NULL, only characters in it are allowed (exceptions:  the
 *   quitchars are always allowed, and if it contains '#' then digits
 *   are allowed); if it includes an <esc>, anything beyond that won't
 *   be shown in the prompt to the user but will be acceptable as input.
 */
char Sdlgl_yn_function(const char *query, const char *resp, CHAR_P def)
{
  struct TextWindow *win;
  
  int ch = 'n';  /* value to keep compiler sweet */
  int allow_num;

  char prompt[BUFSZ]; 
  char respbuf[BUFSIZ];
  char *rb;
  
  if (WIN_MESSAGE == WIN_ERR)
    return 'n';

  win = text_wins[WIN_MESSAGE];
  assert(win);
  assert(win->base);

  if (sdlgl_alt_prev)
    sdlgl_remove_scrollback();

  if (win->fresh_lines > 0)
    sdlgl_more(win);

  Sdlgl_clear_nhwindow(WIN_MESSAGE);
  
  /* disable scrollback while we control the message win */
  win->scrollback_enable = 0;

  sdlgl_gotoxy(win, 0, 0);

  allow_num = (resp && strchr(resp, '#') != NULL);
  
  /* make up the prompt */
  if (resp)
  {
    strcpy(respbuf, resp);
    rb = strchr(respbuf, '\033');
    if (rb)
      *rb = 0;
    sprintf(prompt, "%s [%s] ", query, respbuf);
    if (def) 
      sprintf(prompt+strlen(prompt), "(%c) ", def);
  }
  else
    strcpy(prompt, query);

  sdlgl_puts(win, prompt);
  
  for (;;)
  {
    sdlgl_flush();
    ch = sdlgl_get_key(0);

    if (!resp)
      break;

    if (ch == C('p'))
    {
      Sdlgl_doprev_message();
      continue;
    }

    if (sdlgl_alt_prev)
      sdlgl_remove_scrollback();

    /* handle escape (yes, it's weird) */
    if (ch == '\033') 
    {
      ch = strchr(resp, 'q') ? 'q' : (strchr(resp, 'n') ? 'n' : def);
      break;
    }

    /* Note: quitchars[] is defined in the main nethack code */
    if (strchr(quitchars, ch))
    {
      ch = def;
      break;
    }

    if (allow_num && (ch == '#' || digit(ch)))
    {
      /* --- handle numbers --- */

      int n_len = 0;
      int value = 0;
      int nc;

      sdlgl_putc(win, '#'); n_len++;

      if (digit(ch))
      {
        sdlgl_putc(win, ch); n_len++;
        value = ch - '0';
      }

      for (;;)
      {
        sdlgl_flush();
        nc = sdlgl_get_key(0);

        if (nc == C('p'))
        {
          Sdlgl_doprev_message();
          continue;
        }

        if (sdlgl_alt_prev)
          sdlgl_remove_scrollback();

        if (digit(nc) && value <= 99999)
        {
          sdlgl_putc(win, nc); n_len++;
          value = value * 10 + (nc - '0');
          continue;
        }

        /* handle escape */
        if (nc == '\033' || (nc == '\b' && n_len <= 1))
        {
          value = -1;
          break;
        }
        
        /* backspace */
        if (nc == '\b')
        {
          assert(n_len > 1);
          sdlgl_puts(win, "\b \b"); n_len--;

          value /= 10;
          continue;
        }

        /* enter */
        if ((nc == '\n' || nc == '\r') && n_len > 1)
          break;

        /* beep ? */
      }

      if (value > 0)
      {
        yn_number = value;
        ch = '#';
        break;
      }
      else if (value == 0)
      {
        ch = 'n';
        break;
      }

      /* user escaped, clear number */
      for (; n_len > 0; n_len--)
        sdlgl_puts(win, "\b \b");

      continue;
      
      /* --- end of handle numbers --- */
    }

    if (strchr(resp, ch))
      break;

    /* beep ? */
  }

  Sdlgl_clear_nhwindow(WIN_MESSAGE);

  win->scrollback_enable = 1;

  return (char) ch;
}

#endif /* GL_GRAPHICS */
/*gl_menu.c*/
