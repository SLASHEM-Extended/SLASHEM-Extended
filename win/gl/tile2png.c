/* Copyright (C) 2002 Andrew Apted <ajapted@users.sourceforge.net> */
/* NetHack may be freely redistributed.  See license for details.  */

/*
 * Convert the given input tile files into the PNG file that the
 * SDL/GL windowing port can use.
 * 
 * Based on win/X11/tile2x11.c
 * 
 * TODO HERE:
 *   + Have a -t option for transparency.
 *   + When not transparent, save as RGB (instead of RGBA).
 *   + Check if < 256 colors are used, and if so, use palette.
 *   + Would be nice if it didn't require so much memory...
 */
#include "hack.h"
#include "tile.h"

/* N.H uses compress/uncompress, conflicting with ZLIB headers */
#undef compress
#undef uncompress

/* certain N.H #defines conflict with the SDL header files */
#undef red
#undef green
#undef blue

#include <png.h>
#include <assert.h>


/* [AJA] Use this value instead of MAX_GLYPH, as MAX_GLYPH is much
 *       greater than what we need (mainly due to the swallows), and
 *       for the 48x64 tileset it would need to allocate almost 200 MB
 *       of memory !
 */
#define MAX_TILE_NUM  2048

#define OUTNAME "gltiles.png"    /* Default output file name */


int ntiles = 0;
int max_rows = -1;
int saved_tile_x = -1;
int saved_tile_y = -1;

unsigned char *tile_bytes = NULL;


/* -------------------------------------------------------------------- */

void save_png(const char *filename, int width, int height)
{
  /* [AJA] all the volatiles here may seem strange.  They are needed
   *       because the ANSI C standard (which GCC adheres to) says
   *       that when setjmp/longjmp is being used, only volatile local
   *       variables are guaranteed to keep their state if longjmp()
   *       gets called.
   */
  FILE * volatile fp = NULL;

  png_bytep * volatile row_pointers = NULL;

  /* we take the address of these two, so we shouldn't need the
   * volatile.  (GCC complains about discarding qualifiers if the
   * volatile is there).
   */
  png_structp /* volatile */ png_ptr  = NULL;
  png_infop   /* volatile */ info_ptr = NULL;

  int row;

  /* open the file */
  fp = fopen(filename, "wb");
  if (fp == NULL)
  {
    fprintf(stderr, "tile2png: Can't open output file: %s\n", filename);
    goto failed;
  }

  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
      NULL, NULL, NULL);

  if (png_ptr == NULL)
  {
    fprintf(stderr, "tile2png: PNG error (out of memory ?)\n");
    goto failed;
  }

  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL)
  {
    fprintf(stderr, "tile2png: PNG error (out of memory ?)\n");
    goto failed;
  }

  /* set error handling since we are using the setjmp/longjmp method
   * (this is the normal method of doing things with libpng).
   */
  if (setjmp(png_ptr->jmpbuf))
  {
    fprintf(stderr, "tile2png: Unknown problem while writing PNG.\n");
    goto failed;
  }

  png_init_io(png_ptr, fp);

  png_set_IHDR(png_ptr, info_ptr, width, height, 8, 
      PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, 
      PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_write_info(png_ptr, info_ptr);

  row_pointers = (png_bytep *) malloc(height * sizeof(png_bytep));
  if (!row_pointers)
  {
    fprintf(stderr, "tile2png: Out of memory.\n");
    goto failed;
  }
   
  for (row=0; row < height; row++)
  {
     row_pointers[row] = tile_bytes + row * width * 4;
  }

  png_write_image(png_ptr, row_pointers);
  png_write_end(png_ptr, info_ptr);

  free(row_pointers);

  /* clean up after the write, and free any memory allocated */
  png_destroy_write_struct(&png_ptr, &info_ptr);

  fclose(fp);

  return;

  /* [AJA] Normally I don't like gotos.  In this situation where there
   *       are lots of points of possible failure and a growing set of
   *       things to be undone, it makes for nicer code.
   */
failed:
  if (png_ptr)
  {
    /* assume NULLs not allowed (png docs don't say) */
    if (info_ptr)
      png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    else
      png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
  }

  if (row_pointers)
    free(row_pointers);

  if (fp)
    fclose(fp);
}


