/* Copyright (C) 2002 Andrew Apted <ajapted@users.sourceforge.net> */
/* NetHack may be freely redistributed.  See license for details.  */

/*
 * SDL/GL window port for NetHack & Slash'EM.
 *
 * The tile engine.  Loads tiles from image files into "TileSets".
 * Does allocation/freeing and rendering of "TileWindows".
 */

#include "hack.h"
#include "patchlevel.h"

#if defined(GL_GRAPHICS) || defined(SDL_GRAPHICS)

#define WINGL_INTERNAL
#include "winGL.h"

extern const char *killed_by_prefix[];


struct TileSet *sdlgl_tiles = NULL;

static struct TileWindow *mapped_tilewins[MAXWIN];
static int mapped_num = 0;
static int start_depth = 0;

/* fading support.  amount is 0 for none */
static float fade_amount = 0;

/* logo support */
static struct TileSet *logo_set = NULL;
static int logo_w, logo_h;

static struct TileWindow *logo_win = NULL;
static struct TileWindow *logo_copyright = NULL;

static enum
{
  LOGST_NOT_BEGUN,
  LOGST_SHOW,
  LOGST_FADE_OUT,
  LOGST_DONE
} 
logo_state = LOGST_NOT_BEGUN;

static int logo_progress, logo_target, logo_epoch;

static struct TileSet *rip_set = NULL;
static int rip_w, rip_h;

static struct TileWindow *rip_win = NULL;
static struct TileWindow *rip_name = NULL;
static struct TileWindow *rip_info = NULL;


void sdlgl_tile_startup(void)
{
  /* nothing to do */
}

void sdlgl_tile_shutdown(void)
{
  if (sdlgl_tiles)
  {
    sdlgl_free_tileset(sdlgl_tiles);
    sdlgl_tiles = NULL;
  }
  
  if (rip_set)
  {
    sdlgl_free_tileset(rip_set);
    rip_set = NULL;
  }
  
  if (logo_set)
  {
    sdlgl_free_tileset(logo_set);
    logo_set = NULL;
  }

  sdlgl_free_extra_shapes();
}
  
/* ---------------------------------------------------------------- */


void sdlgl_add_extrashape(struct TileWindow *win,
    enum ShapeType type, short x, short y, int par1, int par2)
{
  struct ExtraShape *shape;
  
  if (win->extra_num >= win->extra_max)
    return;

  shape = win->extra_shapes + win->extra_num;
  win->extra_num++;

  shape->type = type;
  shape->x = x;  shape->y = y;
  shape->par1 = par1;  shape->par2 = par2;
}

void sdlgl_remove_extrashapes(struct TileWindow *win, short x, short y)
{
  int i, j;

  for (i=j=0; i < win->extra_num; i++)
  {
    struct ExtraShape *shape = win->extra_shapes + i;

    assert(shape->type != SHAPE_None);

    if (shape->x == x && shape->y == y)
      continue;

    if (i != j)
      memmove(win->extra_shapes + j, shape, sizeof(struct ExtraShape));

    j++;
  }

  win->extra_num = j;
  assert(win->extra_num >= 0);
}

/* ---------------------------------------------------------------- */


static void do_load_tileset(void)
{
  /* map tiles are loaded while logo is displayed (unless the splash
   * screen has been disabled), and RIP image is loaded after logo is
   * removed and freed.
   */
  if (iflags.wc_tile_height == 16)
    sdlgl_tiles = sdlgl_load_tileset("gltile16.png", 16,16, 0,0, NULL,NULL);
  else if (iflags.wc_tile_height == 64)
    sdlgl_tiles = sdlgl_load_tileset("gltile64.png", 48,64, 0,0, NULL,NULL);
  else
    sdlgl_tiles = sdlgl_load_tileset("gltile32.png", 32,32, 0,0, NULL,NULL);

  assert(sdlgl_tiles);
}

