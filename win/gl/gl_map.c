/* Copyright (C) 2002 Andrew Apted <ajapted@users.sourceforge.net> */
/* NetHack may be freely redistributed.  See license for details.  */

/*
 * SDL/GL window port for NetHack & Slash'EM.
 *
 * Handles the map window.
 */

#include "hack.h"
#include "display.h"
#include "patchlevel.h"

#if defined(GL_GRAPHICS) || defined(SDL_GRAPHICS)

#define WINGL_INTERNAL
#include "winGL.h"


static int zoom_sets[] =
{
  10, 12,
  16, 20, 24, 
  32, 40, 48, 
  64, 80, 96,
  128
};

#define NUM_ZOOM  (SIZE(zoom_sets))


int sdlgl_quantize_zoom(int zoom_h)
{
  int i;

  if (zoom_h == TEXT_ZOOM)
    return zoom_h;

  if (sdlgl_software)
  {
    if (iflags.wc_tile_height <= 16)
    {
      /* with 16x16 tileset in SW mode, no zooming possible */
      return 16;
    }

    return (zoom_h < iflags.wc_tile_height * 3 / 4) ?
        (iflags.wc_tile_height / 2) : iflags.wc_tile_height;
  }

  for (i=NUM_ZOOM-1; i > 0; i--)
  {
    if (zoom_sets[i] > iflags.wc_tile_height * 4)
      continue;

    if (zoom_sets[i-1] <= iflags.wc_tile_height / 4)
      break;

    if (zoom_h >= zoom_sets[i])
      break;
  }

  return zoom_sets[i];
}

static void center_on_text_map(struct TextWindow *win)
{
  int tw, th;
  int mx, my;

  assert(win->base);
  assert(win->base->is_text);

  tw = win->base->scale_w;
  th = win->base->scale_h;

  /* doesn't need to use scale_skew (text map is never skewed) */

  mx = win->base->total_w * tw / 2 - win->base->scr_w / 2;
  my = win->base->total_h * th / 2 - win->base->scr_h / 2;

  sdlgl_set_pan(win->base, mx, my);
}

static void center_on_player(struct TextWindow *win)
{
  int tw, th, tk;
  int fx, fy;

  assert(win->base);
  assert(! win->base->is_text);

  tw = win->base->scale_w;
  th = win->base->scale_h;
  tk = win->base->scale_skew;

  fx = win->focus_x * tw + tw / 2 - win->base->scr_w / 2 + 
       win->focus_y * tk;
  fy = win->focus_y * th + th / 2 - win->base->scr_h / 2;

  sdlgl_set_pan(win->base, fx, fy);

  win->map_px = fx;
  win->map_py = fy;

  win->jail_x = 0;
  win->jail_y = 0;
}

void sdlgl_create_map(struct TextWindow *win, int w, int h)
{
  int i;

  assert(win->base);

  win->base->is_map = 1;
  win->base->curs_color = OUTLINE_COL;

  win->glyphs = (struct GlyphPair *) alloc(w * h *
      sizeof(struct GlyphPair));

  for (i=0; i < win->base->total_h * win->base->total_w; i++)
  {
    win->glyphs[i].bg = NO_GLYPH;
    win->glyphs[i].fg = NO_GLYPH;
  }

  win->map_px = 0;
  win->map_py = 0;
   
  if (sdlgl_def_zoom == TEXT_ZOOM)
  {
    win->zoom_h = iflags.wc_tile_height;

    sdlgl_change_tileset(win->base, sdlgl_font_map, 1);
    sdlgl_set_scale(win->base, sdlgl_font_map->tile_h);

    center_on_text_map(win);
  }
  else
  {
    win->zoom_h = sdlgl_def_zoom;

    sdlgl_set_scale(win->base, win->zoom_h);

    center_on_player(win);
  }
}

static int glyph_is_translucent(int glyph_fg, int lev_x, int lev_y)
{
  /* The glyph can be drawn 50% translucent under these conditions:
   *   (a) it is a monster or the player.
   *   (b) it is invisible.
   *   (c) the player can see invisible.
   *   (d) if it wasn't invisible, the player would be able to see it
   *       using normal vision.
   */
  
  struct monst *mtmp;

  if (glyph_fg == NO_GLYPH)
    return 0;
  
  assert(glyph_fg <= MAX_GLYPH);

  /* handle the player */
  if (lev_x == u.ux && lev_y == u.uy)
  {
    return canseeself() && Invis;
  }

  if (! glyph_is_monster(glyph_fg))
    return 0;

  mtmp = m_at(lev_x, lev_y);

  if (! mtmp)
    return 0;

  return knowninvisible(mtmp);
}

