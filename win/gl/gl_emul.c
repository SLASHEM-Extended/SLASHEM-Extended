/* Copyright (C) 2002 Andrew Apted <ajapted@users.sourceforge.net> */
/* NetHack may be freely redistributed.  See license for details.  */

/*
 * SDL/GL window port for NetHack & Slash'EM.
 *
 * Simple terminal emulation.  Puts(), Putc(), Gotoxy(), etc etc.
 * Uses TileWindow for the output.  NOTE that the write_y member of
 * TextWindow goes from the bottom up, i.e. 0 is the lowest line.
 * Also handles line input stuff (including extended commands).
 */

#include "hack.h"

#if defined(GL_GRAPHICS) || defined(SDL_GRAPHICS)

#define WINGL_INTERNAL
#include "winGL.h"
#include "func_tab.h"

#include <string.h>
#include <ctype.h>


extern int NDECL(extcmd_via_menu);

#define HEIGHT_ASKNAME  1
#define DEPTH_ASKNAME   8

#define MAX_NAME_LEN     20
#define MAX_EXT_CMD_LEN  20


void sdlgl_emul_startup(void)
{
  /* does nothing */
}

void sdlgl_emul_shutdown(void)
{
  /* does nothing */
}

void Sdlgl_start_screen(void)
{
  /* does nothing (needed for interface) */
}

void Sdlgl_end_screen(void)
{
  /* does nothing (needed for interface) */
}

struct TextWindow *sdlgl_new_textwin(int type)
{
  struct TextWindow *win;

  win = (struct TextWindow *) alloc(sizeof(struct TextWindow));
  memset(win, 0, sizeof(struct TextWindow));

  win->type = type;
  win->is_menu = -1;
  win->base = NULL;
  win->write_x = win->write_y = 0;
  win->write_cursor = 0;
  win->write_col = win->fill_col = (BLACK<<8) + L_GREY;
  win->scrollback = NULL;

  return win;
}

void sdlgl_free_textwin(struct TextWindow *win)
{
  if (win->base)
  {
    sdlgl_free_tilewin(win->base);
    win->base = NULL;
  }

  free(win);
}


/* ---------------------------------------------------------------- */

static void copy_line_to_scrollback(struct TextWindow *win, int y)
{
  int x, tw;

  struct TileWindow *front = win->base;
  struct TileWindow *back  = win->scrollback;
  
  assert(front && back);

  tw = front->total_w;
  assert(back->total_w == tw);
  assert(0 <= y && y < front->total_h);

  /* ignore empty lines */
  
  for (x=0; x < tw; x++)
    if (front->tiles[y * tw + x].fg != TILE_EMPTY &&
        front->tiles[y * tw + x].fg != CHAR_2_TILE(' '))
      break;

  if (x >= tw)
    return;

  /* scroll up the scroll back */
  
  sdlgl_copy_area(back, 0, 0, tw, back->total_h - 1, 0, 1);
   
  sdlgl_transfer_area(front, 0, y, tw, 1, back, 0, 0);

  if (win->scrollback_size < back->total_h)
    win->scrollback_size += 1;
}

static void do_scroll_up(struct TextWindow *win)
{
  int tw = win->base->total_w;
  int th = win->base->total_h;

  if (win->scrollback && win->scrollback_enable)
    copy_line_to_scrollback(win, th - 1);
   
  /* copy lines upward */
  sdlgl_copy_area(win->base, 0, 0, tw, th - 1, 0, 1);
      
  /* blank out bottom line */
  sdlgl_blank_area(win->base, 0, 0, tw, 1);
}

static void do_wrapping(struct TextWindow *win)
{
  int tw = win->base->total_w;
  int th = win->base->total_h;

  if (win->write_x < 0)
  {
    if (win->write_y >= th - 1)
      win->write_x = 0;
    else
    {
      win->write_x = tw - 1;
      win->write_y += 1;
    }
  }
  
  if (win->write_y >= th)
    win->write_y = th - 1;

  if (win->write_x >= tw)
  {
    win->write_x = 0; 
    win->write_y -= 1;
  }

  for (; win->write_y < 0; win->write_y += 1)
  {
    do_scroll_up(win);
  }

  if (win->write_cursor)
    sdlgl_set_cursor(win->base, win->write_x, win->write_y, 1);
}

