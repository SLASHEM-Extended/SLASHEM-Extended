/* Copyright (C) 2002 Andrew Apted <ajapted@users.sourceforge.net> */
/* NetHack may be freely redistributed.  See license for details.  */

/*
 * SDL/GL window port for NetHack & Slash'EM.
 *
 * This file handles text (NHW_TEXT) windows.
 */

#include "hack.h"
#include "dlb.h"
#include "patchlevel.h"

#if defined(GL_GRAPHICS) || defined(SDL_GRAPHICS)

#define WINGL_INTERNAL
#include "winGL.h"

#include <string.h>
#include <ctype.h>


static struct TextItem *reverse_text_items(struct TextItem *items)
{
  struct TextItem *head = NULL;
  struct TextItem *next = NULL;

  for (; items; items = next)
  {
    next = items->next;

    items->next = head;
    head = items;
  }

  return head;
}

#define MAX_ALGN  44

static int find_aligns(unsigned short *aligns, char *line)
{
  int x;
  int count = 0;

  memset(aligns, 0, MAX_ALGN * sizeof(unsigned short));

  for (x=1; x < MAX_ALGN && line[x]; x++)
  {
    unsigned short num;

    if (! isspace(line[x-1]) || isspace(line[x]))
      continue;

    /* compute the `score': number of spaces before the position *and*
     * after something printable.
     */
    for (num=1; (x-num) >= 0 && isspace(line[x-num]); num++)
    { /* nothing in here */ }

    aligns[x] = (x-num < 0) ? 1 : num;
    count++;
  }

  return count;
}

static int merge_aligns(unsigned short *main, unsigned short *other)
{
  int x;
  int count = 0;
  
  for (x=0; x < MAX_ALGN; x++)
  {
    if (main[x] == 0)
      continue;

    if (other[x] == 0)
    {
      main[x] = 0;
      continue;
    }

    main[x] = max(main[x], other[x]);
    count++;
  }

  return count;
}

static int choose_best_align(unsigned short *aligns, int cut_off)
{
  int x;

  assert(cut_off <= MAX_ALGN);

  /* best alignment is the rightmost one.
   */
  for (x=cut_off-1; x >= 0; x--)
  {
    if (aligns[x] > 0)
      return x;
  }

  return 0;
}

#if 0  /* DEBUGGING */
static void show_aligns(unsigned short *aligns)
{
  int x;
  
  fprintf(stderr, "<");

  for (x=0; x < MAX_ALGN; x++)
    fprintf(stderr, "%c", aligns[x] ? '|' : ' ');
   
  fprintf(stderr, ">\n");
}
#endif

void sdlgl_insert_text_item(struct TextWindow *win,
    struct TextItem *pred, int align, int attr, const char *str)
{
  struct TextItem *item;

  item = (struct TextItem *) alloc(sizeof(struct TextItem));
  memset(item, 0, sizeof(struct TextItem));

  item->attr = attr;
  item->str = strdup(str ? str : "");
  item->align = align;

  /* link it in */
  if (pred)
  {
    item->next = pred->next;
    pred->next = item;
  }
  else
  {
    item->next = win->lines;
    win->lines = item;
  }
}

static int is_line_blank(const char *str)
{
  if (str)
    for (; *str; str++)
      if (! isspace(*str))
        return 0;

  return 1;
}

static int is_initial_blank(const char *str, int num)
{
  for (; *str && num > 0; str++, num--)
    if (! isspace(*str))
      return 0;

  return 1;
}

/* result is index of the first character that should be moved to the
 * next line.
 */
static int find_chopoff_point(const char *buf, int cur_align, int avail_w)
{
  int len = strlen(buf);
  int x, min_x;
  
  assert(cur_align < avail_w);
  assert(len > avail_w);

  min_x = cur_align + (avail_w - cur_align) * 3 / 5;

  for (x=avail_w - 1; x > min_x; x--)
    if (isspace(buf[x-1]) && ! isspace(buf[x]))
      return x;
 
  return avail_w - 1;
}