static int glyph_is_dungeon(glyphidx_t glyph)
{
  if (! glyph_is_cmap(glyph))
    return 0;

  /* There are some glyphs in the CMAP range that can stay in the
   * foreground (everything else in CMAP must go into background).
   *
   * These belong elsewhere IMHO (either ZAP range or a new one).
   */
  if (glyph == cmap_to_glyph(S_digbeam) ||
      glyph == cmap_to_glyph(S_flashbeam) ||
      glyph == cmap_to_glyph(S_boomleft) ||
      glyph == cmap_to_glyph(S_boomright))
  {
    return 0;
  }

  if (glyph >= cmap_to_glyph(S_ss1) &&
      glyph <= cmap_to_glyph(S_ss4))
  {
    return 0;
  }

  if (glyph >= cmap_to_glyph(S_explode1) &&
      glyph <= cmap_to_glyph(S_explode9))
  {
    return 0;
  }

  return 1;
}

static int back_to_trap(XCHAR_P lev_x, XCHAR_P lev_y)
{
  struct trap *trap = t_at(lev_x, lev_y);

  if (!trap || !trap->tseen)
    return NO_GLYPH;

  return trap_to_glyph(trap);
}

#define cannot_see(x,y)  \
    (Blind || (viz_array && !cansee((x), (y))))

static void make_double_glyph(struct GlyphPair *gpair,
    XCHAR_P lev_x, XCHAR_P lev_y, glyphidx_t glyph)
{
  int back;
  
  if (glyph_is_dungeon(glyph))
  {
    back  = glyph;
    glyph = NO_GLYPH;
  }
  else
  {
    back = back_to_glyph(lev_x, lev_y);

    /* handle traps */
    if (back == NO_GLYPH ||
        back == cmap_to_glyph(S_room) ||
        back == cmap_to_glyph(S_stone) ||
        back == cmap_to_glyph(S_corr) ||
        back == cmap_to_glyph(S_litcorr))
    {
      int tt = back_to_trap(lev_x, lev_y);

      if (tt != NO_GLYPH && tt != glyph)
        back = tt;
    }

    if (!levl[lev_x][lev_y].waslit && cannot_see(lev_x, lev_y))
    {
      if (back == cmap_to_glyph(S_room))
        back = cmap_to_glyph(S_stone);
      else if (back == cmap_to_glyph(S_litcorr))
        back = cmap_to_glyph(S_corr);
    }
  }

  /* S_stone (a fully black tile) never needs to be drawn.
   */
  if (back == cmap_to_glyph(S_stone))
    back = NO_GLYPH;
  
  if (back == glyph)
    glyph = NO_GLYPH;

  gpair->fg = glyph;
  gpair->bg = back;
}

static void glyph_to_character(struct TextWindow *win,
    struct GlyphPair * gpair, int x, int y)
{
  int ch;
  rgbcol_t rgb;
   
  int glyph = (gpair->fg != NO_GLYPH) ? gpair->fg : gpair->bg;

  int color;
  unsigned special;

  if (glyph == NO_GLYPH)
  {
    sdlgl_blank_area(win->base, x, y, 1, 1);
    return;
  }
  
  /* Map the glyph back to a character */

  mapglyph(glyph, &ch, &color, &special, x, y);

  if (color == NO_COLOR)
    rgb = L_GREY;
  else
    rgb = termcolor_to_tilecol[color];

  /* store it */
  {
    char ch_b = (char)ch;
    sdlgl_store_char(win->base, x, y, ch_b, rgb);
  }
}

static void glyph_to_tilepair(struct TextWindow *win,
    struct GlyphPair *glyph, int x, int y)
{
  tileidx_t fg, mg, bg;
  tileflags_t flags = 0;

  int lev_x = x;
  int lev_y = win->base->total_h - 1 - y;

  /* foreground */

