/* Copyright (C) 2002 Andrew Apted <ajapted@users.sourceforge.net> */
/* NetHack may be freely redistributed.  See license for details.  */

/*
 * SDL/GL window port for NetHack & Slash'EM.
 *
 * Software rendering utilities.
 */

#include "hack.h"
#include "patchlevel.h"

#if defined (SDL_GRAPHICS)  /* software only */

#define WINGL_INTERNAL
#include "winGL.h"

 
#define SHRINK_FAST  0

#define COLOR_CACHE_SIZE  512
#define COLOR_HASH(r,g,b)  \
    ((g) + (r) * 11 + (b) * 41)

struct ColorCacheEntry
{
  /* index into sdlgl_palette[], or -1 if unused */
  short index;
  
  /* color that was matched at the index */
  rgbcol_t color;
};


/* this palette contains the colors used in the 32x32 tileset, and
 * includes the 16 colors used in the 16x16 tileset.
 *
 * the first two colors are black and white, for somewhat arcane
 * reasons (to minimise color-flashing under an 8-bit X server).
 */
rgbcol_t sdlgl_palette[256] =
{
  0x000000, 0xffffff,

#ifdef VANILLA_GLHACK
  0x000010, 0x000020, 0x000030,
  0x000040, 0x000060, 0x000080, 0x0000a0, 0x0000c0,
  0x0000e0, 0x0000ff, 0x001000, 0x001010, 0x001800,
  0x001818, 0x002000, 0x002020, 0x002040, 0x003000,
  0x003018, 0x003030, 0x003060, 0x004000, 0x004080,
  0x004020, 0x004040, 0x0050a0, 0x006000, 0x0060c0,
  0x006030, 0x006060, 0x0070e0, 0x008000, 0x008080,
  0x0080ff, 0x008040, 0x009100, 0x00a000, 0x00a0a0,
  0x00a050, 0x00b6ff, 0x00c000, 0x00c0c0, 0x00c060,
  0x00e000, 0x00e070, 0x00e0e0, 0x00fdfd, 0x00ff00,
  0x00ff80, 0x100000, 0x100010, 0x101000, 0x101010,
  0x180000, 0x181800, 0x181830, 0x183000, 0x183018,
  0x183030, 0x200000, 0x200020, 0x201000, 0x202020,
  0x204000, 0x204020, 0x204040, 0x2a4141, 0x300000,
  0x300030, 0x300c00, 0x301800, 0x301818, 0x302400,
  0x303000, 0x303018, 0x303030, 0x303060, 0x306000,
  0x306030, 0x306060, 0x395858, 0x400000, 0x400080,
  0x400020, 0x400040, 0x408000, 0x408080, 0x408040,
  0x401000, 0x402000, 0x402020, 0x402040, 0x403000,
  0x404000, 0x404080, 0x404020, 0x404040, 0x476c6c,
  0x5000a0, 0x50a000, 0x50a0a0, 0x50a050, 0x5050a0,
  0x505050, 0x600000, 0x6000c0, 0x600030, 0x600060,
  0x608080, 0x608060, 0x60c000, 0x60c0c0, 0x60c060,
  0x601800, 0x603000, 0x603030, 0x603060, 0x604800,
  0x606000, 0x606080, 0x6060c0, 0x606030, 0x606060,
  0x64a0a0, 0x68541f, 0x6c91b6, 0x6cff00, 0x7000e0,
  0x707070, 0x7070e0, 0x70e000, 0x70e070, 0x70e0e0,
  0x756958, 0x7878a0, 0x78a078, 0x78a0a0, 0x7f5a48,
  0x800000, 0x800040, 0x800080, 0x8000ff, 0x802000,
  0x804000, 0x804080, 0x804040, 0x806000, 0x806080,
  0x806060, 0x807020, 0x808000, 0x808080, 0x8080ff,
  0x808040, 0x808060, 0x80ff00, 0x80ff80, 0x80ffff,
  0x8e1000, 0x909090, 0x9090c0, 0x90c090, 0x90c0c0,
  0x91876e, 0x914700, 0xa00000, 0xa000a0, 0xa00050,
  0xa07800, 0xa07878, 0xa078a0, 0xa0a000, 0xa0a078,
  0xa0a0a0, 0xa0a050, 0xa02800, 0xa05000, 0xa050a0,
  0xa05050, 0xa5765e, 0xa8a8e0, 0xa8e0a8, 0xa8e0e0,
  0xaa8834, 0xab9a81, 0xb0b0b0, 0xb64700, 0xc00000,
  0xc000c0, 0xc00060, 0xc09000, 0xc09090, 0xc090c0,
  0xc0a040, 0xc0c000, 0xc0c090, 0xc0c0c0, 0xc0c0ff,
  0xc0c060, 0xc0ffc0, 0xc0ffff, 0xc03000, 0xc06000,
  0xc060c0, 0xc06060, 0xc91700, 0xca8f72, 0xcc4f00,
  0xd0a850, 0xd0d0d0, 0xd57600, 0xdadab6, 0xe00000,
  0xe00070, 0xe000e0, 0xe07000, 0xe07070, 0xe070e0,
  0xe0a800, 0xe0a8a8, 0xe0a8e0, 0xe0c0a0, 0xe0e000,
  0xe0e070, 0xe0e0a8, 0xe0e0e0, 0xe03800, 0xe5a282,
  0xec9100, 0xf0f0f0, 0xf2c44d, 0xf8e060, 0xfcfc99,
  0xff0000, 0xff0080, 0xff00ff, 0xff1e00, 0xff4000,
  0xff6c00, 0xff8000, 0xff8080, 0xff8777, 0xffb691,
  0xffc000, 0xffc0c0, 0xffc0ff, 0xffd3af, 0xffff00,
  0xffff80, 0xffffc0,
  0x102030, 0x301800, 0x282828,  /* bkg colors (unused by tiles) */
  0x000000,  /* 255 = transparent color (0x00ffff) */

#else  /* Slash'EM */
  0x000010, 0x000020, 0x000030,
  0x000040, 0x000060, 0x000080, 0x0000a0, 0x0000c0,
  0x0000e0, 0x0000ff, 0x001000, 0x001010, 0x001800,
  0x001818, 0x001830, 0x002000, 0x002020, 0x002040,
  0x003000, 0x003018, 0x003030, 0x003060, 0x004000,
  0x004020, 0x004040, 0x004080, 0x0050a0, 0x006000,
  0x006030, 0x006060, 0x0060c0, 0x0070e0, 0x008000,
  0x008040, 0x008080, 0x0080ff, 0x009000, 0x00a000,
  0x00a050, 0x00a0a0, 0x00b6ff, 0x00c000, 0x00c060,
  0x00c0c0, 0x00e000, 0x00e070, 0x00e0e0, 0x00ff00,
  0x00ff80, 0x00ffff, 0x100000, 0x100010, 0x101000,
  0x101010, 0x103030, 0x180000, 0x181800, 0x181830,
  0x183000, 0x183018, 0x183030, 0x200000, 0x200020,
  0x202000, 0x202020, 0x204000, 0x204020, 0x204040,
  0x300000, 0x300030, 0x300c00, 0x301800, 0x301818,
  0x302400, 0x303000, 0x303018, 0x303030, 0x303060,
  0x306000, 0x306030, 0x306060, 0x400000, 0x400020,
  0x400040, 0x400080, 0x401000, 0x402000, 0x402020,
  0x402040, 0x403000, 0x404000, 0x404020, 0x404040,
  0x404080, 0x408000, 0x408040, 0x408080, 0x486c6c,
  0x5000a0, 0x505050, 0x5050a0, 0x50a000, 0x50a050,
  0x50a0a0, 0x600000, 0x600030, 0x600060, 0x6000c0,
  0x601800, 0x603000, 0x603030, 0x603060, 0x604800,
  0x606000, 0x606030, 0x606060, 0x606080, 0x6060c0,
  0x608060, 0x608080, 0x60c000, 0x60c060, 0x60c0c0,
  0x6090b2, 0x68541f, 0x6c90b6, 0x6cff00, 0x7000e0,
  0x707070, 0x7070e0, 0x70a0a0, 0x70e000, 0x70e070,
  0x70e0e0, 0x756958, 0x7878a0, 0x78a078, 0x78a0a0,
  0x800000, 0x800040, 0x800080, 0x8000ff, 0x802000,
  0x804000, 0x804040, 0x804080, 0x805a48, 0x806000,
  0x806060, 0x806080, 0x807020, 0x808000, 0x808040,
  0x808060, 0x808080, 0x8080ff, 0x80ff00, 0x80ff80,
  0x80ffff, 0x904800, 0x90876e, 0x909090, 0x9090c0,
  0x90c090, 0x90c0c0, 0xa00000, 0xa00050, 0xa000a0,
  0xa02800, 0xa05000, 0xa05050, 0xa050a0, 0xa07800,
  0xa07878, 0xa078a0, 0xa0a000, 0xa0a050, 0xa0a078,
  0xa0a0a0, 0xa0a0e0, 0xa57660, 0xa8a8e0, 0xa8e0a8,
  0xa8e0e0, 0xaa8834, 0xaa9a80, 0xb0b0b0, 0xb24000,
  0xb64800, 0xc00000, 0xc00060, 0xc000c0, 0xc03000,
  0xc06000, 0xc06060, 0xc060c0, 0xc09000, 0xc09090,
  0xc090c0, 0xc0a040, 0xc0c000, 0xc0c060, 0xc0c090,
  0xc0c0c0, 0xc0c0ff, 0xc0ffc0, 0xc0ffff, 0xca9072,
  0xd0a850, 0xd0d0d0, 0xd2d2b2, 0xdadab6, 0xe00000,
  0xe00070, 0xe000e0, 0xe03800, 0xe07000, 0xe07070,
  0xe070e0, 0xe0a800, 0xe0a8a8, 0xe0a8e0, 0xe0c0a0,
  0xe0e000, 0xe0e070, 0xe0e0a8, 0xe0e0e0, 0xe5a282,
  0xf0f0f0, 0xf20000, 0xf26000, 0xf2c44d, 0xf8e060,
  0xfcfc99, 0xff0000, 0xff0080, 0xff00ff, 0xff4000,
  0xff6c00, 0xff8000, 0xff8080, 0xff80ff, 0xffb690,
  0xffc000, 0xffc0c0, 0xffc0ff, 0xffd3b0, 0xffff00,
  0xffff80, 0xffffc0,
  0x102030, 0x301800, 0x282828,  /* bkg colors (unused by tiles) */
  0x000000,  /* 255 = transparent color (0x476c6c) */
#endif
};