static void compute_text_bounds(struct TextWindow *win)
{
  struct TextItem *item;

  /* compute height */
  win->calc_w = win->calc_h = 0;
  
  for (item=win->lines; item; item=item->next)
  {
    int width = min(CO-1, strlen(item->str));

    if (width > win->calc_w)
      win->calc_w = width;

    win->calc_h++;
  }

  /* one char padding at bottom */
  win->calc_h += 1;

  /* one char padding at left */
  win->calc_w += 1;
}

#define MIN_SCAN_Y  4

static void do_scan_text(struct TextWindow *win, int avail_w)
{
  struct TextItem *item, *succ;

  int pos_x, y;
  int cut_off;

  unsigned short aligns[MAX_ALGN];
  unsigned short tmp_al[MAX_ALGN];

  /* after the `cut_off' point we don't look for vertically aligned
   * blocks.
   */
  cut_off = min(40, win->calc_w / 2);

  if (cut_off < 4)
    return;

  item = win->lines;

  while (item)
  {
    if (is_line_blank(item->str))
    { item=item->next; continue; }

    if (! find_aligns(aligns, item->str))
    { item=item->next; continue; }
    
    for (y=1,succ=item->next; y < MIN_SCAN_Y && succ; 
         y++,succ=succ->next)
    {
      if (is_line_blank(succ->str))
        break;

      if (! find_aligns(tmp_al, succ->str))
        break;
      
      if (! merge_aligns(aligns, tmp_al))
        break;
    }

    if (y != MIN_SCAN_Y)
    { item=item->next; continue; }

    /* OK, vertical alignment has been detected.  Set the `align'
     * field of all following lines that match.
     */
    pos_x = choose_best_align(aligns, cut_off);

    if (pos_x == 0)
    { item=item->next; continue; }
     
    assert(pos_x < sizeof(aligns));

    for (; y > 0; y--, item=item->next)
      item->align = pos_x;

    for (; item; item=item->next)
    {
      if (is_line_blank(item->str))
        break;
          
      if (! find_aligns(tmp_al, item->str))
        break;

      if (tmp_al[pos_x] == 0)
        break;

      item->align = pos_x;
    }
  }
}