  if (glyph->fg == NO_GLYPH)
  {
    fg = TILE_EMPTY;
  }
  else
  {
    int glyph_fg = glyph->fg;  /* prevent glyph_is_monster() warning */
     
    assert(glyph->fg <= MAX_GLYPH);

    fg = glyph2tile[glyph->fg];

    /* support for horizontal flipping */
    if (sdlgl_flipping)
    {
      int dx = 0;

      /* handle the player */
      if (lev_x == u.ux && lev_y == u.uy && fg < NUM_MON_TILES)
      {
        dx = (u.dx != 0) ? u.dx : win->player_dx;
      }
      else if (glyph_is_monster(glyph_fg) && m_at(lev_x, lev_y))
      {
        struct monst *mon = m_at(lev_x, lev_y);

        int i;

        /* find last horizontal move.  A better approach would be to add
         * a field to the monster struct for which way the monster is
         * facing (8 possible dirs), updated in the movement (and
         * attacking !) code.  That's a lot of work though...
         */
        for (i=0; i < MTSZ; i++)
        {
          dx = mon->mx - mon->mtrack[i].x;

          if (dx != 0)
            break;
        }
      }

      dx = dx * -sdlgl_mon_tile_face_dir(fg);

      if (dx > 0)
        flags |= TILE_F_FLIPX;
    }

    /* support for translucent invisibles */
    if (sdlgl_invis_fx && iflags.wc_tile_height >= 32)
    {
      if (glyph_is_translucent(glyph_fg, lev_x, lev_y))
        flags |= TILE_F_TRANS50;
    }
  }

  /* background and mid-ground */

  if (glyph->bg == NO_GLYPH)
  {
    mg = bg = TILE_EMPTY;
  }
  else
  {
    assert(glyph->bg <= MAX_GLYPH);
    
    /* the ROOM tile always lets the BG shine through */
    if (glyph->bg == cmap_to_glyph(S_room))
      mg = TILE_EMPTY;
    else
      mg = glyph2tile[glyph->bg];

    bg = 
#ifdef VANILLA_GLHACK
      In_mines(&u.uz) ? Fl_Mine :
         In_sokoban(&u.uz) ? Fl_Sokoban :
         Is_knox(&u.uz) ? Fl_Knox :
        (In_V_tower(&u.uz) || In_W_tower(lev_x, lev_y, &u.uz)) ? Fl_Tower :
         In_hell(&u.uz) ? Fl_Hell :
         In_quest(&u.uz) ? Fl_Quest :
         In_endgame(&u.uz) ? Fl_Astral :
         Is_medusa_level(&u.uz) ? Fl_Beach :
# ifndef VANILLA_GLHACK
         /* FIXME !!! In_caves(&u.uz) ? Fl_Caves : */
# endif
# ifdef REINCARNATION
         Is_rogue_level(&u.uz) ? Fl_Rogue :
# endif
#endif
    /* otherwise */
         glyph2tile[cmap_to_glyph(S_room)];
  }

  sdlgl_store_tile(win->base, x, y, fg, mg, bg, flags);
}

static void update_map_extras(struct TextWindow *win,
    short x, short y, struct GlyphPair *glyph)
{
  struct TileWindow *base = win->base;

  int worm_tail;

  sdlgl_remove_extrashapes(base, x, y);

  worm_tail = (glyph->fg == petnum_to_glyph(PM_LONG_WORM_TAIL));

  if (iflags.wc_hilite_pet && ! worm_tail)
  {
    int on_left = 0;
    
    /* determine if heart is placed on left or right side */
    if (sdlgl_flipping && ! base->is_text &&
        (base->tiles[(int)y * base->total_w + x].flags & TILE_F_FLIPX))
    {
      on_left = 1;
    }

    if (glyph_is_pet(glyph->fg))
      sdlgl_add_extrashape(base, SHAPE_Heart, x, y, on_left, 0);
    else if (glyph_is_ridden_monster(glyph->fg))
      sdlgl_add_extrashape(base, SHAPE_Ridden, x, y, on_left, 0);
  }
}

/*
 *  sdlgl_print_glyph
 *
 *  Print the glyph to the output device.  Don't flush the output device.
 *
 *  Since this is only called from show_glyph(), it is assumed that the
 *  position and glyph are always correct (checked there)!
 */
