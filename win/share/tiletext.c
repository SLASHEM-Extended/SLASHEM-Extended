/*	SCCS Id: @(#)tiletext.c	3.4	1999/10/24	*/
/* NetHack may be freely redistributed.  See license for details. */

#include "config.h"
#include "tile.h"

/*
 * TEXTCOLORMAPSPACE
 *
 * This is the maximum number of possible unique colours in a .TXT file
 * file. MAXCOLORMAPSIZE may be set (in tile.h) to be smaller than this
 * in which case only that many unique colours may be present in a .TXT
 * file (and in all merged .TXT files). MAXCOLORMAPSIZE may be larger
 * than this without penalty. A value of n*TEXTCOLORMAPSPACE where n is
 * the number of .TXT files that are going to be merged (currently three;
 * monsters, objects & others) produces maximum generality. Increasing
 * MAXCOLORMAPSIZE has performance issues for the game (TEXTCOLORMAPSPACE
 * does not). Windowing systems are required to cope with MAXCOLORMAPSIZE
 * colours and map them onto their possible palette (see tile.doc) so this
 * also introduces an additional burden.
 */

#define TEXTCOLORMAPSPACE	4096

pixval ColorMap[3][MAXCOLORMAPSIZE];
int colorsinmap;
pixval MainColorMap[3][MAXCOLORMAPSIZE];
int colorsinmainmap;
int tile_x = -1, tile_y = -1;

static short color_index[TEXTCOLORMAPSPACE];
static int num_colors;
static char charcolors[MAXCOLORMAPSIZE][3];

static int placeholder_init = 0;
static pixel placeholder[MAX_TILE_Y][MAX_TILE_X];
static FILE *in_file, *out_file;
static int tile_set, tile_set_indx;
static const char *thin_text_sets[] = {
  "monthin.txt", "objthin.txt", "oththin.txt", "comthin.txt"
};
static const char *big_text_sets[] = {
  "mon32.txt", "obj32.txt", "oth32.txt", "com32.txt"
};
static const char *std_text_sets[] = {
  "monsters.txt", "objects.txt", "other.txt", "combined.txt"
};
#define TEXT_SETS(i)	( tile_x == 8 ? thin_text_sets[i] : \
			tile_x == 32 && tile_y == 32 ? big_text_sets[i] : \
			std_text_sets[i] )

extern const char *FDECL(tilename, (int, int));
static void FDECL(read_text_colormap, (FILE *));
static boolean FDECL(write_text_colormap, (FILE *));
static boolean FDECL(peek_txttile_info, (FILE *, char *, int *, char *));
static boolean FDECL(read_txttile_info, (FILE *, pixel(*)[MAX_TILE_X],
	char *, int *, char *));
static boolean FDECL(read_txttile, (FILE *, pixel(*)[MAX_TILE_X]));
static void FDECL(write_txttile_info, (FILE *, pixel(*)[MAX_TILE_X],
	const char *, int, const char *));
static void FDECL(write_txttile, (FILE *, pixel(*)[MAX_TILE_X]));

#define FUZZ

#define ABS(x) ((x) > 0 ? (x) : (-(x)))
#define MAX3(x, y, z)  ((x) > (y) ?  ((x) > (z) ? (x) : (z)) : \
				     ((y) > (z) ? (y) : (z)))

/*
 * ALI
 *
 * Support for 4096 colours.
 *
 * Issues:
 *	- NetHack 3.2/3.3 .TXT readers have a bug which causes them
 *	  to overflow the "c" array in read_text_colormap if more than
 *	  one character is encountered in the set A-Za-z0-9 where it
 *	  is expecting a colour key. This is because no maximum field
 *	  width has been specified for the %[...] scan format.
 *
 * Design goals:
 *	- Capable of reading and writing 62/4096 colour files
 *	- Writes 62 files which NetHack 3.2/3.3 can read.
 *	- Writes 4096 files which NetHack 3.2/3.3 fails gracefully on.
 *
 * We achieve this by introducing two new legal characters "_" and "$".
 * This brings the number of legal characters to 64 which is a nice round
 * number and also means that as long as we arrange for the first colour
 * key in a 4096 file to start with "_", 3.2/3.3 readers will fail without
 * crashing. Instead they will see no valid colour map. The error message
 * thus generated (no colormap set yet) is admittedly not very informative.
 */