void sdlgl_start_logo(void)
{
  const char *pos;
  const char *copyright_str = COPYRIGHT_BANNER_A "\n" 
      COPYRIGHT_BANNER_B "\n" COPYRIGHT_BANNER_C;

  int x, y;
  int w1, h1, w2, h2;
    
  logo_set = sdlgl_load_tileset("gllogo.png", 16,16, 0,0, &logo_w, &logo_h);

  assert(logo_set);

  /* fade color remains same over life of logo */
  fade_amount = 0;

  assert(logo_state == LOGST_NOT_BEGUN);

  logo_state = LOGST_SHOW;
  logo_progress = 0;
  logo_target = 2000;
  logo_epoch = sdlgl_get_time();

  /* build logo window.  -AJA- Using tileset/tilewindow is not
   * strictly necessary, it would be easy enough to have a single
   * image and draw it directly.  The advantage is supporting logos
   * larger than 256x256 (a common texture limitation of 3D cards),
   * and slightly simpler code overall.
   */
  logo_win = sdlgl_new_tilewin(logo_set, logo_w, logo_h, 0,0);

  for (y=0; y < logo_h; y++)
  for (x=0; x < logo_w; x++)
  {
    /* 1-to-1 mapping */
    logo_win->tiles[(logo_h-1 - y) * logo_w + x].fg = y * logo_w + x;
  }

  /* build copyright info window */
  logo_copyright = sdlgl_new_tilewin(sdlgl_font_message, 60, 10, 1,0);

  x = 0; y = logo_copyright->total_h - 1;

  for (pos=copyright_str; *pos; pos++)
  {
    if (*pos == '\n')
    {
      x = 0; y--;

      if (y < 0)
        break;

      continue;
    }

    if (x >= logo_copyright->total_w)
      continue;

    logo_copyright->tiles[y * logo_copyright->total_w + x].fg =
        CHAR_2_TILE(*pos);
    x++;
  }

  /* map windows onto screen */
  w1 = logo_win->total_w * logo_win->set->tile_w;
  h1 = logo_win->total_h * logo_win->set->tile_h;

  sdlgl_map_tilewin(logo_win, 10, sdlgl_height - h1 - 12, w1, h1, 1);

  w2 = logo_copyright->total_w * logo_copyright->set->tile_w;
  h2 = logo_copyright->total_h * logo_copyright->set->tile_h;

  sdlgl_map_tilewin(logo_copyright, 20, sdlgl_height - h1 - 40 - h2, 
      w2, h2, 2);

  sdlgl_start_fading(sdlgl_width, logo_copyright->scr_y);
  sdlgl_mark_dirty(0, 0, sdlgl_width, sdlgl_height, 0);

  sdlgl_flush();

  /* load in the tileset now, whilst the logo is fully up.   They are
   * fairly large, and take some time to load.
   */
  do_load_tileset();
}

/* returns 1 when finished */
int sdlgl_iterate_logo(void)
{
  int last_progress;

  assert (LOGST_SHOW <= logo_state && logo_state <= LOGST_FADE_OUT);

  assert(logo_set);
  assert(logo_win);
  assert(logo_copyright);

  /* compute fading amount */
  if (logo_state == LOGST_FADE_OUT)
  {
    assert(logo_target > 0);
    assert(0 <= logo_progress && logo_progress <= logo_target);

    fade_amount = logo_progress / (float)logo_target;
  }

  /* draw it and wait */
  sdlgl_mark_dirty(0, logo_copyright->scr_y, 
      sdlgl_width, sdlgl_height - logo_copyright->scr_y, 0);
  sdlgl_flush();

  last_progress = logo_progress;
  logo_progress = (sdlgl_get_time() - logo_epoch);
  assert(logo_progress >= 0);
  
  /* when fading out, limit the jump to a bit over 1/3 */
  if (logo_state == LOGST_FADE_OUT && 
      (logo_progress - last_progress) > 350)
  {
    logo_progress = last_progress + 350;
  }
  
  if (logo_progress < logo_target)
    return 0;

  /* current phase is over -- choose new one */
  logo_progress = 0;

  if (logo_state == LOGST_SHOW)
  {
    logo_state  = LOGST_FADE_OUT;
    logo_target = 1000;
    logo_epoch  = sdlgl_get_time();

    return 0;
  }

  /* all done.  Clean up the mess */
  logo_state  = LOGST_DONE;
  fade_amount = 0;
  
  sdlgl_unmap_tilewin(logo_win);
  sdlgl_free_tilewin(logo_win);
  logo_win = NULL;

  sdlgl_unmap_tilewin(logo_copyright);
  sdlgl_free_tilewin(logo_copyright);
  logo_copyright = NULL;

  sdlgl_free_tileset(logo_set);
  logo_set = NULL;

  sdlgl_finish_fading();
  sdlgl_flush();

  return 1;
}

void sdlgl_tile_load_rest(void)
{
  if (! sdlgl_tiles)
  {
    do_load_tileset();
  }

  rip_set = sdlgl_load_tileset("glrip.png", 30,30, 0,1, &rip_w, &rip_h);

  assert(rip_set);

  sdlgl_create_extra_graphics();

  /* FIXME: create facing files for Slash'EM.
   */
#ifdef VANILLA_GLHACK
  sdlgl_load_face_dirs("glface16.lst", tile_16_face_dirs);
  sdlgl_load_face_dirs("glface32.lst", tile_32_face_dirs);
#endif
}

