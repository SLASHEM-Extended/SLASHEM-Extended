/* Copyright (C) 2002 Andrew Apted <ajapted@users.sourceforge.net> */
/* NetHack may be freely redistributed.  See license for details.  */

/*
 * SDL/GL window port for NetHack & Slash'EM.
 *
 * Image handling, loading and saving of PNG files.  This code is in
 * a separate file to make using other image formats easier.
 */

#include "hack.h"
#include "patchlevel.h"
#include "dlb.h"

#if defined(GL_GRAPHICS) || defined(SDL_GRAPHICS)

#define WINGL_INTERNAL
#include "winGL.h"

#include <ctype.h>


unsigned char pet_mark_bits[8] =
{
  0x00,  /* ........ */
  0x6c,  /* .MM.MM.. */
  0xfe,  /* MMMMMMM. */
  0xfe,  /* MMMMMMM. */
  0x7c,  /* .MMMMM.. */
  0x38,  /* ..MMM... */
  0x10,  /* ...M.... */
  0x00   /* ........ */
};

unsigned char ridden_mark_bits[8] =
{
  0x00,  /* ........ */
  0x6c,  /* .MM.MM.. */
  0xfe,  /* MMMMMMM. */
  0xfe,  /* MMMMMMM. */
  0x7c,  /* .MMMMM.. */
  0x38,  /* ..MMM... */
  0x10,  /* ...M.... */
  0x00   /* ........ */
};

unsigned char sdlgl_gamma_table[256];

SDL_Cursor *sdlgl_cursor_main  = NULL;
SDL_Cursor *sdlgl_cursor_left  = NULL;
SDL_Cursor *sdlgl_cursor_right = NULL;
SDL_Cursor *sdlgl_cursor_up    = NULL;
SDL_Cursor *sdlgl_cursor_down  = NULL;
SDL_Cursor *sdlgl_cursor_hand  = NULL;
SDL_Cursor *sdlgl_cursor_cross = NULL;


/*
 * These tables show which way the monsters face in the 16x16 and
 * 32x32 tilesets: `L' for left, `R' for right, otherwise no
 * particular direction.  They are loaded from the glface16.lst and
 * glface32.lst files.
 */

char tile_16_face_dirs[1000] = { 0, };
char tile_32_face_dirs[1000] = { 0, };


/* ---------------------------------------------------------------- */

static unsigned char cursor_up_bits[16 * 4] =
{
  0x01, 0x00, 0x01, 0x00,  /* .......o........ */
  0x02, 0x80, 0x03, 0x80,  /* ......oMo....... */
  0x04, 0x40, 0x07, 0xC0,  /* .....oMMMo...... */
  0x08, 0x20, 0x0F, 0xE0,  /* ....oMMMMMo..... */
  0x10, 0x10, 0x1F, 0xF0,  /* ...oMMMMMMMo.... */
  0x20, 0x08, 0x3F, 0xF8,  /* ..oMMMMMMMMMo... */
  0x40, 0x04, 0x7F, 0xFC,  /* .oMMMMMMMMMMMo.. */
  0x80, 0x02, 0xFF, 0xFE,  /* oMMMMMMMMMMMMMo. */
  0x7C, 0x7C, 0x7F, 0xFC,  /* .oooooMMMooooo.. */
  0x04, 0x40, 0x07, 0xC0,  /* .....oMMMo...... */
  0x04, 0x40, 0x07, 0xC0,  /* .....oMMMo...... */
  0x04, 0x40, 0x07, 0xC0,  /* .....oMMMo...... */
  0x04, 0x40, 0x07, 0xC0,  /* .....oMMMo...... */
  0x04, 0x40, 0x07, 0xC0,  /* .....oMMMo...... */
  0x04, 0x40, 0x07, 0xC0,  /* .....oMMMo...... */
  0x02, 0x80, 0x03, 0x80   /* ......oMo....... */
};