static struct ColorCacheEntry color_cache[COLOR_CACHE_SIZE] =
{ { -1, 0 }, };


void sdlgl_set_surface_colors(SDL_Surface *surf)
{
  int i;
  SDL_Color colors[256];

  for (i=0; i < 256; i++)
  {
    colors[i].r = GAMMA(RGB_RED(sdlgl_palette[i]));
    colors[i].g = GAMMA(RGB_GRN(sdlgl_palette[i]));
    colors[i].b = GAMMA(RGB_BLU(sdlgl_palette[i]));
  }

  SDL_SetColors(surf, colors, 0, 256);
}

static int fast_find_color(int r, int g, int b)
{
  rgbcol_t rgb = RGB_MAKE(r, g, b);  

  int hash = COLOR_HASH(r, g, b) % COLOR_CACHE_SIZE;

  int c = color_cache[hash].index;

  if (c >= 0 && color_cache[hash].color == rgb)
  {
    /* cache hit */
    return c;
  }

  /* cache miss -- find required index */
  {
    int best = 0;
    int best_dist = 1 << 30;

    for (c=0; c < 256; c++)
    {
      int dr, dg, db, dist;

      if (c == TRANS_PIX)
        continue;

      dr = r - RGB_RED(sdlgl_palette[c]);
      dg = g - RGB_GRN(sdlgl_palette[c]);
      db = b - RGB_BLU(sdlgl_palette[c]);

      dist = (dr * dr) + (dg * dg) + (db * db);

      if (dist == 0)
      {
        best = c;
        break;
      }

      if (dist < best_dist)
      {
        best = c;
        best_dist = dist;
      }
    }

    c = best;
  }
 
  color_cache[hash].index = c;
  color_cache[hash].color = rgb;

  return c;
}