void Sdlgl_print_glyph(winid window, XCHAR_P x, XCHAR_P y, int glyph)
{
  struct TextWindow *win;

  int xx, yy, offset;
  
  if (window == WIN_ERR)
    return;

  assert (0 <= window && window < MAXWIN && text_wins[window]);
  win = text_wins[window];

  if (win->type != NHW_MAP)
    return;

  if (! win->base)
    return;

  assert(win->glyphs);

  xx = (int)x;
  yy = win->base->total_h - 1 - (int)y;
    
  if (xx < 0 || xx >= win->base->total_w ||
      yy < 0 || yy >= win->base->total_h)
  {
    return;
  }

  /* give the map window a border.  We do this *here* so that the
   * border doesn't get drawn during the initial game prompts.
   */
  win->base->has_border = 1;

  offset = yy * win->base->total_w + xx;

  make_double_glyph(win->glyphs + offset, x, y, (glyphidx_t) glyph);

  if (win->base->is_text)
    glyph_to_character(win, win->glyphs + offset, xx, yy);
  else
    glyph_to_tilepair(win, win->glyphs + offset, xx, yy);
    
  update_map_extras(win, xx, yy, win->glyphs + offset);
}

static void update_all_glyphs(struct TextWindow *win)
{
  int x, y;
  int offset;

  assert(win->base);
  assert(win->glyphs);

  for (y=0; y < win->base->total_h; y++)
  for (x=0; x < win->base->total_w; x++)
  {
    offset = y * win->base->total_w + x;
 
    if (win->base->is_text)
      glyph_to_character(win, win->glyphs + offset, x, y);
    else
      glyph_to_tilepair(win, win->glyphs + offset, x, y);
    
    update_map_extras(win, x, y, win->glyphs + offset);
  }
}

#if 0
/* clips the given vector (starting at the origin) by the rectangle
 * centered on the origin, with width = 2*w and height = 2*h.
 */
static void clip_vector(int *dx, int *dy, int w, int h)
{
  int neg_dx;
  int neg_dy;

  if (( neg_dx = ((*dx) < 0) )) (*dx) = -(*dx);
  if (( neg_dy = ((*dy) < 0) )) (*dy) = -(*dy);

  if ((*dx) > w)
  {
    (*dy) = (*dy) * w / (*dx);
    (*dx) = w;
  }

  if ((*dy) > h)
  {
    (*dx) = (*dx) * h / (*dy);
    (*dy) = h;
  }

  if (neg_dx) (*dx) = -(*dx);
  if (neg_dy) (*dy) = -(*dy);
}
#endif

/* Definitive jail:
 *
 *   WIDTH = scr_w * jail_size / 100;    (pixels)
 *
 *   SCREEN X1 = scr_w/2 + jail_sdx - jw/2;
 *   SCREEN X2 = scr_w/2 + jail_sdx + jw/2;
 *
 *   MAP X1 = (map_px + SCREEN X1) / tw;    (whole tiles)
 *   MAP X2 = (map_px + SCREEN X2) / tw;
 *
 *   MAP W = (MAP X2 - MAP X1) + 1;
 */