static unsigned char cursor_down_bits[16 * 4] =
{
  0x02, 0x80, 0x03, 0x80,  /* ......oMo....... */
  0x04, 0x40, 0x07, 0xC0,  /* .....oMMMo...... */
  0x04, 0x40, 0x07, 0xC0,  /* .....oMMMo...... */
  0x04, 0x40, 0x07, 0xC0,  /* .....oMMMo...... */
  0x04, 0x40, 0x07, 0xC0,  /* .....oMMMo...... */
  0x04, 0x40, 0x07, 0xC0,  /* .....oMMMo...... */
  0x04, 0x40, 0x07, 0xC0,  /* .....oMMMo...... */
  0x7C, 0x7C, 0x7F, 0xFC,  /* .oooooMMMooooo.. */
  0x80, 0x02, 0xFF, 0xFE,  /* oMMMMMMMMMMMMMo. */
  0x40, 0x04, 0x7F, 0xFC,  /* .oMMMMMMMMMMMo.. */
  0x20, 0x08, 0x3F, 0xF8,  /* ..oMMMMMMMMMo... */
  0x10, 0x10, 0x1F, 0xF0,  /* ...oMMMMMMMo.... */
  0x08, 0x20, 0x0F, 0xE0,  /* ....oMMMMMo..... */
  0x04, 0x40, 0x07, 0xC0,  /* .....oMMMo...... */
  0x02, 0x80, 0x03, 0x80,  /* ......oMo....... */
  0x01, 0x00, 0x01, 0x00   /* .......o........ */
};

static unsigned char cursor_left_bits[16 * 4] =
{
  0x01, 0x00, 0x01, 0x00,  /* .......o........ */
  0x02, 0x80, 0x03, 0x80,  /* ......oMo....... */
  0x04, 0x80, 0x07, 0x80,  /* .....oMMo....... */
  0x08, 0x80, 0x0F, 0x80,  /* ....oMMMo....... */
  0x10, 0x80, 0x1F, 0x80,  /* ...oMMMMo....... */
  0x20, 0xFE, 0x3F, 0xFE,  /* ..oMMMMMooooooo. */
  0x40, 0x01, 0x7F, 0xFF,  /* .oMMMMMMMMMMMMMo */
  0x80, 0x00, 0xFF, 0xFF,  /* oMMMMMMMMMMMMMMM */
  0x40, 0x01, 0x7F, 0xFF,  /* .oMMMMMMMMMMMMMo */
  0x20, 0xFE, 0x3F, 0xFE,  /* ..oMMMMMooooooo. */
  0x10, 0x80, 0x1F, 0x80,  /* ...oMMMMo....... */
  0x08, 0x80, 0x0F, 0x80,  /* ....oMMMo....... */
  0x04, 0x80, 0x07, 0x80,  /* .....oMMo....... */
  0x02, 0x80, 0x03, 0x80,  /* ......oMo....... */
  0x01, 0x00, 0x01, 0x00,  /* .......o........ */
  0x00, 0x00, 0x00, 0x00   /* ................ */
};

static unsigned char cursor_right_bits[16 * 4] =
{
  0x00, 0x80, 0x00, 0x80,  /* ........o....... */
  0x01, 0x40, 0x01, 0xC0,  /* .......oMo...... */
  0x01, 0x20, 0x01, 0xE0,  /* .......oMMo..... */
  0x01, 0x10, 0x01, 0xF0,  /* .......oMMMo.... */
  0x01, 0x08, 0x01, 0xF8,  /* .......oMMMMo... */
  0x7F, 0x04, 0x7F, 0xFC,  /* .oooooooMMMMMo.. */
  0x80, 0x02, 0xFF, 0xFE,  /* oMMMMMMMMMMMMMo. */
  0x00, 0x01, 0xFF, 0xFF,  /* MMMMMMMMMMMMMMMo */
  0x80, 0x02, 0xFF, 0xFE,  /* oMMMMMMMMMMMMMo. */
  0x7F, 0x04, 0x7F, 0xFC,  /* .oooooooMMMMMo.. */
  0x01, 0x08, 0x01, 0xF8,  /* .......oMMMMo... */
  0x01, 0x10, 0x01, 0xF0,  /* .......oMMMo.... */
  0x01, 0x20, 0x01, 0xE0,  /* .......oMMo..... */
  0x01, 0x40, 0x01, 0xC0,  /* .......oMo...... */
  0x00, 0x80, 0x00, 0x80,  /* ........o....... */
  0x00, 0x00, 0x00, 0x00   /* ................ */
};