/* ---------------------------------------------------------------- */


#define STONE_MID_X   (155 + 155/2)
#define STONE_NAME_Y  (92 + 92/2)
#define STONE_NAME_W  (80 + 80/2)
#define STONE_NAME_H  (40 + 40/2)

#define STONE_TEXT_Y  (48 + 48/2)
#define STONE_INFO_LINES  5

#define STONE_MAX_NAME   11
#define STONE_MAX_INFO   16

static void add_rip_info(const char *str, int len, int y)
{
  int shift_x;

  len = min(STONE_MAX_INFO, len);
  shift_x = (STONE_MAX_INFO - len) / 2;
  
  if (len == 0)
    return;

  assert(len > 0);
  assert(shift_x >= 0);
  assert(shift_x + len <= STONE_MAX_INFO);

  sdlgl_store_str(rip_info, shift_x, y, str, len, BLACK);
}

static void add_rip_killer(const char *str, int y)
{
  struct TempLine 
  { 
    char s[STONE_MAX_INFO + 1 /* NUL */];
  }
  bufs[STONE_INFO_LINES];  /* NB: top down */

  int len = strlen(str);
  int i, line;

  if (len == 0)
    return;

  /* FUDGE IT */

  for (line=0; line < STONE_INFO_LINES; line++)
  {
    /* determine how much of the string to use for this line (possibly
     * all of it, if the whole thing fits).
     */
    len = strlen(str);

    if (len == 0)
      break;

    if (len > STONE_MAX_INFO)
    {
      for (len=STONE_MAX_INFO; len > STONE_MAX_INFO/2; len--)
        if (str[len] == ' ')
          break;

      if (str[len] != ' ')
        len = STONE_MAX_INFO;
    }

    memcpy(bufs[line].s, str, len);
    bufs[line].s[len] = 0;
    
    str += len;

    while (*str == ' ')
      str++;
  }

  assert(line > 0 && line <= STONE_INFO_LINES);

  if ('a' <= bufs[0].s[0] && bufs[0].s[0] <= 'z') 
    bufs[0].s[0] += 'A' - 'a';

  /* center the lines vertically */
  y += (STONE_INFO_LINES - line + 1) / 2;
  
  for (i=0; i < line; i++)
    add_rip_info(bufs[i].s, strlen(bufs[i].s), y + line - 1 - i);
}