/* Ugh.  DICE doesn't like %[A-Z], so we have to spell it out... */
#define FORMAT_STRING \
"%2[_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789$] = (%d, %d, %d) "

static char bysx2char(i)
int i;
{
	char c;
	if (!i) c = '_';
	else if (i < 27) c = 'A' + i - 1;
	else if (i < 53) c = 'a' + i - 27;
	else if (i < 63) c = '0' + i - 53;
	else c = '$';
	return c;
}

static int char2bysx(c)
char c;
{
	int i;
	if ( c == '_' ) i = 0;
	else if (c >= 'A' && c <= 'Z') i = c - 'A' + 1;
	else if (c >= 'a' && c <= 'z') i = c - 'a' + 27;
	else if (c >= '0' && c <= '9') i = c - '0' + 53;
	else if ( c == '$' ) i = 63;
	else i = -1;
	return i;
}

static void
read_text_colormap(txtfile)
FILE *txtfile;
{
	int i, n, r, g, b;
	char c[3];

	for (i = 0; i < TEXTCOLORMAPSPACE; i++)
		color_index[i] = -1;

	num_colors = 0;
	while (fscanf(txtfile, FORMAT_STRING, c, &r, &g, &b) == 4) {
		if (c[1])
		    n=char2bysx(c[0]) * 64 + char2bysx(c[1]);
		else
		    n=char2bysx(c[0]);
		if (n >= 0 && n < TEXTCOLORMAPSPACE)
			color_index[n] = num_colors;
		else
		{
			Fprintf(stderr, "error: Illegal color in colormap %s\n",
			  c);
			continue;
		}
		ColorMap[CM_RED][num_colors] = r;
		ColorMap[CM_GREEN][num_colors] = g;
		ColorMap[CM_BLUE][num_colors] = b;
		num_colors++;
	}
	colorsinmap = num_colors;
}

#undef FORMAT_STRING

static boolean
write_text_colormap(txtfile)
FILE *txtfile;
{
	int i;
	char c[3]="?";

	num_colors = colorsinmainmap;
	if (num_colors > MAXCOLORMAPSIZE) {
		Fprintf(stderr, "too many colors (%d)\n", num_colors);
		return FALSE;
	}
	for (i = 0; i < num_colors; i++) {
		if (num_colors > 62)
		{
			c[0] = bysx2char(i / 64);
			c[1] = bysx2char(i % 64);
		}
		else
			c[0] = bysx2char(i + 1);

		strcpy(charcolors[i], c);
		Fprintf(txtfile, "%s = (%d, %d, %d)\n", c,
						(int)MainColorMap[CM_RED][i],
						(int)MainColorMap[CM_GREEN][i],
						(int)MainColorMap[CM_BLUE][i]);
	}
	return TRUE;
}

static boolean
peek_txttile_info(txtfile, ttype, number, name)
FILE *txtfile;
char ttype[BUFSZ];
int *number;
char name[BUFSZ];
{
	long offset;
	int retval;
	offset = ftell(txtfile);
	retval = fscanf(txtfile, "# %s %d (%[^)])", ttype, number, name) == 3;
	(void) fseek(txtfile, offset, SEEK_SET);
	return retval;
}