static unsigned char cursor_hand_bits[16 * 4] =
{
  0x06, 0x00, 0x06, 0x00,  /* .....oo......... */
  0x09, 0x00, 0x0F, 0x00,  /* ....oMMo........ */
  0x09, 0x00, 0x0F, 0x00,  /* ....oMMo........ */
  0x09, 0x00, 0x0F, 0x00,  /* ....oMMo........ */
  0x09, 0xC0, 0x0F, 0xC0,  /* ....oMMooo...... */
  0x09, 0x38, 0x0F, 0xF8,  /* ....oMMoMMooo... */
  0x69, 0x26, 0x6F, 0xFE,  /* .oo.oMMoMMoMMoo. */
  0x98, 0x05, 0xFF, 0xFF,  /* oMMooMMMMMMMMoMo */
  0x88, 0x01, 0xFF, 0xFF,  /* oMMMoMMMMMMMMMMo */
  0x48, 0x01, 0x7F, 0xFF,  /* .oMMoMMMMMMMMMMo */
  0x20, 0x02, 0x3F, 0xFE,  /* ..oMMMMMMMMMMMo. */
  0x20, 0x02, 0x3F, 0xFE,  /* ..oMMMMMMMMMMMo. */
  0x10, 0x04, 0x1F, 0xFC,  /* ...oMMMMMMMMMo.. */
  0x08, 0x04, 0x0F, 0xFC,  /* ....oMMMMMMMMo.. */
  0x04, 0x08, 0x07, 0xF8,  /* .....oMMMMMMo... */
  0x04, 0x08, 0x07, 0xF8   /* .....oMMMMMMo... */
};

static unsigned char cursor_cross_bits[16 * 4] =
{
  0x00, 0x00, 0x00, 0x00,  /* ................ */
  0x08, 0x10, 0x08, 0x10,  /* ....o......o.... */
  0x14, 0x28, 0x1C, 0x38,  /* ...oMo....oMo... */
  0x22, 0x44, 0x3E, 0x7C,  /* ..oMMMo..oMMMo.. */
  0x11, 0x88, 0x1F, 0xF8,  /* ...oMMMooMMMo... */
  0x08, 0x10, 0x0F, 0xF0,  /* ....oMMMMMMo.... */
  0x04, 0x20, 0x07, 0xE0,  /* .....oMMMMo..... */
  0x04, 0x20, 0x07, 0xE0,  /* .....oMMMMo..... */
  0x08, 0x10, 0x0F, 0xF0,  /* ....oMMMMMMo.... */
  0x11, 0x88, 0x1F, 0xF8,  /* ...oMMMooMMMo... */
  0x22, 0x44, 0x3E, 0x7C,  /* ..oMMMo..oMMMo.. */
  0x14, 0x28, 0x1C, 0x38,  /* ...oMo....oMo... */
  0x08, 0x10, 0x08, 0x10,  /* ....o......o.... */
  0x00, 0x00, 0x00, 0x00,  /* ................ */
  0x00, 0x00, 0x00, 0x00,  /* ................ */
  0x00, 0x00, 0x00, 0x00   /* ................ */
};


/* ---------------------------------------------------------------- */

int sdlgl_quantize_tile_size(int size)
{
  return (size < 24) ? 16 : 32;
}

int sdlgl_mon_tile_face_dir(tileidx_t fg)
{
  char dir = '.';

  if (fg >= SIZE(tile_16_face_dirs))
    return 0;
 
  if (iflags.wc_tile_height == 16)
    dir = tile_16_face_dirs[fg];
  else if (iflags.wc_tile_height == 32)
    dir = tile_32_face_dirs[fg];
  else
    return 0;
    
  if (dir == 'L')
    return -1;
  else if (dir == 'R')
    return +1;

  return 0;
}

