/* Copyright (C) 2002 Andrew Apted <ajapted@users.sourceforge.net> */
/* NetHack may be freely redistributed.  See license for details.  */

/*
 * SDL/GL window port for NetHack & Slash'EM.
 *
 * Unit system.  Sorts the units (GL images) based on texture id,
 * which can give _much_ faster results.  The startup routine also
 * initialises the GL into the required state.
 */

#include "hack.h"

#if defined(GL_GRAPHICS)  /* hardware only */

#define WINGL_INTERNAL
#include "winGL.h"


int sdlgl_tex_max = 0;


#define MAX_UNITS  2048

static struct GraphicUnit unit_array[MAX_UNITS];
static short unit_map[MAX_UNITS];
static int num_units = 0;
static int unit_overlap = 0;


void sdlgl_unit_startup(void)
{
  const char *str;

  /* query some GL properties */

  str = (const char *) glGetString(GL_VERSION);
  if (! str)
  {
    sdlgl_error("OpenGL load problem ! (version string is NULL)\n");
  }

  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &sdlgl_tex_max);
  if (sdlgl_tex_max < 128)
  {
    sdlgl_error("OpenGL texture limit too small (%d).\n", sdlgl_tex_max);
  }
  if (sdlgl_tex_max > 2048)
    sdlgl_tex_max = 2048;

#if 0  /* DEBUGGING */
  str = (const char *) glGetString(GL_VERSION);
  sdlgl_warning("OpenGL Version: %s\n", str);
  
  str = (const char *) glGetString(GL_VENDOR);
  sdlgl_warning("OpenGL Vendor: %s\n", str);
  
  str = (const char *) glGetString(GL_RENDERER);
  sdlgl_warning("OpenGL Renderer: %s\n", str);

  sdlgl_warning("OpenGL Texture Size: %d\n", sdlgl_tex_max);
#endif
 
  /* initialise all important GL state */

  glDisable(GL_FOG);
  glDisable(GL_BLEND);
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_SCISSOR_TEST);
  glDisable(GL_STENCIL_TEST);

  glEnable(GL_LINE_SMOOTH);

  glDisable(GL_POLYGON_SMOOTH);
  glDisable(GL_DITHER);

  glEnable(GL_NORMALIZE);

  glFrontFace(GL_CW);
  glShadeModel(GL_SMOOTH);
  glDepthFunc(GL_LEQUAL);
  glAlphaFunc(GL_GREATER, 1.0 / 32.0);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glHint(GL_FOG_HINT, GL_FASTEST);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  /* turn off lighting stuff */
  glDisable(GL_LIGHTING);
  glDisable(GL_COLOR_MATERIAL);

  glNormal3f(0.0, 0.0, 1.0);

  /* setup the GL matrices for drawing 2D stuff */

  glViewport(0, 0, sdlgl_width, sdlgl_height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, (float)sdlgl_width, 
          0.0, (float)sdlgl_height, -1.0, 1.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  /* clear screen */
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
}

void sdlgl_unit_shutdown(void)
{
  /* does nothing */
}

/* ---------------------------------------------------------------- */


static int unit_compare(const void *p1, const void *p2)
{
  const struct GraphicUnit *A = unit_array + *((short *) p1);
  const struct GraphicUnit *B = unit_array + *((short *) p2);

  if (A->pass != B->pass)
    return A->pass - B->pass;

  if (A->tex_id != B->tex_id)
    return (int)A->tex_id - (int)B->tex_id;

  return (int)A->color - (int)B->color;
}