static void update_jail(struct TextWindow *win)
{
  int fx, fy, tw, th, tk;
  int jx, jy, jw, jh;
  int extra_x, extra_y;

  struct TileWindow *base = win->base;

  assert(base);

  tw = base->scale_w;
  th = base->scale_h;
  tk = base->scale_skew;

  /* no jail required for text view.  We just center the map on the
   * screen.  We assume it fits -- only when using a very small mode
   * (640x400) is there any chance it won't (the user can always use
   * the 8x8 font instead of the 8x14 one).
   */
  if (base->is_text)
    return;

  /* handle chunky scrolling mode here.  The X margin is inflated
   * since the map window is a lot wider than it is high.
   */
  if (sdlgl_jump_scroll)
  {
    int marg_x = tw * iflags.wc_scroll_margin * 17 / 10;
    int marg_y = th * iflags.wc_scroll_margin;

    fx = win->focus_x * tw + tw / 2 + win->focus_y * tk;
    fy = win->focus_y * th + th / 2;

    if (fx < win->map_px + marg_x ||
        fx > win->map_px + base->scr_w - marg_x)
    {
      win->map_px = fx - win->base->scr_w / 2;
    }

    if (fy < win->map_py + marg_y ||
        fy > win->map_py + base->scr_h - marg_y)
    {
      win->map_py = fy - win->base->scr_h / 2;
    }

    sdlgl_set_pan(win->base, win->map_px, win->map_py);
    return;
  }

  /* make sure jail offset is valid */

  extra_x = base->scr_w * (100 - sdlgl_jail_size) / 200 + 2;
  extra_y = base->scr_h * (100 - sdlgl_jail_size) / 200 + 2;
 
  if (abs(win->jail_x) > extra_x)
    win->jail_x = sgn(win->jail_x) * extra_x;
   
  if (abs(win->jail_y) > extra_y)
    win->jail_y = sgn(win->jail_y) * extra_y;
   
  /* shift jail if focus has moved outside it, panning the map window
   * when necessary.
   *
   * This is complicated by allowing the jail to be non-centered on
   * the screen; the code here "auto-centers" it rather than panning.
   * Removing the loops is left as an exercise for the reader :).
   */
  
  fx = win->focus_x * tw + tw / 2 + win->focus_y * tk;
  fy = win->focus_y * th + th / 2;

  /* jail no smaller than one tile */
  jw = max(tw, base->scr_w * sdlgl_jail_size / 100);
  jh = max(th, base->scr_h * sdlgl_jail_size / 100);

  jx = win->map_px + base->scr_w/2 + win->jail_x - jw/2;
  jy = win->map_py + base->scr_h/2 + win->jail_y - jh/2;

  while (fx < jx)
  {
    if (win->jail_x >= tw)
      win->jail_x -= tw;
    else
      win->map_px -= tw;
    
    jx = win->map_px + base->scr_w/2 + win->jail_x - jw/2;
  }

  while (fy < jy)
  {
    if (win->jail_y >= th)
      win->jail_y -= th;
    else
      win->map_py -= th;
    
    jy = win->map_py + base->scr_h/2 + win->jail_y - jh/2;
  }

  while (fx > jx + jw)
  {
    if (win->jail_x <= -tw)
      win->jail_x += tw;
    else
      win->map_px += tw;
    
    jx = win->map_px + base->scr_w/2 + win->jail_x - jw/2;
  }

  while (fy > jy + jh)
  {
    if (win->jail_y <= -th)
      win->jail_y += th;
    else
      win->map_py += th;
    
    jy = win->map_py + base->scr_h/2 + win->jail_y - jh/2;
  }

  /* auto-center the jail when possible.  Panning not affected.
   */
 
  while (win->jail_x <= -tw && fx > jx + tw)
  {
    win->jail_x += tw;
    jx = win->map_px + base->scr_w/2 + win->jail_x - jw/2;
  }
 
  while (win->jail_y <= -th && fy > jy + th)
  {
    win->jail_y += th;
    jy = win->map_py + base->scr_h/2 + win->jail_y - jh/2;
  }
 
  while (win->jail_x >= tw && fx < jx + jw - tw)
  {
    win->jail_x -= tw;
    jx = win->map_px + base->scr_w/2 + win->jail_x - jw/2;
  }
 
  while (win->jail_y >= th && fy < jy + jh - th)
  {
    win->jail_y -= th;
    jy = win->map_py + base->scr_h/2 + win->jail_y - jh/2;
  }

  sdlgl_set_pan(win->base, win->map_px, win->map_py);
}

void Sdlgl_cliparound(int x, int y)
{
  struct TextWindow *win;

  int player_x, player_y;

  if (sdlgl_map_win == WIN_ERR)
    return;
  
  win = text_wins[sdlgl_map_win];
  assert(win);
    
  if (! win->base)
    return;

  player_x = u.ux;
  player_y = win->base->total_h - 1 - u.uy;

  /* update player direction */
  if (u.dx != 0)
    win->player_dx = u.dx;

  y = win->base->total_h - 1 - y;

  if (x < 0 || x >= win->base->total_w ||
      y < 0 || y >= win->base->total_h)
    return;

  win->focus_x = x;
  win->focus_y = y;

  update_jail(win);

  /* draw cursor, unless it's on the player */
  if (!win->write_cursor && x == player_x && y == player_y)
  {
    sdlgl_set_cursor(win->base, -1, -1, 1);
  }
  else
  {
    sdlgl_set_cursor(win->base, x, y, 1);
  }
}

int sdlgl_cursor_visible(void)
{
  struct TextWindow *win;

  if (sdlgl_map_win == WIN_ERR)
    return 0;
  
  win = text_wins[sdlgl_map_win];
  assert(win);
    
  if (! win->base)
    return 0;

  return (win->base->curs_x >= 0);
}

