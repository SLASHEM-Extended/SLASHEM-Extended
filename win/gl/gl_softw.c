/* Copyright (C) 2002 Andrew Apted <ajapted@users.sourceforge.net> */
/* NetHack may be freely redistributed.  See license for details.  */

/*
 * SDL/GL window port for NetHack & Slash'EM.
 *
 * Software drawing code (version without OpenGL).
 *
 * Based on work by Allister MacLeod (C) 2002.
 */

#include "hack.h"
#include "patchlevel.h"

#if defined (SDL_GRAPHICS)  /* software only */

#define WINGL_INTERNAL
#include "winGL.h"


struct DirtyMatrix *sdlgl_matrix = NULL;

static SDL_Surface *pet_mark_surf = NULL;
static SDL_Surface *ridden_mark_surf = NULL;

static SDL_Surface *darkness = NULL;


void sw_rend_startup(void)
{
  if (sdlgl_depth == 8)
  {
    sdlgl_set_surface_colors(sdlgl_surf);
  }

  sdlgl_matrix = sdlgl_create_dirty_matrix(sdlgl_width, sdlgl_height);
}

void sw_rend_shutdown(void)
{
  sdlgl_free_dirty_matrix(sdlgl_matrix);
}


/*------------------------------------------------------------------------*/

static GH_INLINE Uint32 rgb2sdl(SDL_PixelFormat *fmt, rgbcol_t color)
{
  Uint8 r = GAMMA(RGB_RED(color));
  Uint8 g = GAMMA(RGB_GRN(color));
  Uint8 b = GAMMA(RGB_BLU(color));

  return SDL_MapRGB(fmt, r, g, b);
}

static void create_iso_borders(struct TileSet *set)
{
  int x, y;
  Uint32 c;
  
  /* the surface consists of three tiles, one for the corners, one for
   * horizontal lines, and one for vertical lines.  "+ - |"
   */
  set->borders = SDL_CreateRGBSurface(SDL_SWSURFACE, 48 * 3, 64,
      8 /* bpp */, 0, 0, 0, 0);
 
  if (! set->borders)
    sdlgl_error("Couldn't create SDL surface for iso borders.\n");

  sdlgl_set_surface_colors(set->borders);
  SDL_SetColorKey(set->borders, SDL_SRCCOLORKEY, TRANS_PIX);
 
  c = rgb2sdl(set->borders->format, BORDER_COL);

  for (y=0; y < 64; y++)
  for (x=0; x < 48; x++)
  {
    int horiz = (y == 48) && (x >= 8) && (x < 40);
    int vert  = (y >= 32) && (x == (48 - y / 2));

    SDL_Rect drect;

    drect.x = x; drect.y = y;
    drect.w = drect.h = 1;

    SDL_FillRect(set->borders, &drect, (horiz || vert) ? c : TRANS_PIX);

    drect.x = x + 48; drect.y = y;
    drect.w = drect.h = 1;

    SDL_FillRect(set->borders, &drect, horiz ? c : TRANS_PIX);

    drect.x = x + 96; drect.y = y;
    drect.w = drect.h = 1;

    SDL_FillRect(set->borders, &drect, vert ? c : TRANS_PIX);
  }

  set->borders_small = sdlgl_shrink_surface(set->borders);

  if (! set->borders_small)
    sdlgl_error("Could not create surface for shrunken borders.\n");
}