static void do_putc(struct TextWindow *win, char t)
{
  int x = win->write_x;
  int y = win->write_y;
  
  int tw = win->base->total_w;
  int th = win->base->total_h;

  assert(0 <= x && x < tw);
  assert(0 <= y && y < th);

  sdlgl_store_char(win->base, x, y, t, win->write_col);

  win->write_x += 1;
  do_wrapping(win);
}

void sdlgl_putc(struct TextWindow *win, int c)
{
  /* make sure character is in range */
  c = ((unsigned int) c) & 0xFF;
   
  assert(c >= 0);

  /* handle special cases */
  switch (c)
  {
    case 0:    /* NUL: do nothing */
      break;
      
    case C('g'):   /* ^G: bel */
    {
      Sdlgl_nhbell();
      break;
    }

    case C('h'):   /* ^H: backspace */
    {
      sdlgl_backsp(win);
      break;
    }

    case C('i'):   /* ^I: tab */
    {
      int num;
      for (num=8 - (win->write_x & 7); num > 0; num--)
        do_putc(win, ' ');
      break;
    }

    case C('j'):  /* ^J: LF */
    {
      win->write_x = 0;
      win->write_y -= 1;
      do_wrapping(win);
      break;
    }

    case C('l'):  /* ^L: clear */
    {
      int y;

      for (y=0; y < win->show_h + 1; y++)
        sdlgl_putc(win, '\n');

      sdlgl_home(win);
      break;
    }

    case C('m'):  /* ^M: CR */
    {
      win->write_x = 0; 
      break;
    }

    /* not special ?  Just display the tile normally */

    default:
      do_putc(win, c);
      break;
  }

  if (win->write_cursor && win->base)
    sdlgl_set_cursor(win->base, win->write_x, win->write_y, 1);
}

void sdlgl_puts(struct TextWindow *win, const char *s)
{
  for (; *s; s++)
    sdlgl_putc(win, (int) (unsigned char) *s);
}

void sdlgl_puts_nolf(struct TextWindow *win, const char *s)
{
  for (; *s; s++)
    if (*s != '\n' && *s != '\r')
      sdlgl_putc(win, (int) (unsigned char) *s);
}


/* ---------------------------------------------------------------- */

void sdlgl_gotoxy(struct TextWindow *win, int x, int y)
{
  int tw = win->base->total_w;
  int th = win->base->total_h;

  if (x < 0)   x = 0;
  if (x >= tw) x = tw - 1;

  if (y < 0)   y = 0;
  if (y >= th) y = th - 1;
  
  win->write_x = x;
  win->write_y = th - 1 - y;

  if (win->write_cursor)
    sdlgl_set_cursor(win->base, win->write_x, win->write_y, 1);
}

void sdlgl_home(struct TextWindow *win)
{
  sdlgl_gotoxy(win, 0, 0);
}

void sdlgl_backsp(struct TextWindow *win)
{
  win->write_x -= 1;
  do_wrapping(win);
}

/* Note: none of these clear routines affects the write position.
 */
void sdlgl_clear_end(struct TextWindow *win)
{
  int x = win->write_x;
  int y = win->write_y;

  int len = win->base->total_w - x;

  if (len > 0)
    sdlgl_blank_area(win->base, x, y, len, 1);
}

void sdlgl_clear_eos(struct TextWindow *win)
{
  int y;

  int bu_x = win->write_x;
  int bu_y = win->write_y;

  /* handle current line */
  sdlgl_clear_end(win);

  /* handle the remaining lines underneath */
  for (y=0; y < bu_y; y++)
  {
    win->write_x = 0;
    win->write_y = y;
    
    sdlgl_clear_end(win);
  }

  win->write_x = bu_x;
  win->write_y = bu_y;
}

void sdlgl_clear(struct TextWindow *win)
{
  int bu_x = win->write_x;
  int bu_y = win->write_y;

  sdlgl_home(win);    
  sdlgl_clear_eos(win);

  win->write_x = bu_x;
  win->write_y = bu_y;
}

void sdlgl_enable_cursor(struct TextWindow *win, int enable)
{
  assert(win->base);

  win->write_cursor = enable;
  win->base->curs_block = 1;
  win->base->curs_color = CURSOR_COL;

  if (enable)
  {
    sdlgl_set_cursor(win->base, win->write_x, win->write_y, 1);
  }
  else
  {
    sdlgl_set_cursor(win->base, -1, -1, 1);
  }
}


