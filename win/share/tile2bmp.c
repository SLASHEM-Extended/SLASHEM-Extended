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

#if 0
#if BITCOUNT==4
#define MAX_X 320		/* 2 per byte, 4 bits per pixel */
#define MAX_Y 480
#else
# if (TILE_X==32)
#define MAX_X (32 * 40)
#define MAX_Y (960 * 2)
# else
#define MAX_X 640		/* 1 per byte, 8 bits per pixel */
#define MAX_Y (480 * 2)
# endif
#endif	
#endif

#if BITCOUNT==4
#define MAX_X 640		/* 2 per byte, 4 bits per pixel */
#define MAX_Y 960
#else
#define MAX_X 768		/* 3 * 2^8 */
#define MAX_Y (960 * 2) /* Arbitrarily large */
#endif	

#define MAX_X_TILES 40
#define MAX_Y_TILES 60

/* GCC fix by Paolo Bonzini 1999/03/28 */
#ifdef __GNUC__
#define PACK		__attribute__((packed))
#else
#define PACK
#endif 

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

#if 0
#if BITCOUNT==4
#define RGBQUAD_COUNT 16
    RGBQUAD          bmaColors[RGBQUAD_COUNT];
#else
#if (TILE_X==32)
#define RGBQUAD_COUNT 256
#else
#define RGBQUAD_COUNT 16
#endif
    RGBQUAD          bmaColors[RGBQUAD_COUNT];
#endif
#endif /* if 0 */

#if 0
#if (COLORS_IN_USE==16)
    uchar            packtile[MAX_Y][MAX_X];
#else
    uchar            packtile[MAX_Y][MAX_X];
/*    uchar            packtile[TILE_Y][TILE_X]; */
#endif
#endif
} PACK bmp_head;

#define RGBQUAD_COUNT_MAX 256

struct colormapBMP{
    RGBQUAD          bmaColors[RGBQUAD_COUNT_MAX];
} PACK bmp_colors;

struct tileBMP{
    uchar            packtile[MAX_Y][MAX_X];
} PACK bmp_img;
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

#if 0
char *tilefiles[] = {
#if (TILE_X == 32)
		"../win/share/mon32.txt",
		"../win/share/obj32.txt",
		"../win/share/oth32.txt"
#else
		"../win/share/monsters.txt",
		"../win/share/objects.txt",
		"../win/share/other.txt"
#endif
};
#endif