/* returns bottom y */
int sdlgl_display_RIP(int how)
{
  int x, y;
  int w, h;
  int base_y;

  char name_buf[BUFSZ];
  char info_buf[BUFSZ];

  assert(! rip_win);
  assert(rip_set);

  /* create tombstone window */

  rip_win = sdlgl_new_tilewin(rip_set, rip_w, rip_h, 0,0);

  for (y=0; y < rip_h; y++)
  for (x=0; x < rip_w; x++)
  {
    /* 1-to-1 mapping */
    rip_win->tiles[(rip_h-1 - y) * rip_w + x].fg = y * rip_w + x;
  }

  w = rip_win->total_w * rip_win->scale_w;
  h = rip_win->total_h * rip_win->scale_h;

  base_y = sdlgl_height - h - 10;

  sdlgl_map_tilewin(rip_win, (sdlgl_width - w) / 2, base_y, w, h, 30);

  /* now create name & role windows */

  strcpy(name_buf, plname);
  name_buf[STONE_MAX_NAME] = 0;

  if ('a' <= name_buf[0] && name_buf[0] <= 'z') 
    name_buf[0] += 'A' - 'a';

  rip_name = sdlgl_new_tilewin(sdlgl_font_message, strlen(name_buf), 1, 1,0);
  rip_info = sdlgl_new_tilewin(sdlgl_font_8, STONE_MAX_INFO, 
      2 + STONE_INFO_LINES, 1,0);

  rip_name->see_through = 1;
  rip_info->see_through = 1;

  /* scale name to fit nicely on tombstone */
  if (! sdlgl_software)
  {
    rip_name->scale_w = STONE_NAME_W / rip_name->total_w;
    rip_name->scale_h = STONE_NAME_H / rip_name->total_h;

    if (rip_name->scale_w > sdlgl_font_message->tile_w * 4 / 2)
      rip_name->scale_w = sdlgl_font_message->tile_w * 4 / 2;
    if (rip_name->scale_h > sdlgl_font_message->tile_h * 4 / 2)
      rip_name->scale_h = sdlgl_font_message->tile_h * 4 / 2;
     
    if (rip_name->scale_w > rip_name->scale_h * 2)
      rip_name->scale_w = rip_name->scale_h * 2;
    else if (rip_name->scale_h > rip_name->scale_w * 4)
      rip_name->scale_h = rip_name->scale_w * 4;

    rip_name->scale_full_w = rip_name->scale_w;
    rip_name->scale_full_h = rip_name->scale_h;
  }

  sdlgl_store_str(rip_name, 0, 0, name_buf, rip_name->total_w, BLACK);
      
  /* Put $ on stone */
  Sprintf(info_buf, "%ld Au", u.ugold);
  add_rip_info(info_buf, strlen(info_buf), 1 + STONE_INFO_LINES);

  /* Put death type on stone */
  switch (killer_format) 
  {
    case KILLED_BY_AN:
      Strcpy(info_buf, killed_by_prefix[how]);
      Strcat(info_buf, an(killer));
      break;
    
    case KILLED_BY:
      Strcpy(info_buf, killed_by_prefix[how]);
      Strcat(info_buf, killer);
      break;
    
    case NO_KILLER_PREFIX:
      Strcpy(info_buf, killer);
      break;
      
    default:
      Strcpy(info_buf, "Killed by a bad switch");
      break;
  }

  add_rip_killer(info_buf, 1);

  /* Put year on stone */
  Sprintf(info_buf, "%4d", getyear());
  add_rip_info(info_buf, strlen(info_buf), 0);

  /* map these bits of text onto the screen, baby */

  w = rip_name->total_w * rip_name->scale_w;
  h = rip_name->total_h * rip_name->scale_h;

  sdlgl_map_tilewin(rip_name, rip_win->scr_x + STONE_MID_X - w/2,
      rip_win->scr_y + STONE_NAME_Y + STONE_NAME_H/2 - h/2, w, h, 32);

  w = rip_info->total_w * rip_info->scale_w;
  h = rip_info->total_h * rip_info->scale_h;

  sdlgl_map_tilewin(rip_info, rip_win->scr_x + STONE_MID_X - w/2,
      rip_win->scr_y + STONE_TEXT_Y, w, h, 31);

  return base_y;
}

void sdlgl_dismiss_RIP(void)
{
  assert(rip_win);
  assert(rip_name);
  assert(rip_info);

  sdlgl_unmap_tilewin(rip_win);
  sdlgl_free_tilewin(rip_win);
  rip_win = NULL;
  
  sdlgl_unmap_tilewin(rip_name);
  sdlgl_free_tilewin(rip_name);
  rip_name = NULL;

  sdlgl_unmap_tilewin(rip_info);
  sdlgl_free_tilewin(rip_info);
  rip_info = NULL;
}

/* ---------------------------------------------------------------- */


struct TileWindow *sdlgl_new_tilewin(struct TileSet *set, 
    int total_w, int total_h, int is_text, int is_map)
{
  int total = total_w * total_h;
  struct TileWindow *win;

  assert(total_w > 0 && total_h > 0);

  win = (struct TileWindow *) alloc(sizeof(struct TileWindow));
  memset(win, 0, sizeof(struct TileWindow));

  win->set = set;
  win->total_w = total_w;
  win->total_h = total_h;
  win->is_text = is_text;
  win->is_map  = is_map;
  win->see_through = 0;
  win->mapped_idx = -1;

  /* set defaults */
  win->scale_full_w = set->tile_w;
  win->scale_full_h = set->tile_h;
  win->scale_w = set->tile_w - set->lap_w;
  win->scale_h = set->tile_h - set->lap_h;
  win->scale_skew = set->lap_skew;
  win->pan_x = win->pan_y = 0;
  win->background = RGB_MAKE(0, 0, 0);
  win->curs_x = win->curs_y = -1;
  win->curs_w = 1;
  win->curs_block = 0;
  win->curs_color = OUTLINE_COL;
  win->has_border = 0;
   
  /* setup tile arrays */
  win->tiles  = (struct TilePair *) alloc(total * sizeof(struct TilePair));
 
  sdlgl_blank_area(win, 0, 0, total_w, total_h);

  /* setup extra_shape array */
  win->extra_max = is_map ? 256 : 10;
  win->extra_num = 0;
  
  win->extra_shapes = (struct ExtraShape *) alloc(win->extra_max *
      sizeof(struct ExtraShape));

  return win;
}