static void flush_units(void)
{
  int i;
  int begun = 0;
  GLuint current_tex = 0x789ABCDE;
  
  if (num_units < 1)
    return;
  
  /* sort units into increasing texture ID */
  for (i=0; i < num_units; i++)
    unit_map[i] = i;

  if (! unit_overlap)
    qsort(unit_map, num_units, sizeof(short), unit_compare);

  glEnable(GL_TEXTURE_2D);

  /* now draw all the goodies */
  for (i=0; i < num_units; i++)
  {
    struct GraphicUnit *unit = unit_array + unit_map[i];

    if (unit->tex_id != current_tex)
    {
      if (begun)
        glEnd();

      current_tex = unit->tex_id;
      glBindTexture(GL_TEXTURE_2D, current_tex);
      
      glBegin(GL_QUADS);
      begun = 1;
    }

    /* we don't bother minimising color changes */
    glColor4f(
        GAMMA_F(RGB_RED(unit->color)),
        GAMMA_F(RGB_GRN(unit->color)),
        GAMMA_F(RGB_BLU(unit->color)), unit->trans);

    glTexCoord2f(unit->tx1, unit->ty1);
    glVertex2i(unit->x, unit->y);

    glTexCoord2f(unit->tx1, unit->ty2);
    glVertex2i(unit->x, unit->y + unit->h);

    glTexCoord2f(unit->tx2, unit->ty2);
    glVertex2i(unit->x + unit->w, unit->y + unit->h);

    glTexCoord2f(unit->tx2, unit->ty1);
    glVertex2i(unit->x + unit->w, unit->y);
  }

  if (begun)
    glEnd();

  glDisable(GL_TEXTURE_2D);

  /* we've drawn 'em all... */
  num_units = 0;
}

/* ---------------------------------------------------------------- */


void sdlgl_begin_units(int blending, int overlap)
{
  assert(num_units == 0);

  if (blending)
    glEnable(GL_BLEND);

  unit_overlap = overlap;
}

void sdlgl_add_unit(GLuint id, float tx1, float ty1, 
    float tx2, float ty2, short x, short y, short w, short h,
    short pass, rgbcol_t color, float trans)
{
  struct GraphicUnit *unit;
  
  if (num_units == MAX_UNITS)
    flush_units();

  assert(num_units < MAX_UNITS);

  unit = unit_array + num_units;
  num_units++;

  unit->tex_id = id;
  unit->tx1 = tx1;  unit->ty1 = ty1;
  unit->tx2 = tx2;  unit->ty2 = ty2;
  unit->x = x;  unit->y = y;
  unit->w = w;  unit->h = h;
  unit->pass  = pass;
  unit->color = color;
  unit->trans = trans;
}
  
void sdlgl_finish_units(void)
{
  if (num_units > 0)
    flush_units();

  glDisable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  assert(num_units == 0);
}


/* ---------------------------------------------------------------- */

void sdlgl_hw_make_screenshot(const char *prefix)
{
  unsigned char *pixels;
  unsigned char *p;
  
  char filename[256];
  char msgbuf[512];

  int x, crc1, crc2;
  int success;

  pixels = (unsigned char *) alloc(sdlgl_width * sdlgl_height * 3);

  glReadBuffer(GL_FRONT);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glReadPixels(0, 0, sdlgl_width, sdlgl_height, GL_RGB, 
      GL_UNSIGNED_BYTE, pixels);

  /* compute Adler CRC (per RFC 1950) on image.
   * We'll create a fairly unique filename using it.
   */
  crc1 = crc2 = 0;
  p = pixels;

  for (x=0; x < sdlgl_width * sdlgl_height * 3; x++, p++)
  {
    crc1 = (crc1 + (int) *p) % 65521;
    crc2 = (crc2 + crc1)     % 65521;
  }

  /* save image as a PPM (Portable PixMap) file.
   */
  sprintf(filename, "%s%04X%04X.ppm", prefix, crc2, crc1);

  success = sdlgl_save_ppm_file(filename, pixels, sdlgl_width, 
      sdlgl_height);

  free(pixels);

  /* FIXME: show a pop-up window ! */
  if (success)
  {
    sprintf(msgbuf, "Screenshot saved to: %s", filename);
    Sdlgl_raw_print(msgbuf);
  }
  else
  {
    sprintf(msgbuf, "Screenshot failed to open: %s", filename);
    Sdlgl_raw_print(msgbuf);
  }
}

#endif /* GL_GRAPHICS */
/*gl_unit.c*/
