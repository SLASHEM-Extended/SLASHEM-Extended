/* font-loading routines, modified from 
   Shawn Hargreaves' code for the Allegro grabber */

/* TODO:  Detect/Make compatible with Allegro WIP */
#if 0

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* splits bitmaps into sub-sprites, using regions bounded by col #255 */
void datedit_find_character(BITMAP *bmp, int *x, int *y, int *w, int *h)
{
   int c1;
   int c2;

   if (bitmap_color_depth(bmp) == 8) {
      c1 = 255;
      c2 = 255;
   }
   else {
      c1 = makecol_depth(bitmap_color_depth(bmp), 255, 255, 0);
      c2 = makecol_depth(bitmap_color_depth(bmp), 0, 255, 255);
   }

   /* look for top left corner of character */
   while ((getpixel(bmp, *x, *y) != c1) ||
          (getpixel(bmp, *x+1, *y) != c2) ||
          (getpixel(bmp, *x, *y+1) != c2) ||
          (getpixel(bmp, *x+1, *y+1) == c1) ||
          (getpixel(bmp, *x+1, *y+1) == c2)) {
      (*x)++;
      if (*x >= bmp->w) {
         *x = 0;
         (*y)++;
         if (*y >= bmp->h) {
            *w = 0;
            *h = 0;
            return;
         }
      }
   }

   /* look for right edge of character */
   *w = 0;
   while ((getpixel(bmp, *x+*w+1, *y) == c2) &&
          (getpixel(bmp, *x+*w+1, *y+1) != c2) &&
          (*x+*w+1 <= bmp->w))
      (*w)++;

   /* look for bottom edge of character */
   *h = 0;
   while ((getpixel(bmp, *x, *y+*h+1) == c2) &&
          (getpixel(bmp, *x+1, *y+*h+1) != c2) &&
          (*y+*h+1 <= bmp->h))
      (*h)++;
}


/* creates a new font object */
static void *makenew_font(long *size)
{
   FONT *f = malloc(sizeof(FONT));

   f->height = 8;
   f->dat.dat_8x8 = malloc(sizeof(FONT_8x8));
   memcpy(f->dat.dat_8x8, font->dat.dat_8x8, sizeof(FONT_8x8));

   return f;
}

/* GRX font file reader by Mark Wodrich.
 *
 * GRX FNT files consist of the header data (see struct below). If the font
 * is proportional, followed by a table of widths per character (unsigned 
 * shorts). Then, the data for each character follows. 1 bit/pixel is used,
 * with each line of the character stored in contiguous bytes. High bit of
 * first byte is leftmost pixel of line.
 *
 * Note : FNT files can have a variable number of characters, so we must
 *        check that the chars 32..127 exist.
 */


#define FONTMAGIC       0x19590214L


/* .FNT file header */
typedef struct {
   unsigned long  magic;
   unsigned long  bmpsize;
   unsigned short width;
   unsigned short height;
   unsigned short minchar;
   unsigned short maxchar;
   unsigned short isfixed;
   unsigned short reserved;
   unsigned short baseline;
   unsigned short undwidth;
   char           fname[16];
   char           family[16];
} FNTfile_header;


#define GRX_TMP_SIZE    4096



/* converts images from bit to byte format */
static void convert_grx_bitmap(int width, int height, unsigned char *src, unsigned char *dest) 
{
   unsigned short x, y, bytes_per_line;
   unsigned char bitpos, bitset;

   bytes_per_line = (width+7) >> 3;

   for (y=0; y<height; y++) {
      for (x=0; x<width; x++) {
	 bitpos = 7-(x&7);
	 bitset = !!(src[(bytes_per_line*y) + (x>>3)] & (1<<bitpos));
	 dest[y*width+x] = bitset;
      }
   }
}



/* reads GRX format images from disk */
static unsigned char **load_grx_bmps(PACKFILE *f, FNTfile_header *hdr, int numchar, unsigned short *wtable) 
{
   int t, width, bmp_size;
   unsigned char *temp;
   unsigned char **bmp;

   /* alloc array of bitmap pointers */
   bmp = malloc(sizeof(unsigned char *) * numchar);

   /* assume it's fixed width for now */
   width = hdr->width;

   /* temporary working area to store FNT bitmap */
   temp = malloc(GRX_TMP_SIZE);

   for (t=0; t<numchar; t++) {
      /* if prop. get character width */
      if (!hdr->isfixed) 
	 width = wtable[t];

      /* work out how many bytes to read */
      bmp_size = ((width+7) >> 3) * hdr->height;

      /* oops, out of space! */
      if (bmp_size > GRX_TMP_SIZE) {
	 free(temp);
	 for (t--; t>=0; t--)
	    free(bmp[t]);
	 free(bmp);
	 return NULL;
      }

      /* alloc space for converted bitmap */
      bmp[t] = malloc(width*hdr->height);

      /* read data */
      pack_fread(temp, bmp_size, f);

      /* convert to 1 byte/pixel */
      convert_grx_bitmap(width, hdr->height, temp, bmp[t]);
   }

   free(temp);
   return bmp;
}