static void do_reformat_text(struct TextWindow *win, int avail_w)
{
  struct TextItem *item, *Z, *succ;

  int reformatting;
  int cur_align, cur_attr;
  int len, pos_x;

  char buf[BUFSZ * 2];
  const char *J;

  reformatting = 0;

  cur_align = cur_attr = 0;

  /* grab the first line, then clear the list, rebuilding the list
   * along the way.  BTW, this is has the desirable side-effect of
   * reversing the list.
   */
  item = win->lines;
  win->lines = NULL;

#undef  ADD_IT
#define ADD_IT()  do  \
    { struct TextItem *succ = item->next;  \
      item->next = win->lines;  \
      win->lines = item;  \
      item = succ;  \
    } while(0)
 
#undef  TRAIL_IT
#define TRAIL_IT(buf)  do  \
    { int p;  \
      assert(buf);  \
      for (p=strlen(buf)-1; p >= cur_align; p--)  \
        if (! isspace(buf[p]))  \
          break;  \
      buf[p+1] = 0;  \
    } while(0)

  /* Loop Invariants:
   *
   *    when `reformatting' is false:
   *    
   *       +  item points at the current line to process.
   *       +  buf[], cur_align and cur_attr are unused.
   *      
   *    when `reformatting' is true:
   *
   *       +  item points at the current line to process, which we
   *          modify (split or join it).
   *       +  item->str != NULL && line not completely blank.
   *       +  no trailing space on line (ignoring any initial indent
   *          spaces).
   *       +  buf[] only used temporarily.
   *       +  cur_align and cur_attr are valid.
   */
  while (item)
  {
    if (! reformatting)
    {
      if (is_line_blank(item->str))
      {
        ADD_IT(); continue;
      }

      len = strlen(item->str);

      if (strlen(item->str) <= avail_w)
      {
        ADD_IT(); continue;
      }

      cur_align = item->align;
      cur_attr  = item->attr;
       
      reformatting = 1;
      continue;
    }

    assert(reformatting);

    assert(! is_line_blank(item->str));

    /* current line too long ? */
    len = strlen(item->str);

    if (len > avail_w)
    {
      pos_x = find_chopoff_point(item->str, cur_align, avail_w);
       
      assert(pos_x > cur_align);
      assert(pos_x < len-1);

      len = pos_x;
      
      /* build new line */

      while (item->str[pos_x] && isspace(item->str[pos_x]))
        pos_x++;

      if (cur_align > 0)
        memset(buf, ' ', cur_align);

      strcpy(buf + cur_align, item->str + pos_x);

      /* shorten the current line, and add to list */

      item->str[len] = 0;
      TRAIL_IT(item->str);
      ADD_IT();

      /* allocate new line */

      Z = (struct TextItem *) alloc(sizeof(struct TextItem));
      memset(Z, 0, sizeof(struct TextItem));

      Z->str   = strdup(buf);
      Z->attr  = cur_attr;
      Z->align = cur_align;

      Z->next = item;
      item = Z;
      
      continue;
    }

    /* is the next line part of the same block ? */

    if (! item->next ||
        item->next->align != cur_align || 
        item->next->attr  != cur_attr ||
        is_line_blank(item->next->str) || 
        ! is_initial_blank(item->next->str, cur_align))
    {
      ADD_IT();
      reformatting = 0;
      continue;
    }

    /* concatenate the following line */

    succ = item->next;

    assert(succ);
    assert(succ->str);

    item->next = succ->next;
 
    for (J=succ->str; *J && isspace(*J); J++)
    { /* nothing in here */ }

    strcpy(buf, item->str);
    strcat(buf, " ");
    strcat(buf, J);
      
    TRAIL_IT(buf);

    free(item->str);
    item->str = strdup(buf);

    free(succ);
  }
}

/*
 * attempts to reformat or shrink the text window.  Returns the
 * tileset to use as the font.
 */
static struct TileSet * reformat_text_window(
    struct TextWindow *win)
{
  struct TileSet *set;
  int avail_w;

  set = sdlgl_font_text;
  avail_w = sdlgl_width / set->tile_w;

  if (sdlgl_shrink_wrap)
  {
    while (win->calc_w > avail_w)
    {
      /* reached smallest ? */
      if (set == sdlgl_next_narrower_font(set->tile_h))
        break;

      set = sdlgl_next_narrower_font(set->tile_h);
      avail_w = sdlgl_width / set->tile_w;
    }
  }

  if (win->calc_w <= avail_w)
    return set;

  if (! sdlgl_reformat)
    return set;

  /* Step 1.  Figure out the alignment positions for each line.  We
   *          require at least four lines to have the same alignment
   *          pattern for it to "kick in".
   */
  do_scan_text(win, avail_w);

  /* reverse lines (which normally go up) to process them.  Better
   * results when this is done *after* the alignment scan.
   */
  win->lines = reverse_text_items(win->lines);

  /* Step 2.  Reformat any blocks where at least one of the lines is
   *          too long.
   */
  do_reformat_text(win, avail_w);
 
  compute_text_bounds(win);

  return set;
}

static void draw_text_items(struct TextWindow *win)
{
  struct TileWindow *base;
  struct TextItem *item;

  int x, y;
  int maxlen;

  tilecol_t tilecol;

  base = win->base;
  assert(base);
  assert(base->is_text);
 
  /* choose background color */
  win->base->background = TEXT_BACK_COL;

  for (item=win->lines,y=1; item; item=item->next,y++)
  {
    assert(y >= 0);

    tilecol = sdlgl_attr_to_tilecol(item->attr);

    x = 0;
    
    /* padding character */
    sdlgl_blank_area(base, x, y, 1, 1);
    x++;

    maxlen = win->calc_w - x;

    if (maxlen > 0)
      x += sdlgl_store_str(base, x, y, item->str, maxlen, tilecol);

    maxlen = win->calc_w - x;

    if (maxlen > 0)
      sdlgl_blank_area(base, x, y, maxlen, 1);
  }
}