static struct TileSet *sw_load_tileset(const char *filename,
    int tile_w, int tile_h, int is_text, int keep_rgba,
    int *across, int *down)
{
  struct TileSet *set;

  unsigned char *data_ptr;
  int width, height;
  int num_w, num_h;
  int is_isometric = ! is_text && tile_h == 64;

  data_ptr = sdlgl_load_png_file(filename, &width, &height);
  if (! data_ptr)
  {
    sdlgl_error("Could not load tileset image: %s\n", filename);
    return NULL; /* NOT REACHED */
  }

  set = (struct TileSet *) alloc(sizeof(struct TileSet));
  memset(set, 0, sizeof(struct TileSet));

  assert(sdlgl_surf);

  if (keep_rgba && sdlgl_surf->format->BytesPerPixel > 1)
  {
    set->surf = sdlgl_RGBA_to_truecolor(data_ptr, width, height);
  }
  else
  {
    set->surf = sdlgl_RGBA_to_palettised(data_ptr, width, height);
    free(data_ptr);
  }

  if (! set->surf)
  {
    sdlgl_error("Could not create SDL surface for tileset.\n");
    return NULL; /* NOT REACHED */
  }

  set->surf_small = NULL;
  if (tile_h >= 32)
  {
    set->surf_small = sdlgl_shrink_surface(set->surf);

    if (! set->surf_small)
    {
      sdlgl_error("Could not create SDL surface for shrunken tileset.\n");
      return NULL; /* NOT REACHED */
    }
  }

  set->tile_w = tile_w;
  set->tile_h = tile_h;
  num_w = set->surf->w / tile_w;
  num_h = set->surf->h / tile_h;
  set->tile_num = num_w * num_h;

  set->lap_w = is_isometric ? 16 : 0;
  set->lap_h = is_isometric ? 32 : 0;
  set->lap_skew = is_isometric ? 16 : 0;

  set->pack_w = num_w;
  set->pack_h = num_h;
 
  set->has_alpha = NULL;
  if (! is_text && ! keep_rgba)
  {
    sdlgl_sw_create_has_alpha(set);
  }

  set->font_cache = NULL;
  if (is_text)
  {
    sdlgl_create_font_cache(set);
  }

  set->borders = set->borders_small = NULL;
  if (is_isometric)
  {
    create_iso_borders(set);
  }
   
  if (across)
    *across = num_w;
  if (down)
    *down = num_h;

  return set;
}

static void sw_free_tileset(struct TileSet *set)
{
  if (set->surf)
  {
    SDL_FreeSurface(set->surf);
    set->surf = NULL;
  }

  if (set->surf_small)
  {
    SDL_FreeSurface(set->surf_small);
    set->surf = NULL;
  }

  if (set->has_alpha)
  {
    free(set->has_alpha);
    set->has_alpha = NULL;
  }

  if (set->font_cache)
  {
    sdlgl_free_font_cache(set);
    set->font_cache = NULL;
  }
 
  free(set);
}

static SDL_Surface *mark_to_surface(const unsigned char *bits,
    Uint8 r, Uint8 g, Uint8 b)
{
  int x, y;
  Uint32 c;
  
  SDL_Surface *surf = SDL_CreateRGBSurface(SDL_SWSURFACE, 8, 8, 
      8 /* bpp */, 0, 0, 0, 0);
 
  if (! surf)
    sdlgl_error("Couldn't create SDL surface for pet mark.\n");

  sdlgl_set_surface_colors(surf);
  SDL_SetColorKey(surf, SDL_SRCCOLORKEY, TRANS_PIX);
 
  c = SDL_MapRGB(surf->format, r, g, b);

  for (y=0; y < 8; y++)
  for (x=0; x < 8; x++)
  {
    int pix = bits[y] & (1 << (7-x));

    SDL_Rect drect;

    drect.x = x; drect.y = y;
    drect.w = drect.h = 1;

    SDL_FillRect(surf, &drect, pix ? c : TRANS_PIX);
  }

  return surf;
}
 
static void sw_create_extra_graphics(void)
{
  pet_mark_surf = mark_to_surface(pet_mark_bits, 255, 0, 0);
  ridden_mark_surf = mark_to_surface(ridden_mark_bits, 0, 224, 0);

  assert(pet_mark_surf && ridden_mark_surf);
}

static void sw_free_extra_shapes(void)
{
  if (pet_mark_surf)
    SDL_FreeSurface(pet_mark_surf);
  pet_mark_surf = NULL;

  if (ridden_mark_surf)
    SDL_FreeSurface(ridden_mark_surf);
  ridden_mark_surf = NULL;
}

static void sw_enable_clipper(int x, int y, int w, int h)
{
  SDL_Rect cliprect;

  cliprect.x = x; cliprect.y = sdlgl_height - y - h;
  cliprect.w = w; cliprect.h = h;

  SDL_SetClipRect(sdlgl_surf, &cliprect);
}

static void sw_disable_clipper(void)
{
  SDL_SetClipRect(sdlgl_surf, NULL);
}

static void sw_blit_frame(void)
{
  int num = sdlgl_dirty_matrix_to_updaters(sdlgl_matrix);

  SDL_UpdateRects(sdlgl_surf, num, sdlgl_matrix->updaters);

  sdlgl_dirty_matrix_clear(sdlgl_matrix);
}


/*------------------------------------------------------------------------*/