/* ---------------------------------------------------------------- */

static void do_getlin(struct TextWindow *win, const char *query, 
    char *bufp, int is_name)
{
  int max = BUFSZ-1;
  int len = 0;
  int ch;

  sdlgl_gotoxy(win, 0, 0);

  /* show the prompt */
  sdlgl_puts(win, query);
  sdlgl_putc(win, ' ');

  sdlgl_enable_cursor(win, 1);

  /* loop invariant: bufp[len] == 0 */
  bufp[0] = 0;
  
  for (;;)
  {
    sdlgl_flush();
    ch = sdlgl_get_key(POSKEY_ALLOW_REPEAT);

    if (ch == C('p'))
    {
      Sdlgl_doprev_message();
      continue;
    }

    if (sdlgl_alt_prev)
      sdlgl_remove_scrollback();

    if (ch == '\033')  /* escape key ? */
    {
      strcpy(bufp, "\033");
      break;
    }

    if (ch == '\b')  /* backspace ? */
    {
      if (len > 0)
      {
        bufp[--len] = 0;
        sdlgl_puts(win, "\b \b");
      }
      continue;
    }

    if (ch == '\n' || ch == '\r')  /* return ? */
    {
      if (! is_name || len > 0)
        break;
    }

    /* for name input, limit characters to letters + a few symbols.
     */
    if (is_name && !(letter(ch) || ch == '_' ||
        ch == '-' || ch == '@'))
      continue;
    else if (is_name && len >= MAX_NAME_LEN)
      continue;

    if (' ' <= ch && ch <= '~')
    {
      if (len < max)
      {
        bufp[len++] = ch;
        bufp[len] = 0;
        sdlgl_putc(win, ch);
      }
      continue;
    }

    /* key unacceptable.  beep ? */
  }

  /* clean up after ourselves */
  sdlgl_enable_cursor(win, 0);

  Sdlgl_clear_nhwindow(WIN_MESSAGE);
}

/*
 * Read a line closed with '\n' into the array char bufp[BUFSZ].
 * The '\n' is not stored. The string is closed with a '\0'.
 * Reading can be interrupted by an escape ('\033') - now the
 * resulting string is "\033".
 */
void Sdlgl_getlin(const char *query, char *bufp)
{
  struct TextWindow *win;

  if (WIN_MESSAGE == WIN_ERR)
  {
    strcpy(bufp, "\033");
    return;
  }

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
  
  do_getlin(win, query, bufp, 0);

  win->scrollback_enable = 1;
}

/* ---------------------------------------------------------------- */


#define VALID_EXT_CH(ch)  ('a' <= (ch) && (ch) <= 'z')

/* fills in `comp_tex' with an "[abcd]" completion string.  If there
 * was a single match, returns the extcmdlist index for it, otherwise
 * returns -1.
 */
static int make_completion_text(const char *word, int len,
    char *comp_tex)
{
  int i, pos, last = -1;
  int letters[26] = { 0, };
  int matches;

  /* special case for zero length: use stock string */
  if (len == 0)
  {
    strcpy(comp_tex, "[a-z?]");
    return -1;
  }
  
  /* default to empty string */
  comp_tex[0] = 0;
    
  for (i=matches=0; extcmdlist[i].ef_txt; i++)
  {
    const char *cmd = extcmdlist[i].ef_txt;

    if (! VALID_EXT_CH(cmd[0]))
      continue;

    if (strlen(cmd) <= len)
      continue;

    if (strncmp(word, cmd, len) != 0)
      continue;

    if (! VALID_EXT_CH(cmd[len]))
      continue;

    letters[cmd[len] - 'a'] = 1;
    matches++;
    last = i;
  }
  
  if (matches == 0)
    return -1;
  else if (matches == 1)
    return last;

  /* build match list string */
  pos = 0;
  
  comp_tex[pos++] = '[';

  for (i=0; i < 26; i++)
    if (letters[i])
      comp_tex[pos++] = 'a' + i;

  comp_tex[pos++] = ']';
  comp_tex[pos] = 0;

  return -1;
}

/* -AJA- I think it's cleaner to have a whole new routine rather than
 *       try to shoe-horn the extended command completion stuff into
 *       the existing do_getlin() routine.
 */