static void do_zoom(struct TextWindow *win, int zoom_h)
{
  struct TileWindow *base;
  
  int tw, th, tk;
  int dx, dy;
  int cur_x, cur_y;

  base = win->base;
  assert(base);

  assert(! base->is_text);

  assert(zoom_h != TEXT_ZOOM);
  assert(sdlgl_quantize_zoom(zoom_h) == zoom_h);

  if (zoom_h == win->zoom_h)
    return;

  /* compute the center of the focus tile, in terms of on-screen
   * pixels.
   */
  tw = base->scale_w;
  th = base->scale_h;
  tk = base->scale_skew;

  cur_x = win->focus_x * tw + tw / 2 - win->map_px + win->focus_y * tk;
  cur_y = win->focus_y * th + th / 2 - win->map_py;

  /* update zoom */

  win->zoom_h = zoom_h;

  /* update panning, so that the focus tile remains as close as
   * possible on the screen to where it was before.
   */
  sdlgl_set_scale(win->base, zoom_h);

  tw = win->base->scale_w;
  th = win->base->scale_h;
  tk = win->base->scale_skew;

  win->map_px = win->focus_x * tw + tw / 2 - cur_x + win->focus_y * tk;
  win->map_py = win->focus_y * th + th / 2 - cur_y;

  /* move jail center towards focus, upto 1 tile step.  The rough
   * clipping (handling dx & dy separately) doesn't matter.  We rely
   * on update_jail() to limit jail_x/y to valid values.
   */
  dx = cur_x - (base->scr_w / 2 + win->jail_x);
  dy = cur_y - (base->scr_h / 2 + win->jail_y);

  if (abs(dx) >= tw)
    dx = sgn(dx) * tw;

  if (abs(dy) >= th)
    dy = sgn(dy) * th;

  win->jail_x += dx / 2;
  win->jail_y += dy / 2;

  update_jail(win);
}

/* adjust is -1 to zoom out, +1 to zoom in,
 *           -2 to zoom to maximum, +2 to zoom to default.
 */
void sdlgl_zoom_map(int adjust)
{
  struct TextWindow *win;

  int zoom_h;

  if (sdlgl_map_win == WIN_ERR)
    return;

  win = text_wins[sdlgl_map_win];
  assert(win);
  assert(win->zoom_h != TEXT_ZOOM);

  /* ignore CTRL-PGUP/PGDN if the default zoom is text-mode.
   */
  if (sdlgl_def_zoom == TEXT_ZOOM && abs(adjust) == 2)
    return;

  /* allow CTRL-PGUP/PGDN to switch from text-mode.  The rationale
   * behind not doing this for plain PGUP/DN is that the result could
   * be confusing or unexpected.  With CTRL, a specific zoom factor is
   * implied.
   */
  if (win->base->is_text)
  {
    if (abs(adjust) != 2)
      return;

    sdlgl_toggle_text_view();
  }

  if (adjust == -2)
    zoom_h = sdlgl_quantize_zoom(1);
  else if (adjust == +2)
    zoom_h = sdlgl_def_zoom;
  else
  {
    if (sdlgl_software)
    {
      if (iflags.wc_tile_height <= 16)
        return;

      zoom_h = (adjust < 0) ? (iflags.wc_tile_height / 2) :
          iflags.wc_tile_height;
    }
    else  /* OpenGL, all zooms are available */
    {
      int i;

      for (i=0; i < NUM_ZOOM; i++)
        if (zoom_sets[i] == win->zoom_h)
          break;

      assert(i != NUM_ZOOM);

      i = max(0, min(NUM_ZOOM-1, i + adjust));

      zoom_h = sdlgl_quantize_zoom(zoom_sets[i]);
    }
  }
 
  do_zoom(win, zoom_h);
}

void sdlgl_center_screen_on_player(void)
{
  struct TextWindow *win;
  
  if (sdlgl_map_win == WIN_ERR)
    return;

  win = text_wins[sdlgl_map_win];
  assert(win);

  if (win->base->is_text)
    return;

  center_on_player(win);
}

void sdlgl_toggle_text_view(void)
{
  struct TextWindow *win;

  if (sdlgl_map_win == WIN_ERR)
    return;

  win = text_wins[sdlgl_map_win];
  assert(win);
  assert(win->base);
  assert(win->glyphs);

  if (win->base->is_text)
  {
    sdlgl_change_tileset(win->base, sdlgl_tiles, 0);
    update_all_glyphs(win);

    sdlgl_set_scale(win->base, win->zoom_h);

    update_jail(win);
    return;
  }

  sdlgl_change_tileset(win->base, sdlgl_font_map, 1);
  update_all_glyphs(win);

  sdlgl_set_scale(win->base, sdlgl_font_map->tile_h);

  center_on_text_map(win);
}