void sdlgl_process_text_window(int window, struct TextWindow *win)
{
  struct TileSet *font_set;

  int bottom_y;
  int pixel_w, pixel_h;
  
  assert(! win->base);

  if (! win->lines)
    return;

  if (sdlgl_alt_prev)
    sdlgl_remove_scrollback();

  sdlgl_top_win = window;

  compute_text_bounds(win);
  
  font_set = reformat_text_window(win);
  
  win->base = sdlgl_new_tilewin(font_set, win->calc_w, 
      win->calc_h, 1,0);
 
  draw_text_items(win);

  /* here, strangely enough, is where we show the RIP image */
  if (win->is_rip)
    bottom_y = sdlgl_display_RIP(win->how_died) - 20;
  else
    bottom_y = sdlgl_height;

  pixel_w = min(sdlgl_width, win->base->scale_w * win->calc_w);
  pixel_h = min(bottom_y,    win->base->scale_h * win->calc_h);
   
  win->show_w = pixel_w / win->base->scale_w;
  win->show_h = pixel_h / win->base->scale_h;

  assert(0 < win->show_w && win->show_w <= win->calc_w); 
  assert(0 < win->show_h && win->show_h <= win->calc_h);

  sdlgl_map_tilewin(win->base, sdlgl_width - pixel_w,
      bottom_y - pixel_h, pixel_w, pixel_h, 10+window);

  /* set focus */
  win->focus_x = 0;
  win->focus_y = win->calc_h - win->show_h;
  
  /* this flushes: */
  sdlgl_pan_window(window, 0, 0);

  for (;;)
  {
    int ch = sdlgl_get_key(0);

    if (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\033' ||
        ch == 'q' || ch == 'n' || ch == 'y')
      break;
  }
 
  if (win->is_rip)
    sdlgl_dismiss_RIP();
  
  sdlgl_unmap_tilewin(win->base);
  sdlgl_free_tilewin(win->base);
  win->base = NULL;

  sdlgl_top_win = WIN_ERR;
}

void sdlgl_clear_text_items(struct TextWindow *win)
{
  struct TextItem *item;

  while (win->lines)
  {
    item = win->lines;
    win->lines = item->next;

    free(item->str);
    free(item);
  }

  assert(win->lines == NULL);
}

void Sdlgl_display_file(const char *fname, BOOLEAN_P complain)
{
  dlb *f;
  winid datawin;

  char buf[BUFSZ];
  char *cr;
#ifdef FILE_AREAS
  f = dlb_fopen_area(FILE_AREA_SHARE, fname, RDTMODE);
#else
  f = dlb_fopen(fname, RDTMODE);
#endif

  if (!f) 
  {
    if (complain) 
    {
      sprintf(buf, "Cannot open `%s' to display.", fname);
      Sdlgl_raw_print(buf);
    }

    return;
  }

  datawin = Sdlgl_create_nhwindow(NHW_TEXT);

  while (dlb_fgets(buf, BUFSZ, f) != NULL) 
  {
    /* NUL-terminate string */
    if ((cr = index(buf, '\n')) != 0) *cr = 0;
    if ((cr = index(buf, '\r')) != 0) *cr = 0;

    if (index(buf, '\t') != 0) 
      tabexpand(buf);

    Sdlgl_putstr(datawin, 0, buf);
  }

  Sdlgl_display_nhwindow(datawin, FALSE);
  Sdlgl_destroy_nhwindow(datawin);

  dlb_fclose(f);
}

#endif /* GL_GRAPHICS */
/*gl_text.c*/
