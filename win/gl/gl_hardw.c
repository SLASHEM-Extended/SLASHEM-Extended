/* Copyright (C) 2002 Andrew Apted <ajapted@users.sourceforge.net> */
/* NetHack may be freely redistributed.  See license for details.  */

/*
 * SDL/GL window port for NetHack & Slash'EM.
 *
 * Hardware accelerated drawing code (using OpenGL).
 */

#include "hack.h"
#include "patchlevel.h"

#if defined(GL_GRAPHICS)  /* hardware only */

#define WINGL_INTERNAL
#include "winGL.h"


#define TILE_PAD  2

#define RIP_ALPHA  0.64

static GLuint pet_mark_id = 0;
static GLuint ridden_mark_id = 0;

static rgbcol_t fade_color = RGB_MAKE(0, 0, 0);


void hw_rend_startup(void)
{
  sdlgl_unit_startup();
}

void hw_rend_shutdown(void)
{
  sdlgl_unit_shutdown();
}


/* ---------------------------------------------------------------- */

static GH_INLINE void copy_border_pix(unsigned char *tex_dat,
    int dx, int dy, int ex, int ey, int e_stride, int tw, int th)
{
  int sx = 1 + max(0, min(tw - 1, dx - 1));
  int sy = 1 + max(0, min(th - 1, dy - 1));

  unsigned char *src = tex_dat +
    (ey+th+TILE_PAD-1-sy) * e_stride + (ex+sx) * 4;

  unsigned char *dest = tex_dat +
    (ey+th+TILE_PAD-1-dy) * e_stride + (ex+dx) * 4;

  dest[0] = src[0];  dest[1] = src[1];
  dest[2] = src[2];  dest[3] = src[3];
}

static void copy_one_tile(struct TileSet *set, int is_text,
    const unsigned char *image_dat, int sx, int sy, int s_stride,
    unsigned char *tex_dat, int ex, int ey, int e_stride,
    unsigned char *has_alpha)
{
  int dx, dy;
  int alpha_flag = 0;

  int tw = set->tile_w;
  int th = set->tile_h;

  for (dy = 0; dy < th; dy++)
  for (dx = 0; dx < tw; dx++)
  {
    const unsigned char *src = image_dat +
        (sy+dy) * s_stride + (sx+dx) * 4;

    unsigned char *dst = tex_dat + 
        (ey+th+TILE_PAD-1-(dy+1)) * e_stride + (ex+(dx+1)) * 4;

    /* copy RGBA colors, handling alpha */
    if (src[3] < 128)
    {
      dst[0] = dst[1] = dst[2] = 0;
      dst[3] = 0;

      alpha_flag = 1;
    }
    else
    {
      dst[0] = GAMMA(src[0]);
      dst[1] = GAMMA(src[1]);
      dst[2] = GAMMA(src[2]);
      dst[3] = 255;
    }
  }

  /* for text tiles, we simply leave the padding blank.
   * for graphical tiles, we copy the border pixels to the padding.
   */
  if (! is_text)
  {
    for (dy=0; dy <= (th + 1); dy += (th + 1))
    for (dx=0; dx < (tw + TILE_PAD); dx++)
    {
      copy_border_pix(tex_dat, dx, dy, ex, ey, e_stride, tw, th);
    }

    for (dx=0; dx <= (tw + 1); dx += (tw + 1))
    for (dy=1; dy < (th + TILE_PAD - 1); dy++)
    {
      copy_border_pix(tex_dat, dx, dy, ex, ey, e_stride, tw, th);
    }
  }

  (*has_alpha) = (unsigned char) alpha_flag;
}

GLuint sdlgl_send_graphic_RGBA(unsigned char *data,
    int width, int height)
{
  GLuint id;

  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, 
               GL_UNSIGNED_BYTE, data);
  
  return id;
}

/* 
 * Note: assumes image_dat is 4 bytes per pixel (RGBA)
 */
