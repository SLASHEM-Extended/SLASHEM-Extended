/*	SCCS Id: @(#)tile2bmp.c	3.4	2002/03/14	*/
/*   Copyright (c) NetHack PC Development Team 1995                 */
/*   NetHack may be freely redistributed.  See license for details. */

/*
 * Edit History:
 *
 *	Initial Creation			M.Allison   1994/01/11
 *
 */

/* #pragma warning(4103:disable) */

#include "hack.h"
#include "tile.h"
#ifndef __GNUC__
#include "win32api.h"
#endif

/* #define COLORS_IN_USE MAXCOLORMAPSIZE       /* 256 colors */
#if (TILE_X==32)
#define COLORS_IN_USE 256
#else
#define COLORS_IN_USE 16                       /* 16 colors */
#endif

#define BITCOUNT 8

extern char *FDECL(tilename, (int, int));

#if BITCOUNT==4
#define MAX_X 640		/* 2 per byte, 4 bits per pixel */
#define MAX_Y 960
#else
#define MAX_X (40 * 128)	
#define MAX_Y (40 * 128) /* Arbitrarily large */
#endif	

#define MAX_X_TILES 40
#define MAX_Y_TILES 120

/* GCC fix by Paolo Bonzini 1999/03/28 */
#ifdef __GNUC__
#define PACK		__attribute__((packed))
#else
#define PACK
#endif 

static short leshort(short x)
{
#ifdef __BIG_ENDIAN__
    return ((x&0xff)<<8)|((x>>8)&0xff);
#else
    return x;
#endif
}


static long lelong(long x)
{
#ifdef __BIG_ENDIAN__
    return ((x&0xff)<<24)|((x&0xff00)<<8)|((x>>8)&0xff00)|((x>>24)&0xff);
#else
    return x;
#endif
}

#ifdef __GNUC__
typedef struct tagBMIH {
        unsigned long   biSize;
        long       	biWidth;
        long       	biHeight;
        unsigned short  biPlanes;
        unsigned short  biBitCount;
        unsigned long   biCompression;
        unsigned long   biSizeImage;
        long       	biXPelsPerMeter;
        long       	biYPelsPerMeter;
        unsigned long   biClrUsed;
        unsigned long   biClrImportant;
} PACK BITMAPINFOHEADER;

typedef struct tagBMFH {
        unsigned short bfType;
        unsigned long  bfSize;
        unsigned short bfReserved1;
        unsigned short bfReserved2;
        unsigned long  bfOffBits;
} PACK BITMAPFILEHEADER;

typedef struct tagRGBQ {
        unsigned char    rgbBlue;
        unsigned char    rgbGreen;
        unsigned char    rgbRed;
        unsigned char    rgbReserved;
} PACK RGBQUAD;
#define UINT unsigned int
#define DWORD unsigned long
#define LONG long
#define WORD unsigned short
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L
#endif /* __GNUC__ */

#pragma pack(1)

struct tagBMP{
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;
} PACK bmp_head;

#define MAX_RGBQUAD_COUNT 256

/* WAC split the header from the colormap and pixel data
 * to accomodate variable sized BMP files
 */

RGBQUAD          bmaColors[MAX_RGBQUAD_COUNT];

uchar            packtile[MAX_Y][MAX_X];

#pragma pack()

#define BMPFILESIZE ((sizeof(struct tagBMP)) + \
					 (sizeof(RGBQUAD) * rgbquad_count) + \
					 (sizeof(uchar) * maxbmp_x * (yoffset + tile_y)))
FILE *tibfile2;

/*pixel tilepixels[TILE_Y][TILE_X];*/
pixel tilepixels[MAX_TILE_Y][MAX_TILE_X];

static void FDECL(build_bmfh,(BITMAPFILEHEADER *));
static void FDECL(build_bmih,(BITMAPINFOHEADER *));
static void FDECL(build_bmptile,(pixel (*)[MAX_TILE_X]));

int tilecount;
int max_tiles_in_row = 40;
int tiles_in_row;
int filenum;
int initflag;
int yoffset,xoffset;
char bmpname[128];
int maxbmp_x = -1, maxbmp_y = -1;
int rgbquad_count = -1;
FILE *fp;