static boolean
read_txttile_info(txtfile, pixels, ttype, number, name)
FILE *txtfile;
pixel (*pixels)[MAX_TILE_X];
char ttype[BUFSZ];
int *number;
char name[BUFSZ];
{
	int i, j, k, n;
	const char *fmt_string;
	char c[3];

	if (fscanf(txtfile, "# %s %d (%[^)])", ttype, number, name) <= 0) {
		return FALSE;
	}
	
	/* look for non-whitespace at each stage */
	if (fscanf(txtfile, "%1s", c) < 0) {
		Fprintf(stderr, "unexpected EOF\n");
		return FALSE;
	}
	if (c[0] != '{') {
		Fprintf(stderr, "didn't find expected '{'\n");
		return FALSE;
	}
	fmt_string = colorsinmap > 64 ? "%2s" : "%1s";
	for (j = 0; j < tile_y; j++) {
		for (i = 0; i < tile_x; i++) {
			if (fscanf(txtfile, fmt_string, c) < 0) {
				Fprintf(stderr, "unexpected EOF\n");
				return FALSE;
			}
			if (c[1])
				n=char2bysx(c[0]) * 64 + char2bysx(c[1]);
			else
				n=char2bysx(c[0]);
			if (n >= 0 && n < TEXTCOLORMAPSPACE)
				k = color_index[n];
			else
				k = -1;
			if (k == -1)
				Fprintf(stderr,
				  "%s %d (%s): color %s not in colormap!\n",
				  ttype, *number, name, c);
			else {
				pixels[j][i].r = ColorMap[CM_RED][k];
				pixels[j][i].g = ColorMap[CM_GREEN][k];
				pixels[j][i].b = ColorMap[CM_BLUE][k];
			}
		}
	}
	if (fscanf(txtfile, "%1s ", c) < 0) {
		Fprintf(stderr, "unexpected EOF\n");
		return FALSE;
	}
	if (c[0] != '}') {
		Fprintf(stderr, "didn't find expected '}'\n");
		return FALSE;
	}
#ifdef _DCC
	/* DICE again... it doesn't seem to eat whitespace after the } like
	 * it should, so we have to do so manually.
	 */
	while ((*c = fgetc(txtfile)) != EOF && isspace(*c))
		;
	ungetc(*c, txtfile);
#endif
	return TRUE;
}

static boolean
read_txttile(txtfile, pixels)
FILE *txtfile;
pixel (*pixels)[MAX_TILE_X];
{
	int ph, i;
	int tile_no;
	const char *p;
	char buf[BUFSZ], ttype[BUFSZ];

	if (!read_txttile_info(txtfile, pixels, ttype, &i, buf))
		return FALSE;
	
	ph = strcmp(ttype, "placeholder") == 0;

	if (!ph && strcmp(ttype,"tile") != 0)
		Fprintf(stderr,
			"Keyword \"%s\" unexpected for entry %d\n",
			ttype, i);

	if (tile_set != 0) {
	    /* check tile name, but not relative number, which will
	     * change when tiles are added
	     */
	    p = tilename(tile_set, tile_set_indx);
	    if (p && strcmp(p, buf)) {
		Fprintf(stderr, "warning: for tile %d (numbered %d) of %s,\n",
				tile_set_indx, i, TEXT_SETS(tile_set-1));
		Fprintf(stderr, "\tfound '%s' while expecting '%s'\n",
				buf, p);
	    }
	}
	tile_no=i;
	tile_set_indx++;

	if ( ph ) {
	    /* remember it for later */
	    memcpy( placeholder, pixels, sizeof(placeholder) );
	}
	return TRUE;
}

static void
write_txttile_info(txtfile, pixels, type, number, name)
FILE *txtfile;
pixel (*pixels)[MAX_TILE_X];
const char *type;
int number;
const char *name;
{
	int i, j, k;

	if (name)
	    Fprintf(txtfile, "# %s %d (%s)\n", type, number, name);
	else
	    Fprintf(txtfile, "# %s %d (null)\n", type, number);

	Fprintf(txtfile, "{\n");
	for (j = 0; j < tile_y; j++) {
		Fprintf(txtfile, "  ");
		for (i = 0; i < tile_x; i++) {
			for (k = 0; k < colorsinmainmap; k++) {
				if (MainColorMap[CM_RED][k] == pixels[j][i].r &&
				    MainColorMap[CM_GREEN][k] == pixels[j][i].g &&
				    MainColorMap[CM_BLUE][k] == pixels[j][i].b)
					break;
			}
			if (k >= colorsinmainmap)
				Fprintf(stderr, "color not in colormap!\n");
			(void) fputs(charcolors[k], txtfile);
		}
		Fprintf(txtfile, "\n");
	}
	Fprintf(txtfile, "}\n");
}