/*------------------------------------------------------------------------*/

static GH_INLINE void setpixel(SDL_Surface *s, int x, int y, Uint32 c)
{
  int bpp = s->format->BytesPerPixel;
  Uint8 *p = (Uint8 *)s->pixels + y * s->pitch + x * bpp;

  switch(bpp)
  {
    case 1:
      *p = c;
      break;
    
    case 2:
      *(Uint16 *)p = c;
      break;
    
    case 3:
      if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
      {
        p[0] = (c >> 16) & 0xff;
        p[1] = (c >> 8)  & 0xff;
        p[2] = c & 0xff;
      } 
      else
      {
        p[0] = c & 0xff;
        p[1] = (c >> 8)  & 0xff;
        p[2] = (c >> 16) & 0xff;
      }
      break;
    
    case 4:
      *(Uint32 *)p = c;
      break;
    
    default:
      sdlgl_error("Bit depth not one of (8,16,24,32) (depth: %d)\n", bpp);
      break; /* NOT REACHED */
  }
}

static GH_INLINE Uint32 getpixel(SDL_Surface *s, int x, int y)
{
  int bpp = s->format->BytesPerPixel;
  Uint8 *p = (Uint8 *)s->pixels + y * s->pitch + x * bpp;

  switch(bpp)
  {
    case 1:
      return *p;
    
    case 2:
      return *(Uint16 *)p;
    
    case 3:
      if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
        return (p[0] << 16) | (p[1] << 8) | p[2];
      else
        return (p[2] << 16) | (p[1] << 8) | p[0];

    case 4:
      return *(Uint32 *)p;
      
    default:
      sdlgl_error("Bit depth not one of (8,16,24,32) (depth: %d)\n", bpp);
      return 0; /* NOT REACHED */
  }
}