static GH_INLINE void draw_line(int x1, int y1, int x2, int y2,
    Uint32 c, int depth)
{
  SDL_Rect r;

  /* doesn't handle diagonal lines (never will) */
  assert(x1 == x2 || y1 == y2);

  if (x1 == x2)  /* vertical line */
  {
    r.x = x1;
    r.y = min(y1, y2);
    r.w = 1;
    r.h = max(y1, y2) - r.y + 1;
  }
  else  /* horizontal line */
  {
    r.x = min(x1, x2);
    r.y = y1;
    r.w = max(x1, x2) - r.x + 1;
    r.h = 1;
  }

  sdlgl_dirty_matrix_blit(sdlgl_matrix, NULL, NULL, &r, c, depth);
}

#define BG_CELL  64

static void sw_draw_background(int sx, int sy, int sw, int sh, 
    rgbcol_t color, int start_idx, int depth)
{
  int x, y;

  SDL_Rect r;

  Uint32 c = rgb2sdl(sdlgl_surf->format, color);
 
  for (y=0; y < sh; y += BG_CELL)
  for (x=0; x < sw; x += BG_CELL)
  {
    r.x = sx + x;
    r.y = sy + y;

    r.w = min(sw - x, BG_CELL);
    r.h = min(sh - y, BG_CELL);

    /* the for loop prevents zero sizes */
    assert(r.w > 0 && r.h > 0);
    
    if (! sdlgl_test_tile_visible(start_idx, r.x, r.y, r.w, r.h))
      continue;

    r.y = sdlgl_height - r.y - r.h;

    if (sdlgl_dirty_matrix_test(sdlgl_matrix, r.x, r.y, r.w, r.h, 
        depth) == 0)
    {
      continue;
    }

    sdlgl_dirty_matrix_blit(sdlgl_matrix, NULL, NULL, &r, c, depth);
  }
}

static void sw_draw_extra_shape(struct TileWindow *win, struct ExtraShape *shape)
{
  SDL_Rect drect;
  SDL_Surface *surf;

  int sx, sy, sw, sh;

  sw = win->scale_w;
  sh = win->scale_h;

  sx = win->scr_x - win->pan_x + shape->x * sw + shape->y * win->scale_skew;
  sy = win->scr_y - win->pan_y + shape->y * sh;
   
  /* trivial clipping */
  if (sx + sw <= win->scr_x || sx >= win->scr_x + win->scr_w)
    return;

  if (sy + sh <= win->scr_y || sy >= win->scr_y + win->scr_h)
    return;

  switch (shape->type)
  {
    case SHAPE_None:
      break;

    case SHAPE_Heart:
    case SHAPE_Ridden:
      /* 
       * par1: 0 is normal, 1 puts it on the left.
       */

      /* don't show them in text mode */
      if (sw < 14 || sh < 14)
        return;

      sx += (win->scale_full_w + sw) / 2 - 8;
      sy += (win->scale_full_h + sh) / 2 - 8;

      drect.w = drect.h = 8;
      drect.x = sx;
      drect.y = sdlgl_height - sy - drect.h;

      if (shape->type == SHAPE_Ridden)
        surf = ridden_mark_surf;
      else
        surf = pet_mark_surf;

      sdlgl_dirty_matrix_blit(sdlgl_matrix, surf, NULL, &drect, 
          0, win->scr_depth);
      break;

    default:
      impossible("Unknown extra shape %d", shape->type);
  }
}

static void sw_draw_cursor(struct TileWindow *win)
{
  int x = win->curs_x;
  int y = win->curs_y;
  int w = win->curs_w;

  int sx, sy, sw, sh;

  Uint32 c = rgb2sdl(sdlgl_surf->format, win->curs_color);
   
  assert(x >= 0 && y >= 0 && w > 0);

  sw = win->scale_w;
  sh = win->scale_h;

  sx = win->scr_x - win->pan_x + x * sw + y * win->scale_skew;
  sy = win->scr_y - win->pan_y + y * sh;

  sw = sw * (w - 1) + win->scale_full_w;
  sh = win->scale_full_h;

  /* trivial clipping */
  if (sx + sw <= win->scr_x || sx >= win->scr_x + win->scr_w ||
      sy + sh <= win->scr_y || sy >= win->scr_y + win->scr_h)
  {
    return;
  }

  /* SDL coordinates are y-inverted from OpenGL */
  sy = sdlgl_height - sy - sh;

  if (win->curs_block)
  {
    SDL_Rect r;

    r.x = sx; r.y = sy;
    r.w = sw; r.h = sh;

    sdlgl_dirty_matrix_blit(sdlgl_matrix, NULL, NULL, &r, c, 
        win->scr_depth);
  }
  else
  {
    sw -= 1;  /* don't draw outside of tile boundary */
    sh -= 1;

    draw_line(sx, sy, sx + sw, sy, c, win->scr_depth);
    draw_line(sx + sw, sy, sx + sw, sy + sh, c, win->scr_depth);
    draw_line(sx + sw, sy + sh, sx, sy + sh, c, win->scr_depth);
    draw_line(sx, sy + sh, sx, sy, c, win->scr_depth);
  }
}