void sdlgl_free_tilewin(struct TileWindow *win)
{
  if (win->scr_depth > 0)
    sdlgl_unmap_tilewin(win);

  if (win->tiles)
  {
    free(win->tiles);
    win->tiles = NULL;
  }

  if (win->extra_shapes)
  {
    free(win->extra_shapes);
    win->extra_shapes = NULL;
  }

  free(win);
}

void sdlgl_set_start_depth(int depth)
{
  start_depth = depth;
}

void sdlgl_change_tileset(struct TileWindow *win, struct TileSet *set,
    int is_text)
{
  int i;

  /* handle text <-> nontext changes */

  if (is_text && !win->is_text)
  {
    for (i=0; i < (win->total_w * win->total_h); i++)
      win->tiles[i].u.col = L_GREY;
  }

  win->set = set;
  win->is_text = is_text;

  /* scale, pan & pad values are updated elsewhere (namely in
   * sdlgl_zoom_map), which includes changing the tile values for the
   * new tile set.
   */

  sdlgl_mark_dirty(win->scr_x, win->scr_y, win->scr_w, win->scr_h,
      win->scr_depth);
}

static int tilewin_compare(const void *p1, const void *p2)
{
  const struct TileWindow *A = *((const struct TileWindow **) p1);
  const struct TileWindow *B = *((const struct TileWindow **) p2);

  return (A->scr_depth - B->scr_depth);
}

void sdlgl_map_tilewin(struct TileWindow *win, int x, int y, 
    int w, int h, int depth)
{
  int i;

  assert(w > 0 && h > 0);
  assert(depth > 0);
  assert(win->scr_depth == 0);
  assert(mapped_num < MAXWIN);

  win->scr_x = x;
  win->scr_y = y;
  win->scr_w = w;
  win->scr_h = h;
  win->scr_depth = depth;

  mapped_tilewins[mapped_num++] = win;

  if (mapped_num > 1)
    qsort(mapped_tilewins, mapped_num, sizeof(void *), tilewin_compare);

  /* set private index values */
  for (i=0; i < mapped_num; i++)
    mapped_tilewins[i]->mapped_idx = i;

  sdlgl_mark_dirty(win->scr_x, win->scr_y, win->scr_w, win->scr_h,
      win->scr_depth);
}

void sdlgl_unmap_tilewin(struct TileWindow *win)
{
  int i;
  
  assert(win->scr_depth > 0);
  assert(win->mapped_idx >= 0);

  sdlgl_mark_dirty(win->scr_x, win->scr_y, win->scr_w, win->scr_h, 0);

  win->scr_depth  = 0;
  win->mapped_idx = -1;

  for (i=0; i < mapped_num; i++)
  {
    if (mapped_tilewins[i] == win)
    {
      mapped_tilewins[i] = NULL;
      break;
    }
  }

  /* it better be there in the list ! */
  assert(i != mapped_num);

  mapped_num--;
  
  /* if there's a gap, close it by shifting stuff down */
  for (; i < mapped_num; i++)
    mapped_tilewins[i] = mapped_tilewins[i+1];

  /* this isn't strictly necessary -- however if we were using garbage
   * collection, then it would be (prevent spurious references).
   */
  mapped_tilewins[mapped_num] = NULL;

  /* update private index values */
  for (i=0; i < mapped_num; i++)
    mapped_tilewins[i]->mapped_idx = i;
}

/* ---------------------------------------------------------------- */

static GH_INLINE void mark_dirty_tiles(struct TileWindow *win,
    int x, int y, int w, int h)
{
  int x2, y2;

  x = win->scr_x - win->pan_x + x * win->scale_w + y * win->scale_skew;
  y = win->scr_y - win->pan_y + y * win->scale_h;

  x2 = x + w * win->scale_full_w;
  y2 = y + h * win->scale_full_h;

  /* clip to window coords */

  x = max(x, win->scr_x);
  y = max(y, win->scr_y);

  x2 = min(x2, win->scr_x + win->scr_w);
  y2 = min(y2, win->scr_y + win->scr_h);

  if (x2 <= x || y2 <= y)
    return;
   
  sdlgl_mark_dirty(x, y, x2 - x, y2 - y, win->scr_depth);
}

void sdlgl_store_char(struct TileWindow *win, int x, int y,
      char ch, rgbcol_t col)
{
  int offset = y * win->total_w + x;

  assert(0 <= x && x < win->total_w);
  assert(0 <= y && y < win->total_h);
  
  win->tiles[offset].fg = CHAR_2_TILE(ch);
  win->tiles[offset].mg = TILE_EMPTY;
  win->tiles[offset].u.col = col;
  win->tiles[offset].flags = 0;

  mark_dirty_tiles(win, x, y, 1, 1);
}