static void create_tile_texture(struct TileSet *set, int is_text,
    const unsigned char *image_dat, int image_w, int image_h, 
    unsigned char *tex_dat, int num_w, int num_h, int n) 
{
  int dx, dy;
  int pack_total = set->pack_w * set->pack_h;

  GLuint id;

  /* clear texture, mainly for the last-page case */
  memset(tex_dat, 0, set->tex_size_w * set->tex_size_h * 4);

  for (dy = 0; dy < set->pack_h; dy++)
  for (dx = 0; dx < set->pack_w; dx++)
  {
    int tile = n * pack_total + (dy * set->pack_w) + dx;

    /* source location in image_dat */
    int sx = (tile % num_w) * set->tile_w;
    int sy = (tile / num_w) * set->tile_h;

    /* end location in tex_dat */
    int ex = dx * (set->tile_w + TILE_PAD);
    int ey = dy * (set->tile_h + TILE_PAD);

    if (tile < set->tile_num)
    {
      copy_one_tile(set, is_text, image_dat, sx, sy, image_w * 4,
          tex_dat, ex, ey, set->tex_size_w * 4, set->has_alpha + tile);
    }
  }

  id = sdlgl_send_graphic_RGBA(tex_dat, set->tex_size_w, set->tex_size_h);

#if 0  /* Test the new image */
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_QUADS);
  glTexCoord2f(0,0); glVertex2i(0,0);
  glTexCoord2f(0,1); glVertex2i(0,300);
  glTexCoord2f(1,1); glVertex2i(300,300);
  glTexCoord2f(1,0); glVertex2i(300,0);
  glEnd();
  SDL_GL_SwapBuffers();
  sleep(10);
#endif

  set->tex_ids[n] = id;
}
 
static struct TileSet *hw_load_tileset(const char *filename, 
    int tile_w, int tile_h, int is_text, int keep_rgba,
    int *across, int *down)
{
  struct TileSet *set;

  unsigned char *image_dat;
  unsigned char *tex_dat;

  int n, image_w, image_h;
  int num_w, num_h, pack_total;

  glEnable(GL_TEXTURE_2D);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  
  image_dat = sdlgl_load_png_file(filename, &image_w, &image_h);

  if (! image_dat)
  {
    sdlgl_error("Could not load tileset %s\n", filename);
    return NULL; /* NOT REACHED */
  }

  /* check size */
  if (image_h < tile_h || image_w < tile_w ||
      (image_h % tile_h) != 0 || (image_w % tile_w) != 0)
  {
    free(image_dat);
    sdlgl_error("PNG file `%s' has bad dimensions (total %dx%d) "
        "(tile %dx%d)\n", filename, image_w, image_h, tile_w, tile_h);
    return NULL; /* NOT REACHED */
  }

  num_w = image_w / tile_w;
  num_h = image_h / tile_h;

  set = (struct TileSet *) alloc(sizeof(struct TileSet));
  memset(set, 0, sizeof(struct TileSet));

  set->tile_num = num_h * num_w;
  set->tile_w = tile_w;
  set->tile_h = tile_h;
  
  set->lap_w = (tile_h == 64) ? 16 : 0;
  set->lap_h = (tile_h == 64) ? 32 : 0;
  set->lap_skew = (tile_h == 64) ? 16 : 0;

  set->tex_size_w = sdlgl_tex_max;
  set->tex_size_h = sdlgl_tex_max;
 
  /* tiles are packed into 256x256 GL textures */
  set->pack_w = set->tex_size_w / (tile_w + TILE_PAD);
  set->pack_h = set->tex_size_h / (tile_h + TILE_PAD);
  pack_total = set->pack_w * set->pack_h;

  assert(set->pack_w > 0 && set->pack_h > 0);

  set->tex_num = (set->tile_num + pack_total - 1) / pack_total;
  assert(set->tex_num > 0);

  /* shrink texture size if possible */
  if (set->tex_num == 1)
  {
    int rows = (set->tile_num + set->pack_w - 1) / set->pack_w;

    while (rows * (tile_h + TILE_PAD) <= (set->tex_size_h / 2))
      set->tex_size_h /= 2;

    set->pack_h = set->tex_size_h / (tile_h + TILE_PAD);

    pack_total = set->pack_w * set->pack_h;
    assert(1 == (set->tile_num + pack_total - 1) / pack_total);
  }

  /* allocate temp texture space */
  tex_dat = (unsigned char *) alloc(set->tex_size_w * set->tex_size_h * 4);

  set->tex_ids = (GLuint *) alloc(set->tex_num * sizeof(GLuint));
  set->has_alpha = (unsigned char *) alloc(set->tile_num);

  /* create textures & send to the GL */
  for (n=0; n < set->tex_num; n++)
  {
    create_tile_texture(set, is_text, image_dat, image_w, image_h, 
        tex_dat, num_w, num_h, n);
  }

  /* all done, free stuff */
  free(image_dat);
  free(tex_dat);

  glDisable(GL_TEXTURE_2D);

#if 0  /* DEBUGGING */
  sdlgl_warning("%s (%dx%d) loaded from %s\n", is_text ? "FONT" : 
      "TILESET", set->tile_w, set->tile_h, filename);
#endif

  if (across) 
    (*across) = num_w;

  if (down) 
    (*down) = num_h;

  return set;
}