static void sw_begin_tile_draw(int blending, int overlap)
{
  /* nothing needed */
}

static void sw_finish_tile_draw(void)
{
  /* nothing needed */
}

static void sw_draw_tile(struct TileWindow *win, int sx, int sy,
    int sw, int sh, tileidx_t tile, tilecol_t tilecol,
    tileflags_t flags, short layer)
{
  struct TileSet *set = win->set;

  SDL_Rect trect, drect;
  SDL_Surface *surf;

  assert(set);
  assert(tile < set->tile_num);

  /* ignore spaces */
  if (win->is_text && TILE_2_CHAR(tile) == (unsigned char)' ')
    return;

  /* SDL coordinates are y-inverted from OpenGL coordinates */
  drect.x = sx;
  drect.y = sdlgl_surf->h - sy - sh;
  drect.w = sw;
  drect.h = sh;

  /* ignore tiles that are not dirty */
  if (sdlgl_dirty_matrix_test(sdlgl_matrix, drect.x, drect.y,
      drect.w, drect.h, win->scr_depth) == 0)
  {
    return;
  }
  
  trect.x = (tile % set->pack_w) * set->tile_w;
  trect.y = (tile / set->pack_w) * set->tile_h;
  trect.w = set->tile_w;
  trect.h = set->tile_h;

  if (win->is_text)
  {
    /* use plain surface if color is bright white */
    if (tilecol == WHITE)
    {
      surf = set->surf;
    }
    else
    {
      int pos_x, pos_y;

      assert(set->font_cache);
      assert(set->font_cache->char_surf);

      sdlgl_font_cache_lookup(set, tile, tilecol, &pos_x, &pos_y);

      trect.x = pos_x * set->tile_w;
      trect.y = pos_y * set->tile_h;
      
      surf = set->font_cache->char_surf;
    }
  }
  else
  {
    surf = set->surf;

    if (set->surf_small && set->tile_h / 2 == win->scale_full_h)
    {
      surf = set->surf_small;

      trect.x /= 2; trect.y /= 2; trect.w /= 2; trect.h /= 2;
    }
  }

  /* NOTE: using SDL_SetAlpha() like this will be quite inefficient
   * if there a lots of translucent tiles being drawn, because it
   * invalidates SDL's blitting tables.  Ideally we should draw all
   * the translucent tiles together.
   */
  if (flags & TILE_F_TRANS50)
    SDL_SetAlpha(surf, SDL_SRCALPHA, 128);

  sdlgl_dirty_matrix_blit(sdlgl_matrix, surf, &trect, &drect, 0,
      win->scr_depth);

  if (flags & TILE_F_TRANS50)
    SDL_SetAlpha(surf, SDL_SRCALPHA, 255);
}

static void draw_iso_border_tile(struct TileWindow *win, int x, int y)
{
  struct TileSet *set = win->set;

  SDL_Rect trect, drect;
  SDL_Surface *surf;

  int sx = win->scr_x - win->pan_x + x * win->scale_w + y * win->scale_skew;
  int sy = win->scr_y - win->pan_y + y * win->scale_h;
  int tile;

  assert(set);

  /* SDL coordinates are y-inverted from OpenGL coordinates */
  drect.x = sx;
  drect.y = sdlgl_surf->h - sy - win->scale_h;
  drect.w = win->scale_full_w;
  drect.h = win->scale_full_h;

  /* ignore tiles that are not dirty */
  if (sdlgl_dirty_matrix_test(sdlgl_matrix, drect.x, drect.y,
      drect.w, drect.h, win->scr_depth) == 0)
  {
    return;
  }

  if (x > 0 && x < win->total_w - 1)
    tile = 1;
  else if (y > 0 && y < win->total_h - 1)
    tile = 2;
  else
    tile = 0;
 
  trect.x = tile * 48;
  trect.y = 0;
  trect.w = 48;
  trect.h = 64;

  surf = set->borders;

  if (set->borders_small && set->tile_h / 2 == win->scale_full_h)
  {
    surf = set->borders_small;

    trect.x /= 2; trect.y /= 2; trect.w /= 2; trect.h /= 2;
  }

  sdlgl_dirty_matrix_blit(sdlgl_matrix, surf, &trect, &drect, 0,
      win->scr_depth);
}