int num_colors = 0;
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
#if 0
	while (filenum < (sizeof(tilefiles) / sizeof(char *))) {
		if (!fopen_text_file(tilefiles[filenum], RDTMODE)) {
#endif
	while ((filenum) < argc) {
		if (!fopen_text_file(argv[(filenum)],RDTMODE)) {
			Fprintf(stderr,
				"usage: tile2bmp (from the util directory)\n");
			exit(EXIT_FAILURE);
		}
		num_colors = colorsinmap;
		if (num_colors > 256) {
			Fprintf(stderr, "too many colors (%d)\n", num_colors);
			exit(EXIT_FAILURE);
		}
		if (!initflag) {
			maxbmp_x = MAX_X; /* CONSTANT */
			maxbmp_y = tile_y * MAX_Y_TILES;
			if (num_colors <= 16)
			{
				rgbquad_count = 16;
			} else if (num_colors <= 256)
			{
				rgbquad_count = 256;
			}

			if (maxbmp_x > MAX_X || maxbmp_y > MAX_Y) {
				Fprintf(stderr, "Calculated dimensions (%ix%i) larger than max (%ix%i).  Increase MAX_X, MAX_Y\n",
						maxbmp_x, maxbmp_y, MAX_X, MAX_Y);
				exit(EXIT_FAILURE);
			}
#if 0
		    build_bmfh(&bmp_head.bmfh);
		    build_bmih(&bmp_head.bmih);
#endif
		    for (i = 0; i < maxbmp_y; ++i)
		    	for (j = 0; j < maxbmp_x; ++j)
		    		bmp_img.packtile[i][j] = (uchar)0;
		    for (i = 0; i < num_colors; i++) {
			    bmp_colors.bmaColors[i].rgbRed = ColorMap[CM_RED][i];
			    bmp_colors.bmaColors[i].rgbGreen = ColorMap[CM_GREEN][i];
			    bmp_colors.bmaColors[i].rgbBlue = ColorMap[CM_BLUE][i];
			    bmp_colors.bmaColors[i].rgbReserved = 0;
		    }
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
#if 0
	fwrite(&bmp, sizeof(bmp), 1, fp);
#endif

    build_bmfh(&bmp_head.bmfh);
    build_bmih(&bmp_head.bmih);

Fprintf(stderr, "yoff:%i maxbmp_y:%i\n", yoffset, maxbmp_y);
	fwrite(&bmp_head, sizeof(bmp_head), 1, fp);
	fwrite(&bmp_colors, (sizeof(RGBQUAD) * rgbquad_count), 1, fp);
	fwrite(&bmp_img.packtile[(maxbmp_y - 1) - (tile_y + yoffset)][0], (sizeof(uchar) * maxbmp_x * (yoffset+tile_y)), 1, fp);
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
	pbmfh->bfType = (UINT)0x4D42;
	pbmfh->bfSize = (DWORD)BMPFILESIZE;
	pbmfh->bfReserved1 = (UINT)0;
	pbmfh->bfReserved2 = (UINT)0;
	pbmfh->bfOffBits = sizeof(bmp_head.bmfh) + sizeof(bmp_head.bmih) +
			   (rgbquad_count * sizeof(RGBQUAD));
}

static void
build_bmih(pbmih)
BITMAPINFOHEADER *pbmih;
{
	WORD cClrBits;
	pbmih->biSize = (DWORD) sizeof(bmp_head.bmih);
#if BITCOUNT==4
	pbmih->biWidth = (LONG) maxbmp_x * 2;
#else
	pbmih->biWidth = (LONG) maxbmp_x;
#endif
#if 0
	pbmih->biHeight = (LONG) maxbmp_y;
#endif
	pbmih->biHeight = (LONG) (yoffset + tile_y);
	pbmih->biPlanes = (WORD) 1;
#if BITCOUNT==4
	pbmih->biBitCount = (WORD) 4;
#else
	pbmih->biBitCount = (WORD) 8;
#endif
	cClrBits = (WORD)(pbmih->biPlanes * pbmih->biBitCount); 
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
	pbmih->biCompression = (DWORD) BI_RGB;
	pbmih->biXPelsPerMeter = (LONG)0;
	pbmih->biYPelsPerMeter = (LONG)0;
	if (tile_x == 32)
	{
		if (cClrBits < 24) 
        		pbmih->biClrUsed = (1<<cClrBits);
	} else {
		pbmih->biClrUsed = (DWORD)rgbquad_count; 
	}

	if (tile_x == 16) {
		pbmih->biSizeImage = 0;
	} else {
		pbmih->biSizeImage = ((pbmih->biWidth * cClrBits +31) & ~31) /8
                              * pbmih->biHeight;
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
	  for (cur_color = 0; cur_color < num_colors; cur_color++) {
	   if (ColorMap[CM_RED][cur_color] == pixels[cur_y][cur_x].r &&
	      ColorMap[CM_GREEN][cur_color]== pixels[cur_y][cur_x].g &&
	      ColorMap[CM_BLUE][cur_color] == pixels[cur_y][cur_x].b)
		break;
	  }
	  if (cur_color >= num_colors)
		Fprintf(stderr, "color not in colormap!\n");
	  y = (maxbmp_y - 1) - (cur_y + yoffset);
#if BITCOUNT==4
	  x = (cur_x / 2) + xoffset;
	  bmp_img.packtile[y][x] = cur_x%2 ?
		(uchar)(bmp.packtile[y][x] | cur_color) :
		(uchar)(cur_color<<4);
#else
	  x = cur_x + xoffset;
	  bmp_img.packtile[y][x] = (uchar)cur_color;
#endif
	 }
	}
}