static void hw_free_tileset(struct TileSet *set)
{
  if (set->tex_ids)
  {
    glDeleteTextures(set->tex_num, set->tex_ids);
    free(set->tex_ids);
    set->tex_ids = NULL;
  }

  if (set->has_alpha)
  {
    free(set->has_alpha);
    set->has_alpha = NULL;
  }

  free(set);
}

/* ---------------------------------------------------------------- */


static void hw_create_extra_graphics(void)
{
  int x, y;
  unsigned char pet_dat[8 * 8 * 4];
  
  assert(pet_mark_id == 0);

  glEnable(GL_TEXTURE_2D);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  /* create the normal pet mark (8x8) */

  for (y=0; y < 8; y++)
  for (x=0; x < 8; x++)
  {
    int pix = pet_mark_bits[7 - y] & (1 << (7-x));

    pet_dat[(y*8 + x) * 4 + 0] = pix ? 255 : 0;
    pet_dat[(y*8 + x) * 4 + 1] = pix ?   0 : 0;
    pet_dat[(y*8 + x) * 4 + 2] = pix ?   0 : 0;
    pet_dat[(y*8 + x) * 4 + 3] = pix ? 255 : 0;
  }

  pet_mark_id = sdlgl_send_graphic_RGBA(pet_dat, 8, 8);

  /* create the ridden monster mark (8x8) */

  for (y=0; y < 8; y++)
  for (x=0; x < 8; x++)
  {
    int pix = ridden_mark_bits[7 - y] & (1 << (7-x));

    pet_dat[(y*8 + x) * 4 + 0] = pix ?   0 : 0;
    pet_dat[(y*8 + x) * 4 + 1] = pix ? 224 : 0;
    pet_dat[(y*8 + x) * 4 + 2] = pix ?   0 : 0;
    pet_dat[(y*8 + x) * 4 + 3] = pix ? 255 : 0;
  }

  ridden_mark_id = sdlgl_send_graphic_RGBA(pet_dat, 8, 8);

  glDisable(GL_TEXTURE_2D);
}

static void hw_free_extra_shapes(void)
{
  if (pet_mark_id)
    glDeleteTextures(1, &pet_mark_id);
  pet_mark_id = 0;

  if (ridden_mark_id)
    glDeleteTextures(1, &ridden_mark_id);
  ridden_mark_id = 0;
}

/* ---------------------------------------------------------------- */


static void hw_enable_clipper(int x, int y, int w, int h)
{
  glEnable(GL_SCISSOR_TEST);
  glScissor(x, y, w, h);
}

static void hw_disable_clipper(void)
{
  glDisable(GL_SCISSOR_TEST);
}

#if 0  /* JAIL debugging */
static void debug_jail(void)
{
  if (sdlgl_map_win != WIN_ERR)
  {
    struct TextWindow *win;
    struct TileWindow *base;

    win = text_wins[sdlgl_map_win];
    
    if (win && ((base = win->base)))
    {
      int jw = base->scr_w * sdlgl_jail_size / 200;
      int jh = base->scr_h * sdlgl_jail_size / 200;

      int mx = base->scr_x + base->scr_w / 2 + win->jail_x;
      int my = base->scr_y + base->scr_h / 2 + win->jail_y;

      glColor3f(0.0, 1.0, 0.0);

      glBegin(GL_LINE_LOOP);
      glVertex2i(mx-jw, my-jh);
      glVertex2i(mx-jw, my+jh);
      glVertex2i(mx+jw, my+jh);
      glVertex2i(mx+jw, my-jh);
      glEnd();
    }
  }
}
#endif


static void hw_blit_frame(void)
{
  SDL_GL_SwapBuffers();
}

/* ---------------------------------------------------------------- */

  
#define BG_STEP  32

static void hw_set_rgb_color(rgbcol_t color, float trans)
{
  glColor4f(GAMMA_F(RGB_RED(color)), GAMMA_F(RGB_GRN(color)),
      GAMMA_F(RGB_BLU(color)), trans);
}