static void
write_txttile(txtfile, pixels)
FILE *txtfile;
pixel (*pixels)[MAX_TILE_X];
{
	const char *p;
	const char *type;

	if ( memcmp(placeholder, pixels, sizeof(placeholder)) == 0 )
	    type = "placeholder";
	else
	    type = "tile";

	if (tile_set == 0)
		p = "unknown";
	else
		p = tilename(tile_set, tile_set_indx);

	write_txttile_info(txtfile, pixels, type, tile_set_indx, p);
	tile_set_indx++;
}

/* initialize main colormap from globally accessed ColorMap */
void
init_colormap()
{
	int i;

	colorsinmainmap = colorsinmap;
	for (i = 0; i < colorsinmap; i++) {
		MainColorMap[CM_RED][i] = ColorMap[CM_RED][i];
		MainColorMap[CM_GREEN][i] = ColorMap[CM_GREEN][i];
		MainColorMap[CM_BLUE][i] = ColorMap[CM_BLUE][i];
	}
}

/* merge new colors from ColorMap into MainColorMap */
void
merge_colormap()
{
	int i, j;
	pixel trans_pix = DEFAULT_BACKGROUND;

	for (i = 0; i < colorsinmap; i++) {
#ifdef FUZZ
		int fuzz = -1, totalfuzz;
		pixval best_r, best_g, best_b;
#endif
		for (j = 0; j < colorsinmainmap; j++) {
		    if (MainColorMap[CM_RED][j] == ColorMap[CM_RED][i] &&
			MainColorMap[CM_GREEN][j] == ColorMap[CM_GREEN][i] &&
			MainColorMap[CM_BLUE][j] == ColorMap[CM_BLUE][i])
			    break;
		    
		    /* [AJA] make sure that we DO NOT convert any
		     *       colors into the transparency color.
		     */
		    if (MainColorMap[CM_RED][j] == trans_pix.r &&
			MainColorMap[CM_GREEN][j] == trans_pix.g &&
			MainColorMap[CM_BLUE][j] == trans_pix.b) {
			    continue;
		    }

#ifdef FUZZ
		    if ((fuzz == -1) || 
		    	((fuzz >= MAX3(
		    		ABS(MainColorMap[CM_RED][j] - ColorMap[CM_RED][i]),
		    		ABS(MainColorMap[CM_GREEN][j] - ColorMap[CM_GREEN][i]),
		    		ABS(MainColorMap[CM_BLUE][j] - ColorMap[CM_BLUE][i]))) &&
		    	(totalfuzz > (ABS(MainColorMap[CM_RED][j] - ColorMap[CM_RED][i]) +
		    		ABS(MainColorMap[CM_GREEN][j] - ColorMap[CM_GREEN][i]) +
		    		ABS(MainColorMap[CM_BLUE][j] - ColorMap[CM_BLUE][i]))))) {
		    	fuzz = MAX3(
		    		ABS(MainColorMap[CM_RED][j] - ColorMap[CM_RED][i]),
		    		ABS(MainColorMap[CM_GREEN][j] - ColorMap[CM_GREEN][i]),
		    		ABS(MainColorMap[CM_BLUE][j] - ColorMap[CM_BLUE][i]));
		    	totalfuzz = ABS(MainColorMap[CM_RED][j] - ColorMap[CM_RED][i]) +
		    		ABS(MainColorMap[CM_GREEN][j] - ColorMap[CM_GREEN][i]) +
		    		ABS(MainColorMap[CM_BLUE][j] - ColorMap[CM_BLUE][i]);
		    	best_r = MainColorMap[CM_RED][j];
		    	best_g = MainColorMap[CM_GREEN][j];
		    	best_b = MainColorMap[CM_BLUE][j];
		    }
#endif
		}
		if (j >= colorsinmainmap) {	/* new color */
#ifdef FUZZ
		    if (colorsinmainmap >= MAXCOLORMAPSIZE) {
			Fprintf(stderr,
			    "Changing %i,%i,%i => %i,%i,%i (fuzz max(%i), total(%i)).\n",
			    ColorMap[CM_RED][i],ColorMap[CM_GREEN][i],ColorMap[CM_BLUE][i],
			    best_r, best_g, best_b, fuzz, totalfuzz);
			ColorMap[CM_RED][i] = best_r;
			ColorMap[CM_GREEN][i] = best_g;
			ColorMap[CM_BLUE][i] = best_b;
		    } else {
			j = colorsinmainmap;
			MainColorMap[CM_RED][j] = ColorMap[CM_RED][i];
			MainColorMap[CM_GREEN][j] = ColorMap[CM_GREEN][i];
			MainColorMap[CM_BLUE][j] = ColorMap[CM_BLUE][i];
			colorsinmainmap++;
		    }
#else
		    if (colorsinmainmap >= MAXCOLORMAPSIZE) {
			Fprintf(stderr,
			    "Too many colors to merge -- excess ignored.\n");
		    }
		    j = colorsinmainmap;
		    MainColorMap[CM_RED][j] = ColorMap[CM_RED][i];
		    MainColorMap[CM_GREEN][j] = ColorMap[CM_GREEN][i];
		    MainColorMap[CM_BLUE][j] = ColorMap[CM_BLUE][i];
		    colorsinmainmap++;
#endif
		}
	}
}