#ifdef POSITIONBAR
void Sdlgl_update_positionbar(char *posbar)
{
  /* implement this one day... */
}
#endif

/* returns 1 if found (updating the x/y coords), otherwise 0.
 */
int sdlgl_find_click(int window, int *x, int *y)
{
  struct TextWindow *win;
  struct TileWindow *base;

  int xx, yy;
  
  if (window == WIN_ERR)
    return 0;

  win = text_wins[window];
  if (! win || ! win->base)
    return 0;

  base = win->base;

  xx = *x - base->scr_x;
  yy = sdlgl_height - 1 - *y - base->scr_y;

  if (xx < 0 || yy < 0 || xx >= base->scr_w || yy >= base->scr_h)
    return 0;

  yy = (yy + base->pan_y) / base->scale_h;
  xx = (xx + base->pan_x - yy * base->scale_skew) / base->scale_w;
 
  if (xx < 0 || yy < 0 || xx >= base->total_w || yy >= base->total_h)
    return 0;
  
  *x = xx;
  *y = base->total_h - 1 - yy;
 
  return 1;
}

/*
 * Pan the map by the given offset.  The dx/dy values have the
 * following meanings (in absolute value) :
 *
 *    0  -  no change (but compute anyway).
 *    1  -  move by a single tile.
 *    2  -  move by a bunch of tiles (5 or so).
 *    3  -  move by a "page" (the window size).
 *    4  -  move as far as possible.
 */
void sdlgl_pan_map_window(int dx, int dy)
{
  struct TextWindow *win;

  int tw, th;
  int show_w, show_h;
  int map_w, map_h;

  if (sdlgl_map_win == WIN_ERR)
    return;

  win = text_wins[sdlgl_map_win];
  assert(win);
  assert(win->base);

  if (win->base->is_text)
    return;

  tw = win->base->scale_w;
  th = win->base->scale_h;

  show_w = max(1, win->base->scr_w / tw);
  show_h = max(1, win->base->scr_h / th);

  map_w = win->base->total_w * win->base->scale_w +
          win->base->total_h * win->base->scale_skew;
  map_h = win->base->total_h * win->base->scale_h;

  switch (abs(dx))
  {
    case 2:
      dx = sgn(dx) * min(6, show_w);
      break;

    case 3:
      dx = sgn(dx) * (show_w - 1);
      break;

    case 4:
      dx = sgn(dx) * 999;
      break;
  }

  switch (abs(dy))
  {
    case 2:
      dy = sgn(dy) * min(4, show_h);
      break;

    case 3:
      dy = sgn(dy) * (show_h - 1);
      break;
        
    case 4:
      dy = sgn(dy) * 999;
      break;
  }

  dx *= tw;
  dy *= th;

  /* limit the amount moved so that the map stays on-screen.
   */
  map_w = max(0, map_w - win->base->scr_w);
  map_h = max(0, map_h - win->base->scr_h);
   
  if (dx < 0 && win->map_px + dx < -2 * tw)
    dx = min(0, -2 * tw - win->map_px);

  if (dx > 0 && win->map_px + dx > map_w + 2 * tw)
    dx = max(0, map_w + 2 * tw - win->map_px);

  if (dy < 0 && win->map_py + dy < -2 * th)
    dy = min(0, -2 * th - win->map_py);

  if (dy > 0 && win->map_py + dy > map_h + 2 * th)
    dy = max(0, map_h + 2 * th - win->map_py);

  /* update jail center */

  if (dx < 0)
    win->jail_x = min(0, win->jail_x + -dx);
  else if (dx > 0)
    win->jail_x = max(0, win->jail_x - dx);
   
  if (dy < 0)
    win->jail_y = min(0, win->jail_y + -dy);
  else if (dy > 0)
    win->jail_y = max(0, win->jail_y - dy);
   
  /* update panning */

  win->map_px += dx;
  win->map_py += dy;

  sdlgl_set_pan(win->base, win->map_px, win->map_py);
  sdlgl_flush();
}

#endif /* GL_GRAPHICS */
/*gl_map.c*/