void sdlgl_load_face_dirs(const char *filename, char *face_dirs)
{
  FILE *fp;

  int mon = 0;
  int ch;

#ifdef FILE_AREAS
  fp = fopen_datafile_area(FILE_AREA_SHARE, filename, RDTMODE, FALSE);
#else
  fp = fopen_datafile(filename, RDTMODE, FALSE);
#endif
  if (!fp)
  {
    sdlgl_error("Failed to open file: %s\n", filename);
    return;  /* NOT REACHED */
  }

  while ((ch = fgetc(fp)) != EOF)
  {
    ch = highc(ch);

    if (isspace(ch) || !isprint(ch))
      continue;

    if (ch != '.' && ch != 'L' && ch != 'R')
    {
      sdlgl_warning("Ignoring bad character `%c' in face file: %s\n",
          ch, filename);
      continue;
    }

    face_dirs[mon++] = ch;

    if (mon >= SIZE(tile_16_face_dirs))
    {
      sdlgl_warning("Too many monsters in face file: %s\n", filename);
      break;
    }
  }

  fclose(fp);
}


/* ---------------------------------------------------------------- */

/* Gamma tables.
 *    top[] = 512 * (x ** 0.5 - x)
 *    bottom[] = 512 * (x - x ** 2)
 * where x = input_value / 255.
 */
static unsigned char gamma_table_top[256] =
{
  0,30,41,49,56,61,66,70,74,78,81,84,86,89,91,94,
  96,98,99,101,103,104,106,107,108,110,111,112,113,114,115,116,
  117,117,118,119,120,120,121,121,122,122,123,123,124,124,125,125,
  125,126,126,126,126,127,127,127,127,127,127,127,127,127,127,127,
  127,127,127,127,127,127,127,127,127,127,127,127,126,126,126,126,
  126,125,125,125,125,124,124,124,124,123,123,123,122,122,122,121,
  121,121,120,120,119,119,119,118,118,117,117,116,116,115,115,114,
  114,113,113,112,112,111,111,110,110,109,109,108,108,107,106,106,
  105,105,104,103,103,102,102,101,100,100,99,98,98,97,96,96,
  95,94,94,93,92,92,91,90,90,89,88,87,87,86,85,85,
  84,83,82,82,81,80,79,79,78,77,76,75,75,74,73,72,
  71,71,70,69,68,67,67,66,65,64,63,62,62,61,60,59,
  58,57,57,56,55,54,53,52,51,50,50,49,48,47,46,45,
  44,43,42,42,41,40,39,38,37,36,35,34,33,32,31,31,
  30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,
  14,13,12,11,10,9,8,7,6,5,4,3,3,2,1,0
};

static unsigned char gamma_table_bottom[256] =
{
  0,1,3,5,7,9,11,13,15,17,19,21,22,24,26,28,
  30,31,33,35,37,38,40,42,43,45,46,48,50,51,53,54,
  56,57,59,60,62,63,64,66,67,69,70,71,73,74,75,76,
  78,79,80,81,83,84,85,86,87,88,89,91,92,93,94,95,
  96,97,98,99,100,101,101,102,103,104,105,106,107,107,108,109,
  110,110,111,112,113,113,114,115,115,116,116,117,118,118,119,119,
  120,120,121,121,122,122,122,123,123,124,124,124,125,125,125,125,
  126,126,126,126,126,127,127,127,127,127,127,127,127,127,127,127,
  127,127,127,127,127,127,127,127,127,127,127,126,126,126,126,126,
  125,125,125,125,124,124,124,123,123,122,122,122,121,121,120,120,
  119,119,118,118,117,116,116,115,115,114,113,113,112,111,110,110,
  109,108,107,107,106,105,104,103,102,101,101,100,99,98,97,96,
  95,94,93,92,91,89,88,87,86,85,84,83,81,80,79,78,
  76,75,74,73,71,70,69,67,66,64,63,62,60,59,57,56,
  54,53,51,50,48,46,45,43,42,40,38,37,35,33,31,30,
  28,26,24,22,21,19,17,15,13,11,9,7,5,3,1,0
};


void sdlgl_generate_gamma_table(void)
{
  int i, result = 0;

  for (i=0; i < 256; i++)
  {
    switch (sdlgl_gamma)
    {
      case -5: result = i - gamma_table_bottom[i] / 2; break;
      case -4: result = i - gamma_table_bottom[i] / 3; break;
      case -3: result = i - gamma_table_bottom[i] / 4; break;
      case -2: result = i - gamma_table_bottom[i] / 6; break;
      case -1: result = i - gamma_table_bottom[i] / 8; break;
      case  0: result = i; break;
      case +1: result = i + gamma_table_top[i] / 8; break;
      case +2: result = i + gamma_table_top[i] / 6; break;
      case +3: result = i + gamma_table_top[i] / 4; break;
      case +4: result = i + gamma_table_top[i] / 3; break;
      case +5: result = i + gamma_table_top[i] / 2; break;

      default:
        impossible("Bad sdlgl_gamma value %d", sdlgl_gamma);
        break;
    }

    sdlgl_gamma_table[i] = min(255, max(0, result));
  }
}
 