int
main(argc, argv)
int argc;
char *argv[];
{
	int i, j;

	if (argc < 3) {
		Fprintf(stderr, "usage: %s outfile.bmp txt_file1 [txt_file2 ...]\n", argv[0]);
		exit(EXIT_FAILURE);
	} else
		strcpy(bmpname, argv[1]);

#ifdef OBSOLETE
	bmpfile2 = fopen(NETHACK_PACKED_TILEFILE, WRBMODE);
	if (bmpfile2 == (FILE *)0) {
		Fprintf(stderr, "Unable to open output file %s\n",
				NETHACK_PACKED_TILEFILE);
		exit(EXIT_FAILURE);
	}
#endif

	tilecount = 0;
	xoffset = yoffset = 0;
	initflag = 0;
	filenum = 2;
	fp = fopen(bmpname,"wb");
	if (!fp) {
	    printf("Error creating tile file %s, aborting.\n",bmpname);
	    exit(1);
	}
	while ((filenum) < argc) {
		if (!fopen_text_file(argv[(filenum)],RDTMODE)) {
			Fprintf(stderr,
				"usage: tile2bmp (from the util directory)\n");
			exit(EXIT_FAILURE);
		}
		if (colorsinmap > 256) {
			Fprintf(stderr, "too many colors (%d)\n", colorsinmap);
			exit(EXIT_FAILURE);
		}
		if (!initflag) {
			maxbmp_x = tile_x * 40; /* CONSTANT */
			maxbmp_y = MAX_Y;

			if (maxbmp_x > MAX_X || maxbmp_y > MAX_Y) {
				Fprintf(stderr, "Calculated dimensions (%ix%i) larger than max (%ix%i).  Increase MAX_X, MAX_Y\n",
						maxbmp_x, maxbmp_y, MAX_X, MAX_Y);
				exit(EXIT_FAILURE);
			}
		    for (i = 0; i < maxbmp_y; ++i)
		    	for (j = 0; j < maxbmp_x; ++j)
		    		packtile[i][j] = (uchar)0;
		    initflag = 1;
		}
/*		printf("Colormap initialized\n"); */
		while (read_text_tile(tilepixels)) {
			build_bmptile(tilepixels);
			tilecount++;
#if BITCOUNT==4
			xoffset += (tile_x / 2);
#else
			xoffset += tile_x;
#endif
			if (xoffset >= maxbmp_x) {
				yoffset += tile_y;
				xoffset = 0;
				if ((yoffset+tile_y) > maxbmp_y) {
					Fprintf(stderr,
						"Too many tiles (increase MAX_X_TILES or MAX_Y_TILES)\n");
					exit(EXIT_FAILURE);
				}
			}
		}
		(void) fclose_text_file();
		++filenum;
	}

	if (tilecount<1) {
	    Fprintf(stderr,"No tiles created! (check line end character sequence for your OS).\n");
	    fclose(fp);
	    unlink(bmpname);
		exit(EXIT_FAILURE);
	}

	/* fill the rest with the checkerboard */
	for (j=0; j < tile_y; j++)
	{
		for (i = xoffset; i < maxbmp_x; i+=2) {
		  int y = (maxbmp_y - 1) - (j + yoffset);
#if BITCOUNT==4
		  packtile[y][i] = 	(uchar)((1+y)&1) | (uchar)(((0+y)&1)<<4);
#else
		  packtile[y][i] = (uchar)((0+y)&1);
		  packtile[y][i+1] = (uchar)((1+y)&1);
#endif
		}
	}

	if (colorsinmap <= 16)
		rgbquad_count = 16;
	else if (colorsinmap <= 256)
		rgbquad_count = 256;

	for (i = 0; i < colorsinmap; i++) {
		bmaColors[i].rgbRed = MainColorMap[CM_RED][i];
		bmaColors[i].rgbGreen = MainColorMap[CM_GREEN][i];
		bmaColors[i].rgbBlue = MainColorMap[CM_BLUE][i];
		bmaColors[i].rgbReserved = 0;
	}
	

    build_bmfh(&bmp_head.bmfh);
    build_bmih(&bmp_head.bmih);

	fwrite(&bmp_head, sizeof(bmp_head), 1, fp);

	for (i = 0; i < rgbquad_count; i++)
		fwrite(&bmaColors[i], sizeof(RGBQUAD), 1, fp);

	/* packtile[][] is filled bottom up, but is written top-down
	 * starting at the topmost line used
	 */
	for (i = (maxbmp_y - (tile_y + yoffset)); i < maxbmp_y; i++)
		for (j = 0; j < maxbmp_x; j++)
			fwrite(&packtile[i][j], sizeof(uchar), 1, fp);
	fclose(fp);
	Fprintf(stderr, "Total of %d tiles written to %s.\n",
		tilecount, bmpname);

	exit(EXIT_SUCCESS);
	/*NOTREACHED*/
	return 0;
}