SDL_Surface *sdlgl_RGBA_to_truecolor(unsigned char *data,
    int width, int height)
{
  SDL_Surface *surf;

  Uint32 r, g, b, a;
 
  if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
  {
    r = 0xFF000000;
    g = 0x00FF0000;
    b = 0x0000FF00;
    a = 0x000000FF;
  }
  else
  {
    r = 0x000000FF;
    g = 0x0000FF00;
    b = 0x00FF0000;
    a = 0xFF000000;
  }
  
  surf = SDL_CreateRGBSurfaceFrom(data, width, height, 32, 4 * width,
      r, g, b, a);

  if (! surf)
    return NULL;

  /* Since SDL_CreateRGBSurfaceFrom merely copies the data pointer, we
   * must not free it.  Instead, we'll tell SDL to free it.
   * NOTE: this is *not* a public API.
   */
  surf->flags &= ~SDL_PREALLOC;

  return surf;
}

SDL_Surface *sdlgl_RGBA_to_palettised(unsigned char *data,
    int width, int height)
{
  int x, y;

  SDL_Surface *surf = SDL_CreateRGBSurface(SDL_SWSURFACE,
      width, height, 8, 0, 0, 0, 0);

  if (! surf)
    return NULL;

  sdlgl_set_surface_colors(surf);
  SDL_SetColorKey(surf, SDL_SRCCOLORKEY, TRANS_PIX);
  
  if (SDL_MUSTLOCK(surf))
    SDL_LockSurface(surf);
 
  for (y=0; y < height; y++)
  for (x=0; x < width;  x++)
  {
    unsigned char *src = data + (y * width * 4) + (x * 4);

    int pix = (src[3] < 128) ? TRANS_PIX : 
        fast_find_color(src[0], src[1], src[2]);
     
    setpixel(surf, x, y, pix);
  }

  if (SDL_MUSTLOCK(surf))
    SDL_UnlockSurface(surf);
 
  return surf;
}