/*
 * [ALI]
 *
 * This function accepts a slightly different syntax than read_txttile().
 * We insist that each line of pixels is encoded as one line in the file.
 * This restriction is necessary so that we can detect the tile width.
 * We also allow white space inside a two-character colour key, which is
 * not actually legal, but there seems little point complicating the code
 * just to detect it.
 */

static boolean
set_tile_size(txtfile)
FILE *txtfile;
{
	int i, j, ch;
	long pos;
	char c[2];

	pos = ftell(txtfile);

	if (fscanf(txtfile, "# %*s %*d (%*[^)]%c",c) <= 0 || c[0] != ')') {
		Fprintf(stderr, "no tiles in file\n");
		return FALSE;
	}
	
	if (fscanf(txtfile, "%1s", c) < 0) {
		Fprintf(stderr, "unexpected EOF\n");
		return FALSE;
	}
	if (c[0] != '{') {
		Fprintf(stderr, "didn't find expected '{'\n");
		return FALSE;
	}
	do
		ch = getc(txtfile);
	while (ch == '\n' || ch =='\r');
	for (j = 0; ; j++) {
		for(i = 0; ; i++) {
			while (ch == ' ' || ch == '\t')
				ch = getc(txtfile);
			if (ch == '_' || ch >= 'A' && ch <= 'Z' || ch >= 'a' &&
			  ch <= 'z' || ch >= '0' && ch <= '9' || ch == '$')
				ch = getc(txtfile);
			else
				break;
		}
		if (!i && ch == '}')
			break;
		if (ch != '\n' && ch != '\r') {
			Fprintf(stderr, "unexpected character %c\n",ch);
			return FALSE;
		}
		else
			ch = getc(txtfile);
		if (colorsinmap > 64) {
			if (i & 1) {
				Fprintf(stderr, "half a pixel?\n");
				return FALSE;
			}
			i /= 2;
		}
		if (tile_x < 0)
			tile_x = i;
		else if (tile_x != i)
		{
			Fprintf(stderr, "tile width mismatch %d != %d\n",
			  tile_x, i);
			return FALSE;
		}
	}
	if (tile_y < 0)
		tile_y = j;
	else if (tile_y != j)
	{
		Fprintf(stderr, "tile height mismatch %d != %d\n",
		  tile_y, j);
		return FALSE;
	}
	fseek(txtfile, pos, SEEK_SET);
	return TRUE;
}