static void hw_draw_background(int sx, int sy, int sw, int sh,
    rgbcol_t color, int start_idx, int depth)
{
  int x, y;
  int bx, by, bw, bh;
  
  /* fill background color */
  hw_set_rgb_color(color, 1);

  glBegin(GL_QUADS);
  
  /* break rectangle into 32x32 blocks, skipping any blocks that are
   * obscured by higher windows.  Note how we do this _inside_ the
   * glBegin/glEnd pair -- this is very important for speed.
   */

  for (y=0; y < sh; y += BG_STEP)
  for (x=0; x < sw; x += BG_STEP)
  {
    bx = sx + x;
    by = sy + y;

    bw = min(sw - x, BG_STEP);
    bh = min(sh - y, BG_STEP);

    /* the for loop prevents zero sizes */
    assert(bw > 0 && bh > 0);

    if (! sdlgl_test_tile_visible(start_idx, bx, by, bw, bh))
      continue;

    glVertex2i(bx, by);
    glVertex2i(bx, by+bh);
    glVertex2i(bx+bw, by+bh);
    glVertex2i(bx+bw, by);
  }

  glEnd();
}

static void hw_draw_extra_shape(struct TileWindow *win, 
    struct ExtraShape *shape)
{
  int sx, sy, sw, sh;
  int size;

  sw = win->scale_w;
  sh = win->scale_h;

  sx = win->scr_x - win->pan_x + shape->x * sw + shape->y * win->scale_skew;
  sy = win->scr_y - win->pan_y + shape->y * sh;
   
  /* trivial clipping */
  if (sx + sw <= win->scr_x || sx >= win->scr_x + win->scr_w)
    return;

  if (sy + sh * 2 <= win->scr_y || sy >= win->scr_y + win->scr_h)
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
      sx += win->scale_full_w * (shape->par1 ? 1 : 3) / 4;

      size = (win->scale_h <= 10) ? 6 : 8;

      if (win->scale_h > 24)
        size += (win->scale_h - 24) / 4;

      sy += (win->scale_full_h - sh) * 3 / 5;

      if (win->scale_h < 16)
        sy += 4;

      sx -= size / 2;

      glEnable(GL_TEXTURE_2D);

      if (shape->type == SHAPE_Ridden)
        glBindTexture(GL_TEXTURE_2D, ridden_mark_id);
      else
        glBindTexture(GL_TEXTURE_2D, pet_mark_id);

      glColor3f(1.0, 1.0, 1.0);
      glEnable(GL_BLEND);

      glBegin(GL_QUADS);
      glTexCoord2f(0.01, 0.01);  glVertex2i(sx, sy+sh-size);
      glTexCoord2f(0.01, 0.99);  glVertex2i(sx, sy+sh);
      glTexCoord2f(0.99, 0.99);  glVertex2i(sx+size, sy+sh);
      glTexCoord2f(0.99, 0.01);  glVertex2i(sx+size, sy+sh-size);
      glEnd();
      
      glDisable(GL_TEXTURE_2D);
      glDisable(GL_BLEND);
      break;
  }
}

static void hw_draw_cursor(struct TileWindow *win)
{
  int x = win->curs_x;
  int y = win->curs_y;
  int w = win->curs_w;

  rgbcol_t col = win->curs_color;

  int sx, sy, sw, sh;
  
  assert(x >= 0 && y >= 0 && w > 0);

  sw = win->scale_w;
  sh = win->scale_h;

  sx = win->scr_x - win->pan_x + x * sw + y * win->scale_skew;
  sy = win->scr_y - win->pan_y + y * sh;
   
  sw = sw * (w - 1) + win->scale_full_w;
  sh = win->scale_full_h;

  /* trivial clipping */
  if (sx + sw <= win->scr_x || sx >= win->scr_x + win->scr_w)
    return;

  if (sy + sh <= win->scr_y || sy >= win->scr_y + win->scr_h)
    return;

  hw_set_rgb_color(col, 1);

  if (win->curs_block)
  {
    glBegin(GL_QUADS);
    glVertex2i(sx, sy);
    glVertex2i(sx, sy+sh);
    glVertex2i(sx+sw, sy+sh);
    glVertex2i(sx+sw, sy);
    glEnd();
  }
  else
  {
    glBegin(GL_LINE_LOOP);
    glVertex2i(sx, sy);
    glVertex2i(sx, sy+sh);
    glVertex2i(sx+sw, sy+sh);
    glVertex2i(sx+sw, sy);
    glEnd();
  }
}

static void hw_draw_border(struct TileWindow *win, rgbcol_t col)
{
  int sx, sy, sw, sh;
  int skew;
  
  sw = win->total_w * win->scale_w;
  sh = win->total_h * win->scale_h;
  skew = win->total_h * win->scale_skew;

  sx = win->scr_x - win->pan_x;
  sy = win->scr_y - win->pan_y;
   
  hw_set_rgb_color(col, 1);

  glBegin(GL_LINE_LOOP);
  glVertex2i(sx, sy);
  glVertex2i(sx+skew, sy+sh);
  glVertex2i(sx+skew+sw, sy+sh);
  glVertex2i(sx+sw, sy);
  glEnd();
}