SDL_Surface *sdlgl_shrink_surface(SDL_Surface *src)
{
  int x, y;

  SDL_Surface *dest = SDL_CreateRGBSurface(SDL_SWSURFACE,
      src->w / 2, src->h / 2, 8, 0, 0, 0, 0);

  if (! dest)
    return NULL;

  /* Only works with 8 bit surfaces.
   */
  assert(src->format->BytesPerPixel == 1);

  sdlgl_set_surface_colors(dest);
  SDL_SetColorKey(dest, SDL_SRCCOLORKEY, TRANS_PIX);
  
  assert((src->w % 2) == 0);
  assert((src->h % 2) == 0);

  if (SDL_MUSTLOCK(src))
    SDL_LockSurface(src);

  if (SDL_MUSTLOCK(dest))
    SDL_LockSurface(dest);
 
#if SHRINK_FAST
  {
    for (y=0; y < src->h; y += 2)
    for (x=0; x < src->w; x += 2)
    {
      setpixel(dest, x/2, y/2, getpixel(src, x, y));
    }
  }
#else  /* slow but nicer */
  {
    for (y=0; y < src->h; y += 2)
    for (x=0; x < src->w; x += 2)
    {
      /* compute average of the four pixels */

      int sub;
      
      Uint8 r, g, b;
      Uint32 c, r_tot=0, g_tot=0, b_tot=0, a_count=0;

      for (sub=0; sub < 4; sub++)
      {
        c = getpixel(src, x + (sub & 1), y + (sub & 2) / 2);

        if (c == TRANS_PIX)
        {
          a_count++;
        }
        else
        {
          /* assert(0 <= c && c <= 255); */

          /* -AJA- Previously I used SDL_GetRGB() here, which had the
           *       unfortunate effect of apply gamma _twice_ to the
           *       resulting image.
           */
          r_tot += RGB_RED(sdlgl_palette[c]);
          g_tot += RGB_GRN(sdlgl_palette[c]);
          b_tot += RGB_BLU(sdlgl_palette[c]);
        }
      }

      if (a_count > 4/2)
      {
        setpixel(dest, x/2, y/2, TRANS_PIX);
      }
      else
      {
        r = r_tot / (4 - a_count);
        g = g_tot / (4 - a_count);
        b = b_tot / (4 - a_count);

        setpixel(dest, x/2, y/2, fast_find_color(r, g, b));
      }
    }
  }
#endif

  if (SDL_MUSTLOCK(src))
    SDL_UnlockSurface(src);

  if (SDL_MUSTLOCK(dest))
    SDL_UnlockSurface(dest);
 
  return dest;
}

void sdlgl_sw_create_has_alpha(struct TileSet *set)
{
  int tile;

  SDL_Surface *surf = set->surf;

  if (surf->format->BytesPerPixel != 1)
    return;
  
  set->has_alpha = (unsigned char *) alloc(set->tile_num);
  memset(set->has_alpha, 0, set->tile_num * sizeof(unsigned char));

  if (SDL_MUSTLOCK(surf))
    SDL_LockSurface(surf);

  for (tile = 0; tile < set->tile_num; tile++)
  {
    int x = (tile % set->pack_w) * set->tile_w;
    int y = (tile / set->pack_w) * set->tile_h;

    int dx, dy;
    int done = 0;
    
    for (dy = 0; (dy < set->tile_h) && !done; dy++)
    for (dx = 0; (dx < set->tile_w) && !done; dx++)
    {
      if (getpixel(surf, x+dx, y+dy) == TRANS_PIX)
      {
        set->has_alpha[tile] = 1;
        done = 1;
      }
    }
  }

  if (SDL_MUSTLOCK(surf))
    SDL_UnlockSurface(surf);
}


/*------------------------------------------------------------------------*/

void sdlgl_create_font_cache(struct TileSet *set)
{
  int i;
  struct FontCache *cache;

  cache = (struct FontCache *) alloc(sizeof(struct FontCache));

  cache->char_num = FONTCACHE_SIZE;
  cache->pack_w = FONTCACHE_PACK_W;
  cache->pack_h = (cache->char_num + cache->pack_w - 1) / cache->pack_w;
  
  cache->char_surf = SDL_CreateRGBSurface(SDL_SWSURFACE,
      cache->pack_w * set->tile_w, cache->pack_h * set->tile_h,
      8, 0, 0, 0, 0);

  if (! cache->char_surf)
  {
    sdlgl_error("Failed to create SDL surface for font cache.\n");
    /* NOT REACHED */
  }

  sdlgl_set_surface_colors(cache->char_surf);
  SDL_SetColorKey(cache->char_surf, SDL_SRCCOLORKEY, TRANS_PIX);

  cache->what_cols = (tilecol_t *) alloc(cache->char_num *
      sizeof(tilecol_t));

  for (i=0; i < cache->char_num; i++)
    cache->what_cols[i] = TILECOL_UNUSED;
  
  set->font_cache = cache;
}

void sdlgl_free_font_cache(struct TileSet *set)
{
  assert(set->font_cache);
  assert(set->font_cache->char_surf);
  assert(set->font_cache->what_cols);

  SDL_FreeSurface(set->font_cache->char_surf);
  free(set->font_cache->what_cols);

  free(set->font_cache);
  set->font_cache = NULL;
}