boolean
read_text_file_colormap(filename)
const char *filename;
{
	FILE *fp;

	fp = fopen(filename, RDTMODE);
	if (fp == (FILE *)0) {
		Fprintf(stderr, "cannot open text file %s\n", filename);
		return FALSE;
	}
	read_text_colormap(fp);
	fclose(fp);
	return TRUE;
}

boolean
fopen_text_file(filename, type)
const char *filename;
const char *type;
{
	const char *p;
	int i;
	int write_mode;
	FILE *fp;

	if (!strcmp(type, RDTMODE))
		write_mode = FALSE;
	else if (!strcmp(type, WRTMODE)) {
		/* Seems like Mingw32's fscanf is confused by the CR/LF issue */
		/* Force text output in this case only */
#ifdef WIN32
		type = "w+";
#endif
		write_mode = TRUE;
		
	} else {
		Fprintf(stderr, "bad mode (%s) for fopen_text_file\n", type);
		return FALSE;
	}

	if ((write_mode ? out_file : in_file) != (FILE *)0) {
		Fprintf(stderr, "can only open one text file at at time\n");
		return FALSE;
	}

	fp = fopen(filename, type);
	if (fp == (FILE *)0) {
		Fprintf(stderr, "cannot open text file %s\n", filename);
		return FALSE;
	}

	p = rindex(filename, '/');
	if (p) p++;
	else p = filename;

	if (!write_mode) {
		in_file = fp;
		/* Fill placeholder with noise */
		if ( !placeholder_init ) {
		    placeholder_init++;
		    for ( i=0; i<sizeof(placeholder); i++ )
			((char*)placeholder)[i]=i%256;
		}

		read_text_colormap(in_file);
		if (!set_tile_size(in_file))
			return FALSE;
		if (!colorsinmainmap)
			init_colormap();
		else
			merge_colormap();
	} else {
		out_file = fp;
		if (!colorsinmainmap) {
			Fprintf(stderr, "no colormap set yet\n");
			return FALSE;
		}
		if (tile_x < 0 || tile_y < 0) {
			Fprintf(stderr, "no tile size set yet\n");
			return FALSE;
		}
		if (!write_text_colormap(out_file))
			return FALSE;
	}

	tile_set = 0;
	for (i = 0; i < SIZE(std_text_sets); i++) {
		if (!strcmp(p, TEXT_SETS(i)))
			tile_set = i+1;
	}
	tile_set_indx = 0;

	return TRUE;
}

boolean
peek_text_tile_info(ttype, number, name)
char ttype[BUFSZ];
int *number;
char name[BUFSZ];
{
	return(peek_txttile_info(in_file, ttype, number, name));
}

boolean
read_text_tile_info(pixels, ttype, number, name)
pixel (*pixels)[MAX_TILE_X];
char *ttype;
int *number;
char *name;
{
	return read_txttile_info(in_file, pixels, ttype, number, name);
}

boolean
read_text_tile(pixels)
pixel (*pixels)[MAX_TILE_X];
{
	return(read_txttile(in_file, pixels));
}

boolean
write_text_tile_info(pixels, ttype, number, name)
pixel (*pixels)[MAX_TILE_X];
const char *ttype;
int number;
const char *name;
{
	write_txttile_info(out_file, pixels, ttype, number, name);
	return TRUE;
}

boolean
write_text_tile(pixels)
pixel (*pixels)[MAX_TILE_X];
{
	write_txttile(out_file, pixels);
	return TRUE;
}

boolean
fclose_text_file()
{
	boolean ret = FALSE;
	if (in_file)
	{
		ret |= !!fclose(in_file);
		in_file = (FILE *)0;
	}
	if (out_file)
	{
		ret |= !!fclose(out_file);
		out_file = (FILE *)0;
	}
	return ret;
}
