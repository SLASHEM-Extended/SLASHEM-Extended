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

#if defined(GL_GRAPHICS) || defined(SDL_GRAPHICS)

#define WINGL_INTERNAL
#include "winGL.h"


unsigned char pet_mark_bits[8] =
{
  0x00,  /* oooooooo */
  0x6c,  /* oMMoMMoo */
  0xfe,  /* MMMMMMMo */
  0xfe,  /* MMMMMMMo */
  0x7c,  /* oMMMMMoo */
  0x38,  /* ooMMMooo */
  0x10,  /* oooMoooo */
  0x00   /* oooooooo */
};

unsigned char ridden_mark_bits[8] =
{
  0x00,  /* oooooooo */
  0x6c,  /* oMMoMMoo */
  0xfe,  /* MMMMMMMo */
  0xfe,  /* MMMMMMMo */
  0x7c,  /* oMMMMMoo */
  0x38,  /* ooMMMooo */
  0x10,  /* oooMoooo */
  0x00   /* oooooooo */
};


/*
 * These tables show which way the monsters face in the 16x16 and
 * 32x32 tilesets: `L' for left, `R' for right, otherwise no
 * particular direction.
 * -AJA- !!! FIXME: use external file.
 */

char tile_16_face_dirs[400] =
  "LLLLLLLLLLLLLLLLLLLLLLLLLLLL..L...LLLLLLL.....L..L"
  "LL..L.L...........RRR...........LLLLLLLLLLLL.LLLL."
  ".LLLLLL......LLLLLL..LLR.L...L.LLLLLLLLLLLLLLLLLLL"
  "LLLL.........................LL....LLLL....RL..LLL"
  "LLLLL....LLL...LLLLLL..........LLL............L..L"
  "L...........................LL..........L...RR..L."
  "L..L...L.L........L.LLLLLLLLLLLLLL................"
  ".......................L..........................";

char tile_32_face_dirs[400] =
  "LLLLLL...LLLLLLLLLLLLLLLLLLL......LLLLLLLLRR..L.LR"
  "RL....L...RRRR....LLLR.RRRRRR...LLLLLLLLLL.LLLLLL."
  ".LLLLLL......RR...L..LRRRL.R.LLLLLLLLLLLLLLLLLLLLL"
  "LLLLL.LLL....................LLL.LL............RRR"
  "RRRRR........LL.LLLLLLLLLRR...L.L.LRRL.R...R.LL..L"
  "RL..........L.LL.....LR.....RRRRRR....L.R.L.R...LL"
  "LRL.RR.RRRRLRRRRRRL.LLLLL.LLLLLLLR........LL.L...R"
  "L..LRL..L.L.L.L....LL..LL.L.........L.L...........";


/* ---------------------------------------------------------------- */


int sdlgl_quantize_tile_size(int size)
{
  return (size < 24) ? 16 : 32;
}

int sdlgl_mon_tile_face_dir(tileidx_t fg)
{
#ifdef VANILLA_GLHACK
  char dir = '.';

  if (fg >= 400)
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
#endif

  return 0;
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
  fp = fopen_datafile(filename, "rb", FALSE);
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