void sdlgl_font_cache_lookup(struct TileSet *set, tileidx_t ch, 
    tilecol_t color, int *pos_x, int *pos_y)
{
  struct FontCache *cache = set->font_cache;
  
  int hash = FONTCACHE_HASH(ch,color) % cache->char_num;

  assert(hash >= 0);

  (*pos_x) = hash % cache->pack_w;
  (*pos_y) = hash / cache->pack_w;

  assert((*pos_y) < cache->pack_h);
   
  if (cache->what_cols[hash] == color)
  {
    /* cache hit -- do nothing */
    return;
  }

  /* cache miss -- replace tile with colored font char */
  {
    int x, y;

    int tw = set->tile_w;
    int th = set->tile_h;

    int src_x = (ch % set->pack_w) * tw;
    int src_y = (ch / set->pack_w) * th;

    int dest_x = (*pos_x) * tw;
    int dest_y = (*pos_y) * th;

    rgbcol_t rgb = sdlgl_text_colors[color];

    Uint32 c = SDL_MapRGB(cache->char_surf->format,
        GAMMA(RGB_RED(rgb)), GAMMA(RGB_GRN(rgb)), GAMMA(RGB_BLU(rgb)));
    
    assert(src_y/th < set->pack_h);
    
    if (SDL_MUSTLOCK(set->surf))
      SDL_LockSurface(set->surf);

    if (SDL_MUSTLOCK(cache->char_surf))
      SDL_LockSurface(cache->char_surf);

    for (y=0; y < th; y++)
    for (x=0; x < tw; x++)
    {
      Uint32 pix = getpixel(set->surf, src_x + x, src_y + y);
      
      if (pix != TRANS_PIX)
        pix = c;

      setpixel(cache->char_surf, dest_x + x, dest_y + y, pix);
    }

    if (SDL_MUSTLOCK(set->surf))
      SDL_UnlockSurface(set->surf);

    if (SDL_MUSTLOCK(cache->char_surf))
      SDL_UnlockSurface(cache->char_surf);
  }

  /* fix up cache ! */
  cache->what_cols[hash] = color;
}


/*------------------------------------------------------------------------*/

struct DirtyMatrix *sdlgl_create_dirty_matrix(int pw, int ph)
{
  struct DirtyMatrix *mat;

  mat = (struct DirtyMatrix *) alloc(sizeof(struct DirtyMatrix));

  mat->pw = pw;
  mat->ph = ph;

  mat->cw = (pw + DIRTY_SIZE - 1) / DIRTY_SIZE;
  mat->ch = (ph + DIRTY_SIZE - 1) / DIRTY_SIZE;

  assert(mat->cw > 1);
  assert(mat->ch > 1);

  mat->cells = (unsigned char *) alloc(mat->cw * mat->ch);

  /* initial dirty matrix should be fully dirty, so that initial
   * background gets drawn & blitted.
   */
  memset(mat->cells, 0 /* depth */, mat->cw * mat->ch);

  /* create the updating rectangles */
  
  mat->max_rects = ((mat->cw + 1) / 2) * mat->ch;
  mat->updaters = (SDL_Rect *) alloc(mat->max_rects * sizeof(SDL_Rect));

  return mat;
}

void sdlgl_free_dirty_matrix(struct DirtyMatrix *mat)
{
  free(mat->updaters);
  mat->updaters = NULL;

  free(mat->cells);
  mat->cells = NULL;

  free(mat);
}

void sdlgl_dirty_matrix_clear(struct DirtyMatrix *mat)
{
  int x, y;

  for (y=0; y < mat->ch; y++)
  for (x=0; x < mat->cw; x++)
  {
    mat->cells[y * mat->cw + x] = CLEAN_CELL;
  }
}

void sdlgl_dirty_matrix_add(struct DirtyMatrix *mat, int x, int y,
      int w, int h, int depth)
{
  int dx1, dy1;
  int dx2, dy2;

  /* clip coords */
  if (x < 0) { w += x; x = 0; }
  if (y < 0) { h += y; y = 0; }

  if (x + w >= mat->pw) { w = mat->pw - x; }
  if (y + h >= mat->ph) { h = mat->ph - y; }

  if (w <= 0 || h <= 0)
    return;

  dx1 = x / DIRTY_SIZE;
  dy1 = y / DIRTY_SIZE;

  dx2 = (x + w - 1) / DIRTY_SIZE;
  dy2 = (y + h - 1) / DIRTY_SIZE;
  
  for (y = dy1; y <= dy2; y++)
  for (x = dx1; x <= dx2; x++)
  {
    unsigned char *cell = mat->cells + (y * mat->cw) + x;

    if (*cell > depth)
      *cell = depth;
  }
}