/* main import routine for the GRX font format */
static FONT *import_grx_font(char *fname)
{
   PACKFILE *f, *cf;
   FNTfile_header hdr;              /* GRX font header */
   int numchar;                     /* number of characters in the font */
   unsigned short *wtable = NULL;   /* table of widths for each character */
   unsigned char **bmp;             /* array of font bitmaps */
   FONT *font = NULL;               /* the Allegro font */
   FONT_PROP *font_prop;
   int c, c2, start, width;
   char copyright[256];

   f = pack_fopen(fname, F_READ);
   if (!f)
      return NULL;

   pack_fread(&hdr, sizeof(hdr), f);      /* read the header structure */

   if (hdr.magic != FONTMAGIC) {          /* check magic number */
      pack_fclose(f);
      return NULL;
   }

   numchar = hdr.maxchar-hdr.minchar+1;

   if (!hdr.isfixed) {                    /* proportional font */
      wtable = malloc(sizeof(unsigned short) * numchar);
      pack_fread(wtable, sizeof(unsigned short) * numchar, f);
   }

   bmp = load_grx_bmps(f, &hdr, numchar, wtable);
   if (!bmp)
      goto get_out;

   if (pack_ferror(f))
      goto get_out;

   if ((hdr.minchar < ' ') || (hdr.maxchar >= ' '+FONT_SIZE))
      printf("Warning: font exceeds range 32..256. Characters will be lost in conversion");

   font = malloc(sizeof(FONT));
   font->height = -1;
   font->dat.dat_prop = font_prop = malloc(sizeof(FONT_PROP));
   font_prop->render = NULL;

   start = 32 - hdr.minchar;
   width = hdr.width;

   for (c=0; c<FONT_SIZE; c++) {
      c2 = c+start;

      if ((c2 >= 0) && (c2 < numchar)) {
	 if (!hdr.isfixed)
	    width = wtable[c2];

	 font_prop->dat[c] = create_bitmap_ex(8, width, hdr.height);
	 memcpy(font_prop->dat[c]->dat, bmp[c2], width*hdr.height);
      }
      else {
	 font_prop->dat[c] = create_bitmap_ex(8, 8, hdr.height);
	 clear(font_prop->dat[c]);
      }
   }

   if (!pack_feof(f)) {
      strcpy(copyright, fname);
      strcpy(get_extension(copyright), "txt");
      c = 'n';
      if ((c != 27) && (c != 'n') && (c != 'N')) {
	 cf = pack_fopen(copyright, F_WRITE);
	 if (cf) {
	    while (!pack_feof(f)) {
	       pack_fgets(copyright, 255, f);
	       if (isspace(copyright[0])) {
		  pack_fputs(copyright, cf);
		  pack_fputs("\n", cf);
	       }
	       else if (!copyright[0])
		  pack_fputs("\n", cf);
	    }
	    pack_fclose(cf);
	 }
      }
   }

   get_out:

   pack_fclose(f);

   if (wtable)
      free(wtable);

   if (bmp) {
      for (c=0; c<numchar; c++)
	 free(bmp[c]);
      free(bmp);
   }

   return font;
}



/* main import routine for the 8x8 and 8x16 BIOS font formats */
static FONT *import_bios_font(char *fname)
{
   PACKFILE *f;
   FONT *font = NULL; 
   unsigned char data16[256][16];
   unsigned char data8[256][8];
   int font_h;
   int c, x, y;

   f = pack_fopen(fname, F_READ);
   if (!f)
      return NULL;

   font_h = (f->todo == 2048) ? 8 : 16;

   if (font_h == 16)
      pack_fread(data16, sizeof(data16), f);
   else
      pack_fread(data8, sizeof(data8), f);

   pack_fclose(f);

   font = malloc(sizeof(FONT));
   font->height = -1;
   font->dat.dat_prop = malloc(sizeof(FONT_PROP));
   font->dat.dat_prop->render = NULL;

   for (c=0; c<FONT_SIZE; c++) {
      font->dat.dat_prop->dat[c] = create_bitmap_ex(8, 8, font_h);
      clear(font->dat.dat_prop->dat[c]);

      for (y=0; y<font_h; y++) {
	 for (x=0; x<8; x++) {
	    if (font_h == 16) {
	       if (data16[c + ' '][y] & (0x80 >> x))
		  font->dat.dat_prop->dat[c]->line[y][x] = 1;
	    }
	    else {
	       if (data8[c + ' '][y] & (0x80 >> x))
		  font->dat.dat_prop->dat[c]->line[y][x] = 1;
	    }
	 }
      }
   }

   return font;
}