/* ---------------------------------------------------------------- */

static SDL_Cursor *make_sdl_cursor(unsigned char *combi)
{
  unsigned char data[256];
  unsigned char mask[256];

  int y;

  /* separate the data & mask from the combined image */
  for (y = 0; y < 16; y++)
  {
    data[y * 2 + 0] = combi[y * 4 + 0];
    data[y * 2 + 1] = combi[y * 4 + 1];
    mask[y * 2 + 0] = combi[y * 4 + 2];
    mask[y * 2 + 1] = combi[y * 4 + 3];
  }

  return SDL_CreateCursor(data, mask, 16, 16, 8, 8);
}

void sdlgl_init_mouse_cursors(void)
{
  sdlgl_cursor_main = SDL_GetCursor();

  sdlgl_cursor_up    = make_sdl_cursor(cursor_up_bits); 
  sdlgl_cursor_down  = make_sdl_cursor(cursor_down_bits); 
  sdlgl_cursor_left  = make_sdl_cursor(cursor_left_bits); 
  sdlgl_cursor_right = make_sdl_cursor(cursor_right_bits); 
  sdlgl_cursor_hand  = make_sdl_cursor(cursor_hand_bits); 
  sdlgl_cursor_cross = make_sdl_cursor(cursor_cross_bits); 
}

void sdlgl_free_mouse_cursors(void)
{
  SDL_SetCursor(sdlgl_cursor_main);
   
  /* the main cursor is made by SDL itself */
  sdlgl_cursor_main = NULL;

  if (sdlgl_cursor_up)
  {
    SDL_FreeCursor(sdlgl_cursor_up);
    sdlgl_cursor_up = NULL;
  }

  if (sdlgl_cursor_down)
  {
    SDL_FreeCursor(sdlgl_cursor_down);
    sdlgl_cursor_down = NULL;
  }

  if (sdlgl_cursor_left)
  {
    SDL_FreeCursor(sdlgl_cursor_left);
    sdlgl_cursor_left = NULL;
  }

  if (sdlgl_cursor_right)
  {
    SDL_FreeCursor(sdlgl_cursor_right);
    sdlgl_cursor_right = NULL;
  }

  if (sdlgl_cursor_hand)
  {
    SDL_FreeCursor(sdlgl_cursor_hand);
    sdlgl_cursor_hand = NULL;
  }

  if (sdlgl_cursor_cross)
  {
    SDL_FreeCursor(sdlgl_cursor_cross);
    sdlgl_cursor_cross = NULL;
  }
}


/* ---------------------------------------------------------------- */


#define CHECK_PNG_BYTES  4

