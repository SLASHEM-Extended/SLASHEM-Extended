/*
 * Copyright (C) Andrew Apted <ajapted@users.sourceforge.net> 2002
 *		 Slash'EM Development Team 2003
 * NetHack may be freely redistributed.  See license for details.
 */

/*
 * Convert the given input tile files into the PNG file for use by
 * various windowing ports.
 * 
 * Based on win/X11/tile2x11.c
 * 
 * TODO HERE:
 *   + Would be nice if it didn't use so much memory...
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

unsigned char *tile_bytes_rgba = NULL;
unsigned char *tile_bytes_i = NULL;
png_colorp tile_palette = NULL;
int tile_palette_n = 0;

int has_trans = 0;
int num_across = TILES_PER_ROW;
pixel trans_p = DEFAULT_BACKGROUND;


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

  png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

  png_set_IHDR(png_ptr, info_ptr, width, height, 8, 
      tile_palette ? PNG_COLOR_TYPE_PALETTE :
      has_trans ? PNG_COLOR_TYPE_RGB_ALPHA : PNG_COLOR_TYPE_RGB, 
      PNG_INTERLACE_NONE, 
      PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  if (tile_palette)
  {
    png_set_PLTE(png_ptr, info_ptr, tile_palette, tile_palette_n);
    if (has_trans)
    {
      png_byte trans = (png_byte)0;		/* Colour zero is transparent */
      png_set_tRNS(png_ptr, info_ptr, &trans, 1, NULL);
    }
  }

  png_write_info(png_ptr, info_ptr);

  row_pointers = (png_bytep *) malloc(height * sizeof(png_bytep));
  if (!row_pointers)
  {
    fprintf(stderr, "tile2png: Out of memory.\n");
    goto failed;
  }
   
  if (tile_palette)
    for (row = 0; row < height; row++)
       row_pointers[row] = tile_bytes_i + row * width;
  else
    for (row = 0; row < height; row++)
       row_pointers[row] = tile_bytes_rgba + row * width * (has_trans ? 4 : 3);

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

  int x, y, i;
  int bx, by;
  int count = 0;

  int pix_w  = (has_trans ? 4 : 3);
  int stride = tile_x * num_across * pix_w;

  while (read_text_tile(tile)) 
  {
    count++;

    bx = ntiles % num_across;
    by = ntiles / num_across;
    
    tb = tile_bytes_rgba + (by * tile_y * stride + bx * tile_x * pix_w);
   
    for (y = 0; y < tile_y; y++, tb += stride)
      for (x = 0; x < tile_x; x++)
      {
	tb[x*pix_w + 0] = tile[y][x].r;
	tb[x*pix_w + 1] = tile[y][x].g;
	tb[x*pix_w + 2] = tile[y][x].b;

	if (has_trans)
	  tb[x*pix_w + 3] = pixel_equal(tile[y][x], trans_p) ? 0 : 255;

	if (tile_palette)
	{
	  for (i = tile_palette_n - 1; i >= 0; i--)
	  {
	    if (tile_palette[i].red == tile[y][x].r &&
		tile_palette[i].green == tile[y][x].g &&
		tile_palette[i].blue == tile[y][x].b)
	      break;
	  }
	  if (i < 0)
	  {
	    if (tile_palette_n < 256)
	    {
	      i = tile_palette_n++;
	      tile_palette[i].red = tile[y][x].r;
	      tile_palette[i].green = tile[y][x].g;
	      tile_palette[i].blue = tile[y][x].b;
	    }
	    else
	    {
	      free(tile_palette);
	      tile_palette = NULL;
	      free(tile_bytes_i);
	      tile_bytes_i = NULL;
	    }
	  }
	}
	if (tile_bytes_i)
	  tile_bytes_i[((by * tile_y + y) * num_across + bx) * tile_x + x] = i;
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

  if (!tile_bytes_rgba) 
  {
    int size;

    /*
     * Delayed until we open the first input file so that
     * we know the size of the tiles we are processing.
     */
    max_rows = (MAX_TILE_NUM + num_across - 1) / num_across;

    size = max_rows * num_across * tile_x * tile_y * (has_trans ? 4 : 3);

    tile_bytes_rgba = calloc(size, 1);
    if (!tile_bytes_rgba) 
    {
      Fprintf(stderr, "tile2png: Not enough memory (%d KB).\n", size/1024);
      exit(1);
    }

    saved_tile_x = tile_x;
    saved_tile_y = tile_y;
  }
  if (!tile_bytes_i) 
  {
    int size;

    max_rows = (MAX_TILE_NUM + num_across - 1) / num_across;

    size = max_rows * num_across * tile_x * tile_y;

    tile_bytes_i = calloc(size, 1);
    if (!tile_bytes_i) 
    {
      Fprintf(stderr, "tile2png: Not enough memory (%d KB).\n", size/1024);
      exit(1);
    }

    saved_tile_x = tile_x;
    saved_tile_y = tile_y;
  }
  if (!tile_palette) 
  {
    tile_palette = malloc(256 * sizeof(*tile_palette));
    if (!tile_palette)
    {
      Fprintf(stderr, "tile2png: Not enough memory (%d bytes).\n",
	  sizeof(*tile_palette));
      exit(1);
    }
    /* [ALI} PNG encoding is more efficient if the transparent
     * colour is first in the palette, so add it here. This will
     * cause a slight inefficiency if the image data proves not
     * to contain any transparent pixels, but still seems the
     * best option.
     */
    if (has_trans)
    {
      tile_palette_n = 1;
      tile_palette[0].red = trans_p.r;
      tile_palette[0].green = trans_p.g;
      tile_palette[0].blue = trans_p.b;
    }
  }
  count = convert_tiles();
  Fprintf(stderr, "%s: %d tiles (%dx%d)\n", fname, count, tile_x, tile_y);
  fclose_text_file();
}


static void usage(void)
{
  Fprintf(stderr, "Usage: tile2png [-o out_file] [-t] [-f] [-a##] [-b######] "
      "txt_file1 [txt_file2 ...]\n");
  fprintf(stderr, "Where: -t enables transparency\n");
  fprintf(stderr, "       -f is used for fonts\n");
  fprintf(stderr, "       -a gives the number of tiles across\n");
  fprintf(stderr, "       -b gives the background (transparent) color\n");
}


int main(int argc, const char **argv)
{
  int i, argn = 1;
  int num_down;

  const char *outname = OUTNAME;

  while (argn < argc) 
  {
    if ((argv[argn][0] == '-' && argv[argn][1] == 'h') ||
        (argv[argn][0] == '-' && argv[argn][1] == '-' && 
         argv[argn][2] == 'h'))
    {
      usage();
      exit(1);
    }

    if (argv[argn][0] == '-' && argv[argn][1] == 'o') 
    {
      if (argv[argn][2])
        outname = argv[argn] + 2;
      else if (argn + 1 < argc)
        outname = argv[++argn];
      else 
      {
        Fprintf(stderr, "tile2png: -o option needs an argument\n");
        exit(EXIT_FAILURE);
      }
      argn++;
      continue;
    }

    if (argv[argn][0] == '-' && argv[argn][1] == 'a') 
    {
      if (argv[argn][2])
        num_across = atoi(argv[argn] + 2);
      else if (argn + 1 < argc)
        num_across = atoi(argv[++argn]);
      else 
      {
        Fprintf(stderr, "tile2png: -a option needs an argument\n");
        exit(EXIT_FAILURE);
      }
      if (num_across < 1)
      {
        Fprintf(stderr, "tile2png: bad value for -a option\n");
        exit(EXIT_FAILURE);
      }
      argn++;
      continue;
    }
    
    if (argv[argn][0] == '-' && argv[argn][1] == 't') 
    {
      has_trans = 1;
      argn++;
      continue;
    }
    
    if (argv[argn][0] == '-' && argv[argn][1] == 'f') 
    {
      has_trans = 1;
      trans_p.r = trans_p.g = trans_p.b = 0;
      num_across = 16;
      argn++;
      continue;
    }
    
    if (argv[argn][0] == '-' && argv[argn][1] == 'b') 
    {
      const char *val;
      int r, g, b;

      if (argv[argn][2])
        val = argv[argn] + 2;
      else if (argn + 1 < argc)
        val = argv[++argn];
      else 
      {
        Fprintf(stderr, "tile2png: -b option needs an argument\n");
        exit(EXIT_FAILURE);
      }

      if (sscanf(val, "%02X%02X%02X", &r, &g, &b) != 3)
      {
        Fprintf(stderr, "tile2png: Background not understood: %s\n", val);
        exit(EXIT_FAILURE);
      }
      trans_p.r = (unsigned char) r;
      trans_p.g = (unsigned char) g;
      trans_p.b = (unsigned char) b;

      argn++;
      continue;
    }
    
    break;
  }

  if (argn == argc) 
  {
    usage();
    exit(1);
  }

  for (i = argn; i < argc; i++)
  {
    process_file(argv[i]);
  }

  Fprintf(stderr, "Total tiles: %d\n", ntiles);

  num_down = (ntiles + num_across - 1) / num_across;
  
  save_png(outname, num_across * saved_tile_x, num_down * saved_tile_y);

  if (tile_bytes_rgba)
  {
    free(tile_bytes_rgba);
    tile_bytes_rgba = NULL;
  }

  if (tile_bytes_i)
  {
    free(tile_bytes_i);
    tile_bytes_i = NULL;
  }

  if (tile_palette)
  {
    free(tile_palette);
    tile_palette = NULL;
  }

  return 0;
}