int sdlgl_dirty_matrix_test(struct DirtyMatrix *mat, int x, int y,
      int w, int h, int depth)
{
  int dx1, dy1;
  int dx2, dy2;

  /* clip coords */
  if (x < 0) { w += x; x = 0; }
  if (y < 0) { h += y; y = 0; }

  if (x + w >= mat->pw) { w = mat->pw - x; }
  if (y + h >= mat->ph) { h = mat->ph - y; }

  if (w <= 0 || h <= 0)
    return 0;  /* clean */

  dx1 = x / DIRTY_SIZE;
  dy1 = y / DIRTY_SIZE;

  dx2 = (x + w - 1) / DIRTY_SIZE;
  dy2 = (y + h - 1) / DIRTY_SIZE;

  for (y = dy1; y <= dy2; y++)
  for (x = dx1; x <= dx2; x++)
  {
    unsigned char *cell = mat->cells + (y * mat->cw) + x;

    if (*cell <= depth)
      return 1;  /* dirty */
  }

  return 0;  /* clean */
}

/* returns the number of rectangles.
 */
int sdlgl_dirty_matrix_to_updaters(struct DirtyMatrix *mat)
{
  int num = 0;
  int x, y, w;
  int pix_w, pix_h;

  for (y = 0; y < mat->ph; y += DIRTY_SIZE)
  for (x = 0; x < mat->pw; x += w * DIRTY_SIZE)
  {
    w = 1;

    /* NOTE: Now 1-to-1, so it could read the matrix directly...
     */
    if (sdlgl_dirty_matrix_test(mat, x, y, 
          DIRTY_SIZE, DIRTY_SIZE, CLEAN_CELL-1) == 0)
    {
      continue;
    }

    /* find longest horizontal run */
    for (;;)
    {
      int x2 = x + w * DIRTY_SIZE;
      
      if (x2 >= mat->pw)
        break;

      if (sdlgl_dirty_matrix_test(mat, x2, y, 
            w * DIRTY_SIZE, DIRTY_SIZE, CLEAN_CELL-1) == 0)
      {
        break;
      }

      w += 1;
    }
    
    assert(num < mat->max_rects);
    
    pix_w = w * DIRTY_SIZE;
    pix_h = 1 * DIRTY_SIZE;

    if (x + pix_w >= mat->pw)
      pix_w = mat->pw - x;

    if (y + pix_h >= mat->ph)
      pix_h = mat->ph - y;

    assert(pix_w > 0 && pix_h > 0);

    mat->updaters[num].x = x;
    mat->updaters[num].y = y;
    mat->updaters[num].w = pix_w;
    mat->updaters[num].h = pix_h;

    num++;
  }

  return num;
}


/*------------------------------------------------------------------------*/

static GH_INLINE void low_level_blit(int sx, int sy, SDL_Surface *src, 
    int dx, int dy, int dw, int dh, Uint32 col,
    int px, int py, int pw, int ph)
{
  SDL_Rect srect, drect;

  dx += px; dy += py;
  dw -= px; dh -= py;

  assert(dw > 0 && dh > 0);
  assert(pw > 0 && ph > 0);

  drect.x = dx;
  drect.y = dy;
  drect.w = min(dw, pw);
  drect.h = min(dh, ph);

  if (src)
  {
    srect.x = sx;
    srect.y = sy;
    srect.w = drect.w;
    srect.h = drect.h;

    SDL_BlitSurface(src, &srect, sdlgl_surf, &drect);
  }
  else
  {
    SDL_FillRect(sdlgl_surf, &drect, col);
  }
}