/* -------------------------------------------------------------------- */

/* Convert the tiles in the file to our RGBA format.
 */
static int convert_tiles(void)
{
  unsigned char *tb;

  pixel tile[MAX_TILE_Y][MAX_TILE_X];
  pixel transp = DEFAULT_BACKGROUND;

  int x, y;
  int bx, by;
  int count = 0;

  int stride = tile_x * TILES_PER_ROW * 4;

  while (read_text_tile(tile)) 
  {
    count++;

    bx = ntiles % TILES_PER_ROW;
    by = ntiles / TILES_PER_ROW;
    
    tb = tile_bytes + (by * tile_y * stride + bx * tile_x * 4);
   
    for (y = 0; y < tile_y; y++, tb += stride)
    for (x = 0; x < tile_x; x++)
    {
      tb[x*4 + 0] = tile[y][x].r;
      tb[x*4 + 1] = tile[y][x].g;
      tb[x*4 + 2] = tile[y][x].b;

      tb[x*4 + 3] = tile_x < 32 ? 255 :
                    pixel_equal(tile[y][x], transp) ? 0 : 255;
    }

    ntiles++;

    if (ntiles >= MAX_TILE_NUM)
    {
      fprintf(stderr, "tile2png: Too many tiles !!\n");
      exit(1);
    }
  }

  return count;
}


/* Open the given file, read & merge the colormap, convert the tiles. 
 */
static void process_file(const char *fname)
{
  int count;

  if (!fopen_text_file(fname, RDTMODE)) 
  {
    Fprintf(stderr, "tile2png: Can't open file \"%s\"\n", fname);
    exit(1);
  }

  if (!tile_bytes) 
  {
    int size;
    
    /*
     * Delayed until we open the first input file so that
     * we know the size of the tiles we are processing.
     */
    max_rows = (MAX_TILE_NUM + TILES_PER_ROW - 1) / TILES_PER_ROW;

    size = max_rows * TILES_PER_ROW * tile_x * tile_y * 4;

    tile_bytes = malloc(size);
    if (!tile_bytes) 
    {
      Fprintf(stderr, "tile2png: Not enough memory (%d KB).\n", size/1024);
      exit(1);
    }
    
    memset(tile_bytes, 0, size);

    saved_tile_x = tile_x;
    saved_tile_y = tile_y;
  }
  count = convert_tiles();
  Fprintf(stderr, "%s: %d tiles (%dx%d)\n", fname, count, tile_x, tile_y);
  fclose_text_file();
}


int main(int argc, const char **argv)
{
  int i, argn = 1;
  int num_rows;

  const char *outname = OUTNAME;

  while (argn < argc) 
  {
    if (argn + 1 < argc && !strcmp(argv[argn], "-o")) 
    {
      outname = argv[argn + 1];
      argn += 2;
    }
    /* put extra argument checks here... */
    else
      break;
  }

  if (argn == argc) 
  {
    Fprintf(stderr, "Usage: %s [-o out_file] txt_file1 [txt_file2 ...]\n",
        argv[0]);
    exit(1);
  }

  for (i = argn; i < argc; i++)
  {
    process_file(argv[i]);
  }

  Fprintf(stderr, "Total tiles: %d\n", ntiles);

  num_rows = (ntiles + TILES_PER_ROW - 1) / TILES_PER_ROW;
  
  save_png(outname, TILES_PER_ROW * saved_tile_x, num_rows * saved_tile_y);

  if (tile_bytes)
  {
    free(tile_bytes);
    tile_bytes = NULL;
  }

  return 0;
}