/*
 * returns the number of characters stored.
 */
int sdlgl_store_str(struct TileWindow *win, int x, int y,
      const char *str, int maxlen, rgbcol_t col)
{
  int offset = y * win->total_w + x;
  int width;

  assert(0 <= x && x < win->total_w);
  assert(0 <= y && y < win->total_h);
  
  /* NOTE: assumes string has been clipped to window bounds */
  for (; *str && maxlen > 0; str++, maxlen--, offset++)
  {
    win->tiles[offset].fg = CHAR_2_TILE(*str);
    win->tiles[offset].mg = TILE_EMPTY;
    win->tiles[offset].u.col = col;
    win->tiles[offset].flags = 0;
  }

  width = offset - (y * win->total_w + x);

  mark_dirty_tiles(win, x, y, width, 1);

  return width;
}

static GH_INLINE int is_tile_solid(struct TileSet *set, tileidx_t idx)
{
  if (! set->has_alpha)
    return 1;

  if (idx == TILE_EMPTY)
    return 0;

  return set->has_alpha[idx] ? 0 : 1;
}

void sdlgl_store_tile(struct TileWindow *win, int x, int y,
      tileidx_t fg, tileidx_t mg, tileidx_t bg, tileflags_t flags)
{
  int offset = y * win->total_w + x;

  /* Optimisation: don't draw anything underneath tiles that are
   * completely solid.
   */
  if (is_tile_solid(win->set, fg))
    mg = bg = TILE_EMPTY;
  else if (is_tile_solid(win->set, mg))
    bg = TILE_EMPTY;

  win->tiles[offset].fg    = fg;
  win->tiles[offset].mg    = mg;
  win->tiles[offset].u.bg  = bg;
  win->tiles[offset].flags = flags;

  mark_dirty_tiles(win, x, y, 1, 1);
}

void sdlgl_blank_area(struct TileWindow *win, int x, int y, int w, int h)
{
  assert(w > 0 && h > 0);
  assert(0 <= x && x+w <= win->total_w);
  assert(0 <= y && y+h <= win->total_h);
  
  mark_dirty_tiles(win, x, y, w, h);

  for (; h > 0; h--, y++)
  {
    int offset = y * win->total_w + x;
    int len;

    if (win->is_text)
    {
      for (len=w; len > 0; len--, offset++)
      {
        win->tiles[offset].fg = TILE_EMPTY;
        win->tiles[offset].mg = TILE_EMPTY;
        win->tiles[offset].u.col = L_GREY;
        win->tiles[offset].flags = 0;
      }
    }
    else
    {
      for (len=w; len > 0; len--, offset++)
      {
        win->tiles[offset].fg    = TILE_EMPTY;
        win->tiles[offset].mg    = TILE_EMPTY;
        win->tiles[offset].u.bg  = TILE_EMPTY;
        win->tiles[offset].flags = 0;
      }
    }
  }
}

void sdlgl_copy_area(struct TileWindow *win, int x, int y, 
    int w, int h, int x2, int y2)
{
  int dx = x - x2;
  int dy = y - y2;

  assert(w > 0 && h > 0);
  assert(0 <= x && x+w <= win->total_w);
  assert(0 <= y && y+h <= win->total_h);
  assert(0 <= x2 && x2+w <= win->total_w);
  assert(0 <= y2 && y2+h <= win->total_h);

  mark_dirty_tiles(win, x2, y2, w, h);

  /* diagonal moves not supported (yet). 
   */
  assert(dx == 0 || dy == 0);

  if (dx == 0 && y2 < y)  /* downwards */
  {
    for (; h > 0; h--, y++)
    {
      struct TilePair *src = win->tiles + (y  * win->total_w + x);
      struct TilePair *dst = win->tiles + (y2 * win->total_w + x2);

      memmove(dst, src, sizeof(struct TilePair) * w);
    }
  }
  else if (dx == 0 && y2 > y)  /* upwards */
  {
    y  += h - 1;
    y2 += h - 1;
    
    for (; h > 0; h--, y--, y2--)
    {
      struct TilePair *src = win->tiles + (y  * win->total_w + x);
      struct TilePair *dst = win->tiles + (y2 * win->total_w + x2);

      memmove(dst, src, sizeof(struct TilePair) * w);
    }
  }
  else if (dy == 0)  /* left or right */
  {
    for (; h > 0; h--, y++)
    {
      struct TilePair *src = win->tiles + (y  * win->total_w + x);
      struct TilePair *dst = win->tiles + (y2 * win->total_w + x2);

      memmove(dst, src, sizeof(struct TilePair) * w);
    }
  }
}