void sdlgl_dirty_matrix_blit(struct DirtyMatrix *mat, SDL_Surface *src,
    SDL_Rect *srect, SDL_Rect *drect, Uint32 col, int depth)
{
  /* This function is the ONLY function that draws into the screen
   * surface.  When `src' is NULL, it fills with the given color,
   * otherwise we are blitting from the source surface.  Both cases
   * will limit drawing to areas marked as dirty.
   * 
   * Algorithm:
   *
   * 1. Clip to matrix size.  Abort if result is empty.
   *
   * 2. Compute dirty area.  Abort if fully clean.  If fully dirty,
   *    apply blit and return early.
   *
   * 3. Complex blit: iterate over dirty area.  For each dirty
   *    cell, set SDL's clipper and apply blit.
   */
 
  int sx, sy;          /* source rect */
  int dx, dy, dw, dh;  /* dest rect */

  int dx1, dy1, dx2, dy2;  /* dirty area */

  dx = drect->x; dy = drect->y;
  dw = drect->w; dh = drect->h;

  if (srect)
  {
    sx = srect->x; sy = srect->y;
    dw = srect->w; dh = srect->h;
  }
  else if (src)
  {
    sx = 0; sy = 0;
    dw = src->w; dh = src->h;
  }
  else
  {
    sx = sy = 0;  /* not needed */
  }
  
  /* --- clipping --- */

  if (dx < 0)
  {
    dw += dx;
    sx -= dx;
    dx = 0;
  }

  if (dy < 0)
  {
    dh += dy;
    sy -= dy;
    dy = 0;
  }

  if (dx + dw >= mat->pw)
  {
    dw = mat->pw - dx;
  }

  if (dy + dh >= mat->ph)
  {
    dh = mat->ph - dy;
  }

  if (dw <= 0 || dh <= 0)
    return;

  /* --- compute dirty area --- */
  {
    int cx, cy;  /* cell */

    int dirty_num = 0;
    int total;

    dx1 = dx / DIRTY_SIZE;
    dy1 = dy / DIRTY_SIZE;

    dx2 = (dx + dw - 1) / DIRTY_SIZE;
    dy2 = (dy + dh - 1) / DIRTY_SIZE;
    
    assert(0 <= dx1 && dx1 <= dx2 && dx2 < mat->cw);
    assert(0 <= dy1 && dy1 <= dy2 && dy2 < mat->ch);

    total = (dx2 - dx1 + 1) * (dy2 - dy1 + 1);

    for (cy = dy1; cy <= dy2; cy++)
    for (cx = dx1; cx <= dx2; cx++)
    {
      if (mat->cells[cy * mat->cw + cx] <= depth)
        dirty_num++;
    }

    if (dirty_num == 0)
      return;

    if (dirty_num == total)
    {
      low_level_blit(sx, sy, src, dx, dy, dw, dh, col, 0, 0, dw, dh);
      return;
    }
  }

  /* --- do complex clipping --- */
  {
    SDL_Rect old_clip, clip;

    int cx, cy, c_len;  /* cell */
    int nw, nh;

    SDL_GetClipRect(sdlgl_surf, &old_clip);

    for (cy = dy1; cy <= dy2; cy++)
    for (cx = dx1; cx <= dx2; cx += c_len)
    {
      c_len = 1;

      if (mat->cells[cy * mat->cw + cx] > depth)
        continue;

      /* find longest horizontal run */
      for (;;)
      {
        int cx2 = cx + c_len;
      
        if (cx2 >= mat->cw)
          break;

        if (mat->cells[cy * mat->cw + cx2] > depth)
          break;

        c_len++;
      }
    
      clip.x = cx * DIRTY_SIZE;
      clip.y = cy * DIRTY_SIZE;
      
      nw = c_len * DIRTY_SIZE;
      nh = DIRTY_SIZE;

      /* compute intersection with old clip rectangle */ 

      if (clip.x < old_clip.x)
      {
        nw -= (old_clip.x - clip.x);
        clip.x = old_clip.x;
      }

      if (clip.y < old_clip.y)
      {
        nh -= (old_clip.y - clip.y);
        clip.y = old_clip.y;
      }

      if (clip.x + nw > old_clip.x + old_clip.w)
        nw = old_clip.x + old_clip.w - clip.x;

      if (clip.y + nh > old_clip.y + old_clip.h)
        nh = old_clip.y + old_clip.h - clip.y;

      if (nw <= 0 || nh <= 0)
        continue;

      clip.w = nw;
      clip.h = nh;
      
      SDL_SetClipRect(sdlgl_surf, &clip);

      low_level_blit(sx, sy, src, dx, dy, dw, dh, col, 0, 0, dw, dh);
    }

    SDL_SetClipRect(sdlgl_surf, &old_clip);
  }
}


#endif /* SDL_GRAPHICS */
/*gl_rendu.c*/