unsigned char *sdlgl_load_png_file(const char *filename,
    int *image_w, int *image_h)
{
  /* -AJA- all these volatiles here may seem strange.  They are needed
   * because the ANSI C standard (which GCC adheres to) says that when
   * setjmp/longjmp is being used, only volatile local variables are
   * guaranteed to keep their state if longjmp() gets called.
   */
  FILE * volatile fp = NULL;
  unsigned char * volatile image_dat = NULL;
  png_bytep * volatile row_pointers = NULL;

  /* we take the address of these two, so we shouldn't need the
   * volatile.  (GCC complains about discarding qualifiers if the
   * volatile is there).
   */
  png_structp /*volatile*/ png_ptr = NULL;
  png_infop   /*volatile*/ info_ptr = NULL;

  char sig_buf[CHECK_PNG_BYTES];
  png_uint_32 width, height;
  int bit_depth, color_type, interlace_type;
  int row, stride;

  /* open the prospective PNG file */
#ifdef FILE_AREAS
  fp = fopen_datafile_area(FILE_AREA_SHARE, filename, RDBMODE, FALSE);
#else
  fp = fopen_datafile(filename, RDBMODE, FALSE);
#endif
  if (!fp)
  {
    sdlgl_warning("Failed to open file: %s\n", filename);
    return NULL;
  }

  /* read in some of the signature bytes */
  if (fread(sig_buf, 1, CHECK_PNG_BYTES, fp) != CHECK_PNG_BYTES)
  {
    sdlgl_warning("Failed to read from file: %s\n", filename);
    goto failed;
  }

  /* compare the first CHECK_PNG_BYTES bytes of the signature */
  if (png_sig_cmp(sig_buf, (png_size_t)0, CHECK_PNG_BYTES) != 0)
  {
    sdlgl_warning("File is not a PNG file: %s\n", filename);
    goto failed;
  }

  /* pass NULLs for the error functions -- thus use the setjump stuff
   */
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (png_ptr == NULL)
  {
    sdlgl_warning("Problem within LibPNG (no memory ?)\n");
    goto failed;
  }

  /* allocate/initialize the memory for image information */
  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL)
  {
    sdlgl_warning("Out of memory with LibPNG\n");
    goto failed;
  }

  /* set error handling since we are using the setjmp/longjmp method
   * (this is the normal method of doing things with libpng).
   */
  if (setjmp(png_ptr->jmpbuf))
  {
    sdlgl_warning("Problem within LibPNG (unknown)\n");
    goto failed;
  }

  /* set up the input control since we're using standard C streams */
  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, CHECK_PNG_BYTES);

  /* the call to png_read_info() gives us all of the information from the
   * PNG file before the first IDAT (image data chunk)
   */
  png_read_info(png_ptr, info_ptr);

  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
      &color_type, &interlace_type, NULL, NULL);

  *image_w = (int)width;
  *image_h = (int)height;

  /* tell libpng to strip 16 bit/color down to 8 bits/color */
  png_set_strip_16(png_ptr);

  /* expand paletted colors into true RGB triplets */
  if (color_type == PNG_COLOR_TYPE_PALETTE ||
      color_type == PNG_COLOR_TYPE_GRAY ||
      png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
  {
     png_set_expand(png_ptr);
  }

  /* set alpha position and filler value */
  png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);
 
  /* let all the above calls take effect */
  png_read_update_info(png_ptr, info_ptr);
  
  /* allocate the memory for the image */
  stride = png_get_rowbytes(png_ptr, info_ptr);

  image_dat = (unsigned char *) alloc(height * stride);
  row_pointers = (png_bytep *) alloc(height * sizeof(png_bytep));
   
  for (row=0; row < height; row++)
  {
     row_pointers[row] = image_dat + row * stride;
  }

  /* now read in the image.  Yeah baby ! */
  png_read_image(png_ptr, row_pointers);
  png_read_end(png_ptr, info_ptr);

  /* free stuff & close the file */
  png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
  free(row_pointers);
  fclose(fp);

  return image_dat;

  /* -AJA- Normally I don't like gotos.  In this situation where there
   * are lots of points of possible failure and a growing set of
   * things to be undone, it makes for nicer code.
   */
failed:

  if (image_dat)
    free(image_dat);
  
  if (png_ptr)
  {
    /* assume NULLs not allowed (png docs don't say, bad bad) */
    if (info_ptr)
      png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    else
      png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
  }

  if (row_pointers)
    free(row_pointers);

  if (fp)
    fclose(fp);
  
  return NULL;
}

/* ---------------------------------------------------------------- */


int sdlgl_save_ppm_file(const char *filename,
    const unsigned char *pixels, int image_w, int image_h)
{
  FILE *fp;
  int x, y;

  fp = fopen(filename, "wb");

  if (! fp)
    return 0;

  fprintf(fp, "P6\n%d %d\n255\n", image_w, image_h);

  /* the pixels are bottom up */
  for (y = image_h-1; y >= 0; y--)
  {
    const unsigned char *p = pixels + y * image_w * 3;

    for (x = 0; x < image_w; x++, p += 3)
    {
      fputc(p[0], fp);
      fputc(p[1], fp);
      fputc(p[2], fp);
    }
  }

  fclose(fp);
  return 1;
}

#endif /* GL_GRAPHICS */
/*gl_image.c*/