static void
build_bmfh(pbmfh)
BITMAPFILEHEADER *pbmfh;
{
	pbmfh->bfType = leshort(0x4D42);
	pbmfh->bfSize = lelong(BMPFILESIZE);
	pbmfh->bfReserved1 = (UINT)0;
	pbmfh->bfReserved2 = (UINT)0;
	pbmfh->bfOffBits = lelong(sizeof(bmp_head.bmfh) + sizeof(bmp_head.bmih) +
			   (rgbquad_count * sizeof(RGBQUAD)));
}

static void
build_bmih(pbmih)
BITMAPINFOHEADER *pbmih;
{
	WORD cClrBits;
	int w,h;
	pbmih->biSize = lelong(sizeof(bmp_head.bmih));
#if BITCOUNT==4
	pbmih->biWidth = lelong(w = maxbmp_x * 2);
#else
	pbmih->biWidth = lelong(w = maxbmp_x);
#endif
	pbmih->biHeight = lelong(h = yoffset + tile_y);
	pbmih->biPlanes = leshort(1);
#if BITCOUNT==4
	pbmih->biBitCount = leshort(4);
	cClrBits = 4;
#else
	pbmih->biBitCount = leshort(8);
	cClrBits = 8;
#endif
	if (cClrBits == 1) 
	        cClrBits = 1; 
	else if (cClrBits <= 4) 
		cClrBits = 4; 
	else if (cClrBits <= 8) 
		cClrBits = 8; 
	else if (cClrBits <= 16) 
		cClrBits = 16; 
	else if (cClrBits <= 24) 
		cClrBits = 24; 
	else cClrBits = 32; 
	pbmih->biCompression = lelong(BI_RGB);
	pbmih->biXPelsPerMeter = lelong(0);
	pbmih->biYPelsPerMeter = lelong(0);
	if (tile_x == 32)
	{
		if (cClrBits < 24) 
        		pbmih->biClrUsed = lelong(1<<cClrBits);
	} else {
		pbmih->biClrUsed = lelong(rgbquad_count); 
	}

	if (tile_x == 16) {
		pbmih->biSizeImage = lelong(0);
	} else {
		pbmih->biSizeImage = lelong(((w * cClrBits +31) & ~31) /8 * h);
	}
 	pbmih->biClrImportant = (DWORD)0;
}

static void
build_bmptile(pixels)
pixel (*pixels)[MAX_TILE_X];
{
	int cur_x, cur_y, cur_color;
	int x,y;

	for (cur_y = 0; cur_y < tile_y; cur_y++) {
	 for (cur_x = 0; cur_x < tile_x; cur_x++) {
	  for (cur_color = 0; cur_color < colorsinmap; cur_color++) {
	   if (MainColorMap[CM_RED][cur_color] == pixels[cur_y][cur_x].r &&
	      MainColorMap[CM_GREEN][cur_color]== pixels[cur_y][cur_x].g &&
	      MainColorMap[CM_BLUE][cur_color] == pixels[cur_y][cur_x].b)
		break;
	  }
	  if (cur_color >= colorsinmap)
		Fprintf(stderr, "color not in colormap!\n");
	  y = (maxbmp_y - 1) - (cur_y + yoffset);
#if BITCOUNT==4
	  x = (cur_x / 2) + xoffset;
	  packtile[y][x] = cur_x%2 ?
		(uchar)(packtile[y][x] | cur_color) :
		(uchar)(cur_color<<4);
#else
	  x = cur_x + xoffset;
	  packtile[y][x] = (uchar)cur_color;
#endif
	 }
	}
}