static void sw_draw_border(struct TileWindow *win, rgbcol_t col)
{
  int x1, y1, x2, y2;
  int cx1, cy1, cx2, cy2;
  int depth = win->scr_depth;

  int sides = 0x1111;   /* L, R, B, T */

  Uint32 c;

  /* handle isometric tileset specially, since the dirty matrix code
   * doesn't support drawing diagonal lines (would *really* mess it
   * up).
   */
  if (win->set->borders)
  {
    for (x1 = 0; x1 < win->total_w; x1++)
    {
      draw_iso_border_tile(win, x1, 0);
      draw_iso_border_tile(win, x1, win->total_h - 1);
    }

    for (y1 = 0; y1 < win->total_h; y1++)
    {
      draw_iso_border_tile(win, 0, y1);
      draw_iso_border_tile(win, win->total_w - 1, y1);
    }
    return;
  }

  x1 = win->scr_x - win->pan_x;
  y1 = win->scr_y - win->pan_y;

  x2 = x1 + win->total_w * win->scale_w - 1;
  y2 = y1 + win->total_h * win->scale_h - 1;

  /* clip box to on-screen window */

  cx1 = win->scr_x;
  cy1 = win->scr_y;
  cx2 = win->scr_x + win->scr_w - 1;
  cy2 = win->scr_y + win->scr_h - 1;

  if (x1 < cx1) x1 = cx1, sides &= ~0x1000;
  if (x2 > cx2) x2 = cx2, sides &= ~0x0100;
  if (y1 < cy1) y1 = cy1, sides &= ~0x0010;
  if (y2 > cy2) y2 = cy2, sides &= ~0x0001;

  if (x1 >= x2 || y1 >= y2 || sides == 0)
    return;

  assert(cx1 <= x1 && x1 <= x2 && x2 <= cx2);
  assert(cy1 <= y1 && y1 <= y2 && y2 <= cy2);

  c = rgb2sdl(sdlgl_surf->format, col);

  y1 = sdlgl_height - 1 - y1;
  y2 = sdlgl_height - 1 - y2;

  if (sides & 0x1000) draw_line(x1, y1, x1, y2, c, depth);
  if (sides & 0x0100) draw_line(x2, y1, x2, y2, c, depth);
  if (sides & 0x0010) draw_line(x1, y1, x2, y1, c, depth);
  if (sides & 0x0001) draw_line(x1, y2, x2, y2, c, depth);
}

static void sw_start_fading(int max_w, int min_y)
{
  SDL_PixelFormat *fmt = sdlgl_surf->format;

  assert(! darkness);

  if ((sdlgl_surf->flags & SDL_HWPALETTE) && sdlgl_depth == 8)
  {
    return;
  }
  else if (fmt->BitsPerPixel <= 8)
  {
    /* SDL doesn't support 8 bpp -> 8 bpp alpha blits.
     */
    darkness = SDL_CreateRGBSurface(SDL_SWSURFACE,
        max_w, sdlgl_height - min_y, 15,
        0x1F << 10, 0x1F << 5, 0x1F, 0);
  }
  else
  {
    darkness = SDL_CreateRGBSurface(SDL_SWSURFACE,
        max_w, sdlgl_height - min_y, fmt->BitsPerPixel,
        fmt->Rmask, fmt->Gmask, fmt->Bmask, 0);
  }

  if (! darkness)
  {
    sdlgl_error("Could not allocate SDL surface to apply fading.\n");
    return; /* NOT REACHED */
  }

  SDL_FillRect(darkness, NULL, SDL_MapRGB(fmt, 0, 0, 0));
}