/* transfer tiles between two _different_ tile windows.
 */
void sdlgl_transfer_area(struct TileWindow *swin, int sx, int sy, 
      int w, int h, struct TileWindow *dwin, int dx, int dy)
{
  assert(w > 0 && h > 0);
  assert(0 <= sx && sx+w <= swin->total_w);
  assert(0 <= sy && sy+h <= swin->total_h);
  assert(0 <= dx && dx+w <= dwin->total_w);
  assert(0 <= dy && dy+h <= dwin->total_h);

  mark_dirty_tiles(dwin, dx, dy, w, h);

  for (; h > 0; h--, sy++, dy++)
  {
    struct TilePair *src = swin->tiles + (sy * swin->total_w + sx);
    struct TilePair *dst = dwin->tiles + (dy * dwin->total_w + dx);

    memcpy(dst, src, sizeof(struct TilePair) * w);
  }
}
 
/* transfer a line of tiles between a tile window and a buffer.  If
 * `retrieve' is 1, the buffer is being filled, otherwise the buffer
 * is being transferred to the tile window.
 */
void sdlgl_transfer_line(struct TileWindow *win, int x, int y, 
      int w, struct TilePair *buffer, int retrieve)
{
  struct TilePair *win_pos;

  assert(w > 0);
  assert(0 <= x && x+w <= win->total_w);
  assert(0 <= y && y < win->total_h);
  
  win_pos = win->tiles + (y * win->total_w) + x;

  if (retrieve)
    memcpy(buffer, win_pos, sizeof(struct TilePair) * w);
  else
    memcpy(win_pos, buffer, sizeof(struct TilePair) * w);

  if (! retrieve)
  {
    mark_dirty_tiles(win, x, y, w, 1);
  }
}
 
void sdlgl_set_scale(struct TileWindow *win, int h)
{
  int th = win->set->tile_h;

  int lw = win->set->lap_w * h / th;
  int lh = win->set->lap_h * h / th;

  win->scale_full_w = win->set->tile_w * h / th;
  win->scale_full_h = h;

  win->scale_w = win->scale_full_w - lw;
  win->scale_h = win->scale_full_h - lh;
  win->scale_skew = win->set->lap_skew * h / th;

  assert(win->scale_w > 0);
  assert(win->scale_h > 0);

  sdlgl_mark_dirty(win->scr_x, win->scr_y, win->scr_w, win->scr_h,
      win->scr_depth);
}

void sdlgl_set_cursor(struct TileWindow *win, int x, int y, int w)
{
  if (win->curs_x >= 0)
    mark_dirty_tiles(win, win->curs_x, win->curs_y, win->curs_w, 1);

  win->curs_x = x;
  win->curs_y = y;
  win->curs_w = w;

  if (win->curs_x >= 0)
    mark_dirty_tiles(win, win->curs_x, win->curs_y, win->curs_w, 1);
}


/* ---------------------------------------------------------------- */

/*
 * test if an on-screen tile is completely obscured by a window
 * sitting above it.  `start_idx' is the index of the first window to
 * test (usually win->mapped_idx+1).
 */
int sdlgl_test_tile_visible(int start_idx, int x, int y, int w, int h)
{
  int x2 = x + w - 1;
  int y2 = y + h - 1;
  
  int scr_x2, scr_y2;
  
  assert(start_idx >= 0);

  for (; start_idx < mapped_num; start_idx++)
  {
    struct TileWindow *win = mapped_tilewins[start_idx];

    assert(win);

    if (win->scr_depth < start_depth)
      continue;
     
    if (win->see_through)
      continue;

    scr_x2 = win->scr_x + win->scr_w - 1;
    scr_y2 = win->scr_y + win->scr_h - 1;
      
    if (win->scr_x <= x && x2 <= scr_x2 &&
        win->scr_y <= y && y2 <= scr_y2)
      return 0;
  }

  /* yes is visible (maybe only partially) */
  return 1;
}