static void hw_begin_tile_draw(int blending, int overlap)
{
  sdlgl_begin_units(blending, overlap);
}

static void hw_finish_tile_draw(void)
{
  sdlgl_finish_units();
}

static void hw_draw_tile(struct TileWindow *win, int sx, int sy,
    int sw, int sh, tileidx_t tile, tilecol_t tilecol, 
    tileflags_t flags, short layer)
{
  struct TileSet *set = win->set;
  rgbcol_t color;

  int bx, by, til_idx;
  float tx1, ty1, tx2, ty2;
  float fudge = 0.1;

  int tw = set->tile_w + TILE_PAD;
  int th = set->tile_h + TILE_PAD;

  float tsize_w = set->tex_size_w;
  float tsize_h = set->tex_size_h;

  assert(set);
  assert(tile < set->tile_num);
 
  /* ignore spaces */
  if (win->is_text && TILE_2_CHAR(tile) == (unsigned char)' ')
    return;

  til_idx = tile / (set->pack_w * set->pack_h);
  tile %= (set->pack_w * set->pack_h);

  bx = tile % set->pack_w;
  by = tile / set->pack_w;

  /* a little fudging to get the pixels we want */
  tx1 = ((bx + 0) * tw + 1 + fudge) / tsize_w;
  ty1 = ((by + 0) * th + 1 + fudge) / tsize_h;
  tx2 = ((bx + 1) * tw - 1 - fudge) / tsize_w;
  ty2 = ((by + 1) * th - 1 - fudge) / tsize_h;

  if (flags & TILE_F_FLIPX)
  {
    float tmp = tx1; tx1 = tx2; tx2 = tmp;
  }
 
  if (win->is_text)
    color = sdlgl_text_colors[tilecol & 0xFF];
  else
    color = RGB_MAKE(255, 255, 255);

  sdlgl_add_unit(set->tex_ids[til_idx], tx1, ty1, tx2, ty2,
      sx, sy, sw, sh, layer, color, 
      win->see_through ? RIP_ALPHA : 
      (flags & TILE_F_TRANS50) ? 0.5 : 1);
}

static void hw_start_fading(int max_w, int min_y)
{
  /* not needed */
}

static void hw_finish_fading(void)
{
  /* not needed */
}
 
static void hw_draw_fading(float fade_amount)
{
  glEnable(GL_BLEND);

  hw_set_rgb_color(fade_color, fade_amount);

  glBegin(GL_QUADS);

  glVertex2i(0, 0);
  glVertex2i(0, sdlgl_height);
  glVertex2i(sdlgl_width, sdlgl_height);
  glVertex2i(sdlgl_width, 0);

  glEnd();
  glDisable(GL_BLEND);
}


/* ---------------------------------------------------------------- */

static void hw_set_pan(struct TileWindow *win, int x, int y)
{
  win->pan_x = x;
  win->pan_y = y;
}
 
static void hw_set_new_pos(struct TileWindow *win, int x, int y, 
    int w, int h)
{
  assert(w > 0 && h > 0);
  assert(win->scr_depth > 0);

  win->scr_x = x;
  win->scr_y = y;
  win->scr_w = w;
  win->scr_h = h;
}

static void hw_mark_dirty(int x, int y, int w, int h, int depth)
{
  /* nothing needed */
}


/* ---------------------------------------------------------------- */

/* Rendering interface table.
 */
struct rendering_procs sdlgl_hardw_rend_procs =
{
  hw_rend_startup,
  hw_rend_shutdown,
  hw_load_tileset,
  hw_free_tileset,
  hw_create_extra_graphics,
  hw_free_extra_shapes,
  hw_enable_clipper,
  hw_disable_clipper,
  hw_blit_frame,
  hw_draw_background,
  hw_draw_extra_shape,
  hw_draw_cursor,
  hw_begin_tile_draw,
  hw_draw_tile,
  hw_finish_tile_draw,
  hw_draw_border,
  hw_start_fading,
  hw_draw_fading,
  hw_finish_fading,
  sdlgl_hw_make_screenshot,
  hw_set_pan,
  hw_set_new_pos,
  hw_mark_dirty
};

#endif /* GL_GRAPHICS */
/*gl_hardw.c*/