static int do_get_ext_cmd(struct TextWindow *win)
{
  char buffer[BUFSZ + 1 /* for NUL */];
  char comp_buf[BUFSZ];

  int len = 0;
  int i, ch;
  int use_menu = 0;

  sdlgl_enable_cursor(win, 1);
  win->write_col = L_GREY;

  /* loop invariant: buffer[len] == 0 */
  buffer[0] = 0;
  comp_buf[0] = 0;
  
  (void) make_completion_text(buffer, len, comp_buf);

  for (;;)
  {
    /* redraw the line, then flush */
    sdlgl_home(win);
    sdlgl_clear_end(win);

    sdlgl_putc(win, '#');
    sdlgl_putc(win, ' ');
    sdlgl_puts(win, buffer); 
    sdlgl_puts(win, comp_buf);

    sdlgl_flush();
    
    /* process a key */
    ch = sdlgl_get_key(POSKEY_ALLOW_REPEAT);

    if (ch == C('p'))
    {
      Sdlgl_doprev_message();
      continue;
    }

    if (sdlgl_alt_prev)
      sdlgl_remove_scrollback();

    if (ch == '\033')  /* escape key ? */
    {
      len = 0;
      break;
    }

    if (ch == '\n' || ch == '\r')  /* return ? */
        break;

    if (ch == '\b')  /* backspace ? */
    {
      if (len == 0)
        break;

      assert(len > 0);

      buffer[--len] = 0;

      (void) make_completion_text(buffer, len, comp_buf);
      continue;
    }

    if (ch == '?' && len == 0)
    {
      len = 0;
      use_menu = 1;
      break;
    }
    
    /* only lowercase letters please */
    if (! VALID_EXT_CH(ch) || len >= MAX_EXT_CMD_LEN)
    {
      /* beep ? */
      continue;
    }

    buffer[len++] = ch;
    buffer[len] = 0;

    /* do completion */
    i = make_completion_text(buffer, len, comp_buf);

    if (i >= 0)
    {
      strcpy(buffer, extcmdlist[i].ef_txt);
      len = strlen(buffer);
    }
  }

  sdlgl_enable_cursor(win, 0);
  sdlgl_clear(win);
  sdlgl_home(win);

  if (use_menu)
    return extcmd_via_menu();

  if (len == 0)
    return -1;

  /* find command, show message if not found */
  for (i=0; extcmdlist[i].ef_txt; i++)
    if (strcmp(extcmdlist[i].ef_txt, buffer) == 0)
      return i;

  pline("%s: unknown extended command.", buffer);
  return -1;
}

/*
 * Read in an extended command, doing command line completion.  We
 * stop when we have found enough characters to make a unique command.
 */
int Sdlgl_get_ext_cmd(void)
{
  struct TextWindow *win;

  int cmd;

#if 0
  if (iflags.extmenu)
    return extcmd_via_menu();
#endif
   
  if (WIN_MESSAGE == WIN_ERR)
    return -1;

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
  
  cmd = do_get_ext_cmd(win);

  win->scrollback_enable = 1;

  return cmd;
}

/*
 * plname is filled either by an option (-u Player  or  -uPlayer) or
 * explicitly (by being the wizard) or by askname.
 * It may still contain a suffix denoting the role, etc.
 * Always called after init_nhwindows() and before display_gamewindows().
 */
void Sdlgl_askname(void)
{
  struct TextWindow *win;
  int pixel_h;

  char buffer[BUFSZ];

  /* create text & tile windows */
  win = sdlgl_new_textwin(NHW_TEXT);  /* type unimportant */
   
  win->show_w = sdlgl_width / sdlgl_font_message->tile_w;
  win->show_h = HEIGHT_ASKNAME;

  pixel_h = sdlgl_font_message->tile_h * win->show_h;
  
  win->base = sdlgl_new_tilewin(sdlgl_font_message, win->show_w, 
      win->show_h, 1,0);
        
  sdlgl_map_tilewin(win->base, 0, sdlgl_height - pixel_h,
      sdlgl_width, pixel_h, DEPTH_ASKNAME);

  /* do the query */
  do_getlin(win, "Who are you?", buffer, 1);

  sdlgl_unmap_tilewin(win->base);
  sdlgl_free_textwin(win);

  if (buffer[0] == '\033')
    sdlgl_error("Quit from who-are-you prompt.\n");

  strncpy(plname, buffer, sizeof(plname) - 1);
  plname[sizeof(plname) - 1] = 0;
}


#endif /* GL_GRAPHICS */
/*gl_emul.c*/