/* main import routine for the Allegro .pcx font format */
static void *import_bitmap_font(char *fname)
{
   PALLETE junk;
   BITMAP *bmp;
   FONT *f;
   int x, y, w, h, c;
   int max_h = 0;

   bmp = load_bitmap(fname, junk);
   if (!bmp)
      return NULL;

   if (bitmap_color_depth(bmp) != 8) {
      destroy_bitmap(bmp);
      return NULL;
   }

   f = malloc(sizeof(FONT));
   f->height = -1;
   f->dat.dat_prop = malloc(sizeof(FONT_PROP));
   f->dat.dat_prop->render = NULL;
   for (c=0; c<FONT_SIZE; c++)
      f->dat.dat_prop->dat[c] = NULL;

   x = 0;
   y = 0;

   for (c=0; c<FONT_SIZE; c++) {
      datedit_find_character(bmp, &x, &y, &w, &h);

      if ((w <= 0) || (h <= 0)) {
	 w = 8;
	 h = 8;
      }

      f->dat.dat_prop->dat[c] = create_bitmap_ex(8, w, h);
      clear(f->dat.dat_prop->dat[c]);
      blit(bmp, f->dat.dat_prop->dat[c], x+1, y+1, 0, 0, w, h);

      max_h = MAX(max_h, h);
      x += w;
   }

   for (c=0; c<FONT_SIZE; c++) {
      if (f->dat.dat_prop->dat[c]->h < max_h) {
	 BITMAP *b = f->dat.dat_prop->dat[c];
	 f->dat.dat_prop->dat[c] = create_bitmap_ex(8, b->w, max_h);
	 clear(f->dat.dat_prop->dat[c]);
	 blit(b, f->dat.dat_prop->dat[c], 0, 0, 0, 0, b->w, b->h);
	 destroy_bitmap(b);
      }
   }

   destroy_bitmap(bmp);
   return f;
}


/* converts a proportional font to 8x8 format */
static FONT *make_font8x8(FONT *f)
{
   FONT_PROP *fp = f->dat.dat_prop;
   FONT_8x8 *f8 = malloc(sizeof(FONT_8x8));
   BITMAP *bmp;
   int c, x, y;

   for (c=0; c<FONT_SIZE; c++) {
      bmp = fp->dat[c];

      for (y=0; y<8; y++) {
	 f8->dat[c][y] = 0;
	 for (x=0; x<8; x++)
	    if (bmp->line[y][x])
	       f8->dat[c][y] |= (0x80 >> x);
      }

      destroy_bitmap(bmp); 
   }

   free(fp);
   f->dat.dat_8x8 = f8;
   f->height = 8;
   return f;
}



/* converts a proportional font to 8x16 format */
static FONT *make_font8x16(FONT *f)
{
   FONT_PROP *fp = f->dat.dat_prop;
   FONT_8x16 *f16 = malloc(sizeof(FONT_8x16));
   BITMAP *bmp;
   int c, x, y;

   for (c=0; c<FONT_SIZE; c++) {
      bmp = fp->dat[c];

      for (y=0; y<16; y++) {
	 f16->dat[c][y] = 0;
	 for (x=0; x<8; x++)
	    if (bmp->line[y][x])
	       f16->dat[c][y] |= (0x80 >> x);
      }

      destroy_bitmap(bmp); 
   }

   free(fp);
   f->dat.dat_8x16 = f16;
   f->height = 16;
   return f;
}



/* make sure a font will use 8x8 or 8x16 format if that is possible */
static FONT *fixup_font(FONT *f)
{
   int c, w, h, x, y, n;
   int col = -1;

   if (!f)
      return NULL;

   w = f->dat.dat_prop->dat[0]->w;
   h = f->dat.dat_prop->dat[0]->h;

   for (c=1; c<FONT_SIZE; c++) {
      if ((f->dat.dat_prop->dat[c]->w != w) ||
	  (f->dat.dat_prop->dat[c]->h != h))
	 return f;

      for (y=0; y<h; y++) {
	 for (x=0; x<w; x++) {
	    n = f->dat.dat_prop->dat[c]->line[y][x];
	    if (n) {
	       if (col < 0)
		  col = n;
	       else if (col != n)
		  return f;
	    }
	 }
      }
   }

   if ((w == 8) && (h == 8))
      return make_font8x8(f);
   else if ((w == 8) && (h == 16))
      return make_font8x16(f);
   else
      return f;
}



/* imports a font from an external file (handles various formats) */
static void *load_font(char *filename)
{
   PACKFILE *f;
   int id;
   if ((stricmp(get_extension(filename), "bmp") == 0) ||
       (stricmp(get_extension(filename), "lbm") == 0) ||
       (stricmp(get_extension(filename), "pcx") == 0) ||
       (stricmp(get_extension(filename), "tga") == 0)) {
      return fixup_font(import_bitmap_font(filename));
   }
   else 
{
      f = pack_fopen(filename, F_READ);
      if (!f)
	 return NULL;

      id = pack_igetl(f);
      pack_fclose(f);

      if(0) /*  (id == FONTMAGIC) */
	 return fixup_font(import_grx_font(filename));
      else
	 return fixup_font(import_bios_font(filename));
   }
}
#endif