static void draw_tilewindow(struct TileWindow *win)
{
  int x, y, n;
  int sx, sy, sw, sh, sfw, sfh;
  int top, bottom, left, right;
  tilecol_t tilecol = 0;
  struct TilePair *cur;

  /* draw background.  Note: TILE_EMPTY guarantees that the background
   * may show through, even on non-text windows.
   */
  if (! win->see_through)
    sdlgl_draw_background(win->scr_x, win->scr_y, win->scr_w, win->scr_h,
        win->background, win->mapped_idx + 1, win->scr_depth);
 
  /* use scissor test to clip tiles to the window.  This is for tiles
   * that cross the window boundary -- we already skip tiles that lie
   * completely outside of the window.
   */
  sdlgl_enable_clipper(win->scr_x, win->scr_y, win->scr_w, win->scr_h);

  if (win->has_border)
    sdlgl_draw_border(win, BORDER_COL);

  /* block cursors are drawn under (text) tiles, outline cursors are
   * drawn over (map) tiles.
   */
  if (win->curs_x >= 0 && win->curs_block)
    sdlgl_draw_cursor(win);

  sw = win->scale_w;
  sh = win->scale_h;
  sfw = win->scale_full_w;
  sfh = win->scale_full_h;
 
  top    = win->scr_y + win->scr_h;
  bottom = win->scr_y;
  left   = win->scr_x;
  right  = win->scr_x + win->scr_w;
  
  /* TWO PASSES: first draw all floor tiles, which are guaranteed to
   * never overlap (in Isometric mode).  Secondly, draw the other
   * stuff on top.  Call this "poor man's Z buffering" :).
   */
 
  if (! win->is_text)
  {
    sdlgl_begin_tile_draw(1, 0);

    for (y=win->total_h - 1; y >= 0; y--)
    {
      sy = win->scr_y - win->pan_y + y * sh;
      if (sy + sfh <= bottom || sy >= top)
        continue;

      for (x=0; x < win->total_w; x++)
      {
        sx = win->scr_x - win->pan_x + x * sw + y * win->scale_skew;
        if (sx + sfw <= left || sx >= right)
          continue;

        if (!sdlgl_test_tile_visible(win->mapped_idx + 1, sx, sy, sfw, sfh))
          continue;

        cur = win->tiles + (y * win->total_w + x);

        if (cur->u.bg != TILE_EMPTY)
            sdlgl_draw_tile(win, sx, sy, sfw, sfh, cur->u.bg, 0, 
                /* flags */ 0, /* layer */ 0);
      }
    }

    sdlgl_finish_tile_draw();
  }

  /* second pass: */

  sdlgl_begin_tile_draw(1, (win->set->lap_w || win->set->lap_h));

  for (y=win->total_h - 1; y >= 0; y--)
  {
    sy = win->scr_y - win->pan_y + y * sh;
    if (sy + sfh <= bottom || sy >= top)
      continue;

    for (x=0; x < win->total_w; x++)
    {
      sx = win->scr_x - win->pan_x + x * sw + y * win->scale_skew;
      if (sx + sfw <= left || sx >= right)
        continue;
      
      if (!sdlgl_test_tile_visible(win->mapped_idx + 1, sx, sy, sfw, sfh))
        continue;
      
      cur = win->tiles + (y * win->total_w + x);

      /* for non-text windows, tilecol is unused */
      if (win->is_text)
        tilecol = cur->u.col;

      if (! win->is_text)
      {
        if (cur->mg != TILE_EMPTY)
          sdlgl_draw_tile(win, sx, sy, sfw, sfh, cur->mg, 0,
              /* flags */ 0, /* layer */ 1);
      }

      if (cur->fg != TILE_EMPTY)
        sdlgl_draw_tile(win, sx, sy, sfw, sfh, cur->fg, tilecol,
            cur->flags, /* layer */ 2);
    }
  }

  sdlgl_finish_tile_draw();

  /* draw the extra shapes, like the love heart */
  for (n=0; n < win->extra_num; n++)
    sdlgl_draw_extra_shape(win, win->extra_shapes + n);

  if (win->curs_x >= 0 && ! win->curs_block)
    sdlgl_draw_cursor(win);

  sdlgl_disable_clipper();
}

void sdlgl_draw_mapped(void)
{
  int i;
  
  /* clear any parts of the screen that windows may not cover.  We are
   * using draw_background() to limit the amount to draw.
   */
  sdlgl_draw_background(0, 0, sdlgl_width, sdlgl_height, 
      RGB_MAKE(0, 0, 0), 0 /* start_idx */, 0 /* depth */);

  for (i=0; i < mapped_num; i++)
    if (mapped_tilewins[i]->scr_depth >= start_depth)
      draw_tilewindow(mapped_tilewins[i]);

  /* handle fading */
  if (fade_amount > 0)
    sdlgl_draw_fading(fade_amount);
}

void sdlgl_flush(void)
{
  sdlgl_draw_mapped();
  sdlgl_blit_frame();
}

#endif /* GL_GRAPHICS */
/*gl_tile.c*/