static void sw_draw_fading(float fade_amount)
{
  SDL_Rect drect;
 
  if ((sdlgl_surf->flags & SDL_HWPALETTE) && sdlgl_depth == 8)
  {
    /* use the palette trick for fading (much smoother) */

    SDL_Color colors[256];

    int mul = 256 - (int)(fade_amount * 256);
    int i;

    for (i=0; i < 256; i++)
    {
      /* we apply gamma _after_ linear interpolation (correct in
       * theory, but in practice ?).
       */
      int r = (RGB_RED(sdlgl_palette[i]) * mul) >> 8;
      int g = (RGB_GRN(sdlgl_palette[i]) * mul) >> 8;
      int b = (RGB_BLU(sdlgl_palette[i]) * mul) >> 8;

      colors[i].r = GAMMA(r);
      colors[i].g = GAMMA(g);
      colors[i].b = GAMMA(b);
    }

    SDL_SetPalette(sdlgl_surf, SDL_PHYSPAL, colors, 0, 256);

    return;
  }

  assert(darkness);

  SDL_SetAlpha(darkness, SDL_SRCALPHA, (int)(fade_amount * 255));

  drect.x = 0; drect.y = 0;
  drect.w = darkness->w;
  drect.h = darkness->h;

  sdlgl_dirty_matrix_blit(sdlgl_matrix, darkness, NULL, &drect, 0, 0);
}

static void sw_finish_fading(void)
{
  if ((sdlgl_surf->flags & SDL_HWPALETTE) && sdlgl_depth == 8)
  {
    /* first cause screen to be cleared (unmapped windows) */
    sdlgl_flush();

    sdlgl_set_surface_colors(sdlgl_surf);
    return;
  }
  
  SDL_FreeSurface(darkness);
  darkness = NULL;
}

void sw_make_screenshot(const char *prefix)
{
  /* FIXME: S/W screenshots */

  sdlgl_warning("Screenshots not yet supported in software mode.\n");
}


/* ---------------------------------------------------------------- */

static void sw_set_pan(struct TileWindow *win, int x, int y)
{
  win->pan_x = x;
  win->pan_y = y;

  sdlgl_mark_dirty(win->scr_x, win->scr_y, win->scr_w, win->scr_h,
      win->scr_depth);
}
 
static void sw_set_new_pos(struct TileWindow *win, int x, int y, 
    int w, int h)
{
  int join_x, join_y;
  int join_w, join_h;

  assert(w > 0 && h > 0);
  assert(win->scr_depth > 0);

  join_x = min(x, win->scr_x);
  join_y = min(y, win->scr_y);
  join_w = max(x+w, win->scr_x + win->scr_w) - join_x;
  join_h = max(y+h, win->scr_y + win->scr_h) - join_y;

  win->scr_x = x;
  win->scr_y = y;
  win->scr_w = w;
  win->scr_h = h;
 
  sdlgl_mark_dirty(join_x, join_y, join_w, join_h, 0);
}
 
static void sw_mark_dirty(int x, int y, int w, int h, int depth)
{
  int x2 = x + w;  /* Note: not inclusive */
  int y2 = y + h;

  /* clip to screen */

  x = max(x, 0);
  y = max(y, 0);

  x2 = min(x2, sdlgl_width);
  y2 = min(y2, sdlgl_height);

  if (x2 <= x || y2 <= y)
    return;
  
  w = x2 - x;
  h = y2 - y;

  /* convert from GL to SDL coords */
  y = sdlgl_height - y - h;

  sdlgl_dirty_matrix_add(sdlgl_matrix, x, y, w, h, depth);
}


/* ---------------------------------------------------------------- */

/* Rendering interface table.
 */
struct rendering_procs sdlgl_softw_rend_procs =
{
  sw_rend_startup,
  sw_rend_shutdown,
  sw_load_tileset,
  sw_free_tileset,
  sw_create_extra_graphics,
  sw_free_extra_shapes,
  sw_enable_clipper,
  sw_disable_clipper,
  sw_blit_frame,
  sw_draw_background,
  sw_draw_extra_shape,
  sw_draw_cursor,
  sw_begin_tile_draw,
  sw_draw_tile,
  sw_finish_tile_draw,
  sw_draw_border,
  sw_start_fading,
  sw_draw_fading,
  sw_finish_fading,
  sw_make_screenshot,
  sw_set_pan,
  sw_set_new_pos,
  sw_mark_dirty
};


#endif /* SDL_GRAPHICS */
/*gl_softw.c*/
