/* Copyright (C) 2002 Andrew Apted <ajapted@users.sourceforge.net> */
/* NetHack may be freely redistributed.  See license for details.  */

/*
 * SDL/GL window port for NetHack & Slash'EM.
 *
 * Font management.
 */

#include "hack.h"
#include "patchlevel.h"

#if defined(GL_GRAPHICS) || defined(SDL_GRAPHICS)

#define WINGL_INTERNAL
#include "winGL.h"


/* colors used for text.  Index 0 guaranteed to be black */
rgbcol_t sdlgl_text_colors[256];

/* actual tilesets */
struct TileSet *sdlgl_font_8  = NULL;  /* always loaded */
struct TileSet *sdlgl_font_14 = NULL;
struct TileSet *sdlgl_font_20 = NULL;
struct TileSet *sdlgl_font_22 = NULL;

/* handles used by the rest of the code */
struct TileSet *sdlgl_font_map  = NULL;
struct TileSet *sdlgl_font_text = NULL;
struct TileSet *sdlgl_font_menu = NULL;
struct TileSet *sdlgl_font_message = NULL;
struct TileSet *sdlgl_font_status  = NULL;

/* mapping from NetHack (terminal) colors */
tilecol_t termcolor_to_tilecol[CLR_MAX] =
{
  BLUE,  /* Note: not BLACK ! */
  RED, GREEN, BROWN, BLUE, MAGENTA, CYAN, GREY, L_GREY,
  B_ORANGE, B_GREEN, B_YELLOW, B_BLUE, B_MAGENTA, B_CYAN, WHITE
};


static struct TileSet *load_font(int size)
{
  switch (size)
  {
    case 8:
      if (! sdlgl_font_8)
        sdlgl_font_8 = sdlgl_load_tileset("glfont8.png", 8,8, 1,0,
            NULL,NULL);
      return sdlgl_font_8;

    case 14:
      if (! sdlgl_font_14)
        sdlgl_font_14 = sdlgl_load_tileset("glfont14.png", 8,14, 1,0,
            NULL,NULL);
      return sdlgl_font_14;

    case 20:
      if (! sdlgl_font_20)
        sdlgl_font_20 = sdlgl_load_tileset("glfont20.png", 10,20, 1,0,
            NULL,NULL);
      return sdlgl_font_20;

    case 22:
      if (! sdlgl_font_22)
        sdlgl_font_22 = sdlgl_load_tileset("glfont22.png", 12,22, 1,0,
            NULL,NULL);
      return sdlgl_font_22;

    default:
      sdlgl_error("INTERNAL ERROR: bad font size %d\n", size);
  }

  return NULL;  /* not reached */
}


void sdlgl_font_startup(void)
{
  int i;

  static int text_intensities[5] = { 0, 64, 136, 208, 255 };

  /* initialise text colors */
  for (i=0; i < 125; i++)
  {
    int r = text_intensities[TXT_GET_R(i)];
    int g = text_intensities[TXT_GET_G(i)];
    int b = text_intensities[TXT_GET_B(i)];

    sdlgl_text_colors[i] = RGB_MAKE(r,g,b);
  }

  /* load the font sets */

  load_font(8);  /* the 8x8 font is always loaded */

  sdlgl_font_map  = load_font(iflags.wc_fontsiz_map);
  sdlgl_font_text = load_font(iflags.wc_fontsiz_text);
  sdlgl_font_menu = load_font(iflags.wc_fontsiz_menu);
  sdlgl_font_message = load_font(iflags.wc_fontsiz_message);
  sdlgl_font_status  = load_font(iflags.wc_fontsiz_status);
 
  assert(sdlgl_font_8);
  assert(sdlgl_font_map);
  assert(sdlgl_font_text && sdlgl_font_menu);
  assert(sdlgl_font_message && sdlgl_font_status);
}


void sdlgl_font_shutdown(void)
{
  if (sdlgl_font_8)
  {
    sdlgl_free_tileset(sdlgl_font_8);
    sdlgl_font_8 = NULL;
  }
  
  if (sdlgl_font_14)
  {
    sdlgl_free_tileset(sdlgl_font_14);
    sdlgl_font_14 = NULL;
  }

  if (sdlgl_font_20)
  {
    sdlgl_free_tileset(sdlgl_font_20);
    sdlgl_font_20 = NULL;
  }

  if (sdlgl_font_22)
  {
    sdlgl_free_tileset(sdlgl_font_22);
    sdlgl_font_22 = NULL;
  }

  sdlgl_font_map = NULL;
  sdlgl_font_text = sdlgl_font_menu = NULL;
  sdlgl_font_message = sdlgl_font_status = NULL;
}


int sdlgl_quantize_font(int size)
{
  if (size <= 10)
    return 8;
  else if (size <= 16)
    return 14;
  else if (size <= 21)
    return 20;
  else
    return 22;
}


struct TileSet *sdlgl_next_narrower_font(int cur_h)
{
  if (cur_h > 20 && sdlgl_font_20)
    return sdlgl_font_20;
   
  if (sdlgl_font_14)
    return sdlgl_font_14;
   
  return sdlgl_font_8;
}
 

#endif /* GL_GRAPHICS */
/*gl_font.c*/
