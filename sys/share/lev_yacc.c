#ifndef lint
static char const 
yyrcsid[] = "$FreeBSD: src/usr.bin/yacc/skeleton.c,v 1.28 2000/01/17 02:04:06 bde Exp $";
#endif
#include <stdlib.h>
#define YYBYACC 1
#define YYMAJOR 1
#define YYMINOR 9
#define YYLEX yylex()
#define YYEMPTY -1
#define yyclearin (yychar=(YYEMPTY))
#define yyerrok (yyerrflag=0)
#define YYRECOVERING() (yyerrflag!=0)
static int yygrowstack();
#define YYPREFIX "yy"
/*	SCCS Id: @(#)lev_yacc.c	3.4	2000/01/17	*/
/*	Copyright (c) 1989 by Jean-Christophe Collet */
/* NetHack may be freely redistributed.  See license for details. */

/*
 * This file contains the Level Compiler code
 * It may handle special mazes & special room-levels
 */

/* In case we're using bison in AIX.  This definition must be
 * placed before any other C-language construct in the file
 * excluding comments and preprocessor directives (thanks IBM
 * for this wonderful feature...).
 *
 * Note: some cpps barf on this 'undefined control' (#pragma).
 * Addition of the leading space seems to prevent barfage for now,
 * and AIX will still see the directive.
 */
#ifdef _AIX
 #pragma alloca		/* keep leading space! */
#endif

#include "hack.h"
#include "sp_lev.h"

#define MAX_REGISTERS	10
#define ERR		(-1)
/* many types of things are put in chars for transference to NetHack.
 * since some systems will use signed chars, limit everybody to the
 * same number for portability.
 */
#define MAX_OF_TYPE	128

#define New(type)		\
	(type *) memset((genericptr_t)alloc(sizeof(type)), 0, sizeof(type))
#define NewTab(type, size)	(type **) alloc(sizeof(type *) * size)
#define Free(ptr)		free((genericptr_t)ptr)

extern void FDECL(yyerror, (const char *));
extern void FDECL(yywarning, (const char *));
extern int NDECL(yylex);
int NDECL(yyparse);

extern int FDECL(get_artifact_id, (char *));
extern int FDECL(get_floor_type, (CHAR_P));
extern int FDECL(get_room_type, (char *));
extern int FDECL(get_trap_type, (char *));
extern int FDECL(get_monster_id, (char *,CHAR_P));
extern int FDECL(get_object_id, (char *,CHAR_P));
extern boolean FDECL(check_monster_char, (CHAR_P));
extern boolean FDECL(check_object_char, (CHAR_P));
extern char FDECL(what_map_char, (CHAR_P));
extern void FDECL(scan_map, (char *));
extern void NDECL(wallify_map);
extern boolean NDECL(check_subrooms);
extern void FDECL(check_coord, (int,int,const char *));
extern void NDECL(store_part);
extern void NDECL(store_room);
extern boolean FDECL(write_level_file, (char *,splev *,specialmaze *));
extern void FDECL(free_rooms, (splev *));

static struct reg {
	int x1, y1;
	int x2, y2;
}		current_region;

static struct coord {
	int x;
	int y;
}		current_coord, current_align;

static struct size {
	int height;
	int width;
}		current_size;

char tmpmessage[256];
digpos *tmppass[32];
char *tmpmap[ROWNO];

digpos *tmpdig[MAX_OF_TYPE];
region *tmpreg[MAX_OF_TYPE];
lev_region *tmplreg[MAX_OF_TYPE];
door *tmpdoor[MAX_OF_TYPE];
drawbridge *tmpdb[MAX_OF_TYPE];
walk *tmpwalk[MAX_OF_TYPE];

room_door *tmprdoor[MAX_OF_TYPE];
trap *tmptrap[MAX_OF_TYPE];
monster *tmpmonst[MAX_OF_TYPE];
object *tmpobj[MAX_OF_TYPE];
altar *tmpaltar[MAX_OF_TYPE];
lad *tmplad[MAX_OF_TYPE];
stair *tmpstair[MAX_OF_TYPE];
gold *tmpgold[MAX_OF_TYPE];
engraving *tmpengraving[MAX_OF_TYPE];
fountain *tmpfountain[MAX_OF_TYPE];
sink *tmpsink[MAX_OF_TYPE];
pool *tmppool[MAX_OF_TYPE];

mazepart *tmppart[10];
room *tmproom[MAXNROFROOMS*2];
corridor *tmpcor[MAX_OF_TYPE];

static specialmaze maze;
static splev special_lev;
static lev_init init_lev;

static char olist[MAX_REGISTERS], mlist[MAX_REGISTERS];
static struct coord plist[MAX_REGISTERS];

int n_olist = 0, n_mlist = 0, n_plist = 0;

unsigned int nlreg = 0, nreg = 0, ndoor = 0, ntrap = 0, nmons = 0, nobj = 0;
unsigned int ndb = 0, nwalk = 0, npart = 0, ndig = 0, nlad = 0, nstair = 0;
unsigned int naltar = 0, ncorridor = 0, nrooms = 0, ngold = 0, nengraving = 0;
unsigned int nfountain = 0, npool = 0, nsink = 0, npass = 0;

static int lev_flags = 0;

unsigned int max_x_map, max_y_map;

static xchar in_room;

extern int fatal_error;
extern int want_warnings;
extern const char *fname;

typedef union
{
	int	i;
	char*	map;
	struct {
		xchar room;
		xchar wall;
		xchar door;
	} corpos;
} YYSTYPE;
#define YYERRCODE 256
#define CHAR 257
#define INTEGER 258
#define BOOLEAN 259
#define PERCENT 260
#define MESSAGE_ID 261
#define MAZE_ID 262
#define LEVEL_ID 263
#define LEV_INIT_ID 264
#define GEOMETRY_ID 265
#define NOMAP_ID 266
#define OBJECT_ID 267
#define COBJECT_ID 268
#define MONSTER_ID 269
#define TRAP_ID 270
#define DOOR_ID 271
#define DRAWBRIDGE_ID 272
#define MAZEWALK_ID 273
#define WALLIFY_ID 274
#define REGION_ID 275
#define FILLING 276
#define RANDOM_OBJECTS_ID 277
#define RANDOM_MONSTERS_ID 278
#define RANDOM_PLACES_ID 279
#define ALTAR_ID 280
#define LADDER_ID 281
#define STAIR_ID 282
#define NON_DIGGABLE_ID 283
#define NON_PASSWALL_ID 284
#define ROOM_ID 285
#define PORTAL_ID 286
#define TELEPRT_ID 287
#define BRANCH_ID 288
#define LEV 289
#define CHANCE_ID 290
#define CORRIDOR_ID 291
#define GOLD_ID 292
#define ENGRAVING_ID 293
#define FOUNTAIN_ID 294
#define POOL_ID 295
#define SINK_ID 296
#define NONE 297
#define RAND_CORRIDOR_ID 298
#define DOOR_STATE 299
#define LIGHT_STATE 300
#define CURSE_TYPE 301
#define ENGRAVING_TYPE 302
#define DIRECTION 303
#define RANDOM_TYPE 304
#define O_REGISTER 305
#define M_REGISTER 306
#define P_REGISTER 307
#define A_REGISTER 308
#define ALIGNMENT 309
#define LEFT_OR_RIGHT 310
#define CENTER 311
#define TOP_OR_BOT 312
#define ALTAR_TYPE 313
#define UP_OR_DOWN 314
#define SUBROOM_ID 315
#define NAME_ID 316
#define FLAGS_ID 317
#define FLAG_TYPE 318
#define MON_ATTITUDE 319
#define MON_ALERTNESS 320
#define MON_APPEARANCE 321
#define CONTAINED 322
#define STRING 323
#define MAP_ID 324
const short yylhs[] = {                                        -1,
    0,    0,   36,   36,   37,   37,   38,   39,   32,   23,
   23,   14,   14,   19,   19,   20,   20,   40,   40,   45,
   42,   42,   46,   46,   43,   43,   49,   49,   44,   44,
   51,   52,   52,   53,   53,   35,   50,   50,   56,   54,
   10,   10,   59,   59,   57,   57,   60,   60,   58,   58,
   55,   55,   61,   61,   61,   61,   61,   61,   61,   61,
   61,   61,   61,   61,   61,   62,   63,   76,   64,   75,
   75,   77,   15,   15,   13,   13,   12,   12,   31,   11,
   11,   41,   41,   78,   79,   79,   82,    1,    1,    2,
    2,   80,   80,   83,   83,   83,   47,   47,   48,   48,
   84,   86,   84,   81,   81,   87,   87,   87,   87,   87,
   87,   87,   87,   87,   87,   87,   87,   87,   87,   87,
   87,   87,   87,   87,   87,  102,   65,  101,  101,  103,
  103,  103,  103,  103,   66,   66,  105,  104,  106,  106,
  107,  107,  107,  107,  108,  108,  109,  110,  110,  111,
  111,  111,  113,   88,  112,  112,  114,   67,   89,   95,
   96,   97,   74,  115,   91,  116,   92,  117,  119,   93,
  120,   94,  118,  118,   22,   22,   69,   70,   71,   98,
   99,   90,   68,   72,   73,   25,   25,   25,   28,   28,
   28,   33,   33,   34,   34,    3,    3,    4,    4,   21,
   21,   21,  100,  100,  100,    5,    5,    6,    6,    7,
    7,    7,    8,    8,  123,   29,   26,    9,   85,   85,
   24,   27,   30,   16,   16,   17,   17,   18,   18,  122,
  121,
};
const short yylen[] = {                                         2,
    0,    1,    1,    2,    1,    1,    5,    7,    3,    0,
   13,    1,    1,    0,    3,    3,    1,    0,    2,    3,
    0,    2,    3,    3,    0,    1,    1,    2,    1,    1,
    1,    0,    2,    5,    5,    7,    2,    2,   12,   12,
    0,    2,    5,    1,    5,    1,    5,    1,    5,    1,
    0,    2,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    3,    3,    0,   11,    0,
    2,    2,    1,    1,    1,    1,    1,    1,    5,    1,
    1,    1,    2,    3,    1,    2,    5,    1,    1,    1,
    1,    0,    2,    3,    3,    3,    1,    3,    1,    3,
    1,    0,    4,    0,    2,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    0,   10,    0,    2,    2,
    2,    2,    2,    3,    2,    2,    0,    9,    1,    1,
    0,    7,    5,    5,    1,    1,    1,    1,    1,    0,
    2,    2,    0,    7,    0,    2,    2,    6,    7,    5,
    1,    5,    5,    0,    8,    0,    8,    0,    0,    8,
    0,    6,    0,    2,    1,   10,    3,    3,    3,    3,
    3,    8,    7,    5,    7,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    1,    1,    1,    1,    1,    0,
    2,    4,    1,    1,    1,    1,    1,    1,    1,    1,
    1,    1,    1,    1,    4,    4,    4,    4,    1,    1,
    1,    1,    1,    1,    1,    0,    1,    1,    1,    5,
    9,
};
const short yydefred[] = {                                      0,
    0,    0,    0,    0,    0,    2,    0,    5,    6,    0,
    0,    0,    0,    0,    4,  223,    0,    9,    0,    0,
    0,    0,    0,    0,   15,    0,    0,    0,    0,   21,
   80,   81,   79,    0,    0,    0,    0,   85,    7,    0,
   92,    0,   19,    0,   16,    0,   20,    0,   83,    0,
   86,    0,    0,    0,    0,    0,   22,   26,    0,   51,
   51,    0,   88,   89,    0,    0,    0,    0,    0,   93,
    0,    0,    0,    0,   31,    8,   29,    0,   28,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  161,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  106,  107,  109,  116,
  117,  122,  123,  121,  105,  108,  110,  111,  112,  113,
  114,  115,  118,  119,  120,  124,  125,  222,    0,   23,
  221,    0,   24,  199,    0,  198,    0,    0,   33,    0,
    0,    0,    0,    0,    0,   52,   53,   54,   55,   56,
   57,   58,   59,   60,   61,   62,   63,   64,   65,    0,
   91,   90,   87,   94,   96,  220,    0,   95,    0,  219,
  227,    0,  135,  136,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  206,  207,
    0,  205,    0,    0,  203,  204,    0,    0,    0,    0,
    0,    0,    0,  164,    0,  175,  180,  181,  166,  168,
  171,  224,  225,    0,    0,  177,   98,  100,  208,  209,
    0,    0,    0,    0,   73,   74,    0,   67,  179,  178,
   66,    0,    0,    0,  190,    0,  189,    0,  191,  187,
    0,  186,    0,  188,  197,    0,  196,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,  103,    0,    0,    0,    0,    0,  153,    0,    0,
  160,    0,    0,  212,    0,  210,    0,  211,  162,    0,
    0,    0,  163,    0,    0,    0,  184,  228,  229,    0,
   44,    0,    0,   46,    0,    0,    0,   35,   34,    0,
    0,  230,    0,  195,  194,  137,    0,  193,  192,    0,
  158,  155,  215,    0,    0,    0,    0,    0,    0,    0,
    0,    0,  169,  172,    0,    0,    0,    0,    0,    0,
    0,    0,  216,    0,  217,    0,    0,  159,    0,    0,
    0,  214,  213,  183,    0,    0,    0,    0,  185,    0,
   48,    0,    0,    0,   50,    0,    0,    0,   75,   76,
    0,   12,   13,   11,    0,  126,    0,  156,    0,    0,
  182,  218,    0,  165,  167,    0,  170,    0,    0,    0,
    0,    0,    0,   77,   78,    0,    0,  140,  139,    0,
  128,  157,    0,    0,    0,  174,   43,    0,    0,   45,
    0,    0,   36,   68,    0,  138,    0,    0,    0,    0,
    0,    0,   40,    0,   39,   70,  146,  145,  147,    0,
    0,    0,  129,  231,  202,    0,   47,   42,   49,    0,
    0,    0,  131,  132,    0,  133,  130,  176,    0,   71,
  149,  148,    0,    0,    0,  134,   72,    0,    0,  143,
  144,    0,  151,  152,  142,
};
const short yydgoto[] = {                                       3,
   65,  163,  266,  135,  211,  241,  307,  374,  308,  443,
   33,  416,  391,  394,  247,  234,  172,  320,   13,   25,
  401,  224,   21,  132,  263,  264,  129,  258,  259,  136,
    4,    5,  340,  336,  244,    6,    7,    8,    9,   28,
   39,   44,   56,   76,   29,   57,  130,  133,   58,   59,
   77,   78,  139,   60,   80,   61,  326,  387,  323,  383,
  146,  147,  148,  149,  150,  151,  152,  153,  154,  155,
  156,  157,  158,  159,  460,  446,  470,   40,   41,   50,
   69,   42,   70,  168,  169,  205,  115,  116,  117,  118,
  119,  120,  121,  122,  123,  124,  125,  126,  127,  225,
  437,  421,  453,  173,  364,  420,  436,  450,  451,  474,
  480,  367,  342,  398,  278,  280,  281,  407,  378,  282,
  226,  215,  216,
};
const short yysindex[] = {                                   -141,
   -1,   39,    0, -208, -208,    0, -141,    0,    0, -198,
 -198,   69, -128, -128,    0,    0,  101,    0, -157,  109,
  -88,  -88, -227,  136,    0,  -67,  133,  -94,  -88,    0,
    0,    0,    0, -157,  149, -127,  137,    0,    0,  -94,
    0, -130,    0, -184,    0,  -60,    0, -132,    0, -119,
    0,  141,  145,  146,  147,  -98,    0,    0, -256,    0,
    0,  162,    0,    0,  163,  150,  151,  152, -118,    0,
  -46,  -45, -262, -262,    0,    0,    0,  -78,    0, -230,
 -230,  -44, -129,  -46,  -45,  -35,  -43,  -43,  -43,  -43,
  156,  158,  160,    0,  161,  164,  165,  166,  167,  169,
  170,  171,  173,  174,  175,  176,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  177,    0,
    0,  191,    0,    0,  193,    0,  194,  182,    0,  183,
  184,  185,  186,  187,  188,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  195,
    0,    0,    0,    0,    0,    0,  -11,    0,    0,    0,
    0,  190,    0,    0,  192,  196, -250,   58,   58,  180,
   58,   58,  -36,  180,  180,  -34,  -34,  -34, -225,   58,
   58,  -46,  -45, -204, -204,  209, -232,   58,   -7,   58,
   58, -198,   -3,  208,  213, -229, -233, -249,    0,    0,
  214,    0,  168,  216,    0,    0,  217,    5,  220,  221,
  260,  268,   63,    0,  289,    0,    0,    0,    0,    0,
    0,    0,    0,  312,  320,    0,    0,    0,    0,    0,
  324,  334,  124,  347,    0,    0,  350,    0,    0,    0,
    0,  351,  127,  -35,    0,  310,    0,  358,    0,    0,
  315,    0,  374,    0,    0,  376,    0,   58,  189,  118,
  138,  395, -204, -194,  131,  198,  402,  409,  140,  416,
  418,  425,   58, -235,  -38,  -37,  426,  -32, -250, -204,
  437,    0,  222, -221,  223, -219,   58,    0,  390,  440,
    0,  227,  442,    0,  396,    0,  444,    0,    0,  445,
  232,  -34,    0,  -34,  -34,  -34,    0,    0,    0,  447,
    0,  234,  449,    0,  237,  450,  197,    0,    0,  468,
  487,    0,  439,    0,    0,    0,  443,    0,    0,  489,
    0,    0,    0, -250,  491, -262,  279, -222,  280,   75,
  495,  496,    0,    0, -198,  497,  -19,  498,   16,  499,
 -117, -214,    0,  500,    0,   58,  501,    0,  288,  503,
  455,    0,    0,    0,  505,  238, -198,  506,    0,  295,
    0, -132,  510,  297,    0,  298,  513, -224,    0,    0,
  528,    0,    0,    0,   77,    0, -198,    0,  541,  316,
    0,    0,  328,    0,    0,  277,    0,  554,  552,   16,
  556,  555, -198,    0,    0,  557, -224,    0,    0,  564,
    0,    0,  342,  565,  566,    0,    0, -129,  568,    0,
  343,  568,    0,    0, -253,    0,  569,  579,  362,  364,
  582,  365,    0,  584,    0,    0,    0,    0,    0,  585,
  589, -180,    0,    0,    0,  587,    0,    0,    0,  590,
 -236, -215,    0,    0, -198,    0,    0,    0, -198,    0,
    0,    0,  591,  592,  592,    0,    0, -215, -265,    0,
    0,  592,    0,    0,    0,
};
const short yyrindex[] = {                                    637,
    0,    0,    0, -153,  353,    0,  640,    0,    0,    0,
    0,    0, -131,  392,    0,    0,    0,    0,    0,    0,
  -81,  419,    0,  341,    0,    0,    0,    0,  380,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,   84,
    0,    0,    0,   92,    0,    0,    0,    0,    0,  525,
    0,    0,    0,    0,    0,  107,    0,    0,   95,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  202,    0,
    0,    0,    0,    0,    0,    0,    0,  126,    0,  400,
  431,    0,    0,    0,    0,    0,  583,  583,  583,  583,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  236,    0,
    0,  296,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,  482,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  560,    0,    0,  593,
    0,    0,    0,    0,    0,    0,    0,  627,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    7,
    0,    0,    0,  660,    0,    0,    0,    0,   81,    0,
    0,   81,    0,    0,    0,    0,   44,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,  181,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  142,  142,    0,    0,    0,    0,    0,
    0,  142,    0,    0,    0,
};
const short yygindex[] = {                                      0,
  265,  224,    0,  -63, -266, -185,  207,    0,    0,  218,
    0,  231,    0,    0,    0,    0,   99,    0,  644,  630,
    0, -168,  647,  459,    0,    0,  461,    0,    0,  -10,
    0,    0,    0,    0,  384,  666,    0,    0,    0,   24,
  634,    0,    0,    0,    0,    0,  -72,  -70,  616,    0,
    0,    0,    0,    0,  615,    0,    0,  269,    0,    0,
    0,    0,    0,    0,  611,  617,  618,  619,  620,    0,
    0,  623,  631,  632,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  430,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0, -165,
    0,    0,    0,  614,    0,    0,    0,    0,  242, -431,
 -376,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  -47,  -77,    0,
};
#define YYTABLESIZE 954
const short yytable[] = {                                      17,
   18,  322,  325,  223,  167,  218,  141,  243,  170,  242,
  137,  164,  214,  217,  165,  220,  221,  229,  230,  231,
  382,  471,  330,  131,  235,  236,  245,  128,   54,   31,
  475,  483,  232,  414,  249,  250,   87,   88,   89,   90,
  140,  134,  471,  127,  392,   30,  482,  447,  209,   96,
  448,  141,   43,  210,  265,  386,   10,  484,   55,  142,
   16,  104,  105,  106,  143,  144,  318,  472,  319,  449,
  260,  246,  261,   16,  255,  256,   32,  368,  233,  415,
   41,  372,  334,   82,  338,  145,  449,  303,  472,  393,
  373,   25,   52,   53,   27,  239,   11,  167,  481,  240,
   54,   16,  298,   16,  331,  485,   32,   14,   12,  304,
   14,   14,   14,  305,  306,  332,  167,  317,  369,  237,
    1,    2,  238,  304,   16,   30,   19,  305,  306,   10,
   55,  341,  219,   10,   10,   20,  227,  228,  463,  464,
  465,  150,   16,  351,   23,  352,  353,  354,   87,   88,
   89,   90,   91,   92,   93,   94,   95,   66,   67,   68,
   24,   96,   97,   98,   99,  100,   26,  101,  102,  103,
   37,   38,   27,  104,  105,  106,  170,   63,   64,   34,
   69,  161,  162,   18,   18,  389,  390,  175,  176,   35,
   36,  251,   46,   51,   48,   47,   62,  267,   71,   75,
  396,   84,   72,   73,   74,   82,   83,   84,   85,   86,
  128,  131,  138,  177,  160,  178,  171,  179,  180,  218,
  192,  181,  182,  183,  184,  328,  185,  186,  187,  419,
  188,  189,  190,  191,  193,   97,  194,  195,  203,  196,
  197,  198,  199,  200,  201,  202,  204,  206,  243,  207,
  248,  253,  222,  208,  222,  252,  254,  268,  269,  270,
  271,  166,  272,  273,  274,  321,  324,  212,  141,  141,
  213,  141,  141,  141,  141,  141,  141,  141,  141,  141,
  141,  141,  370,  335,  381,  339,  141,  141,  141,  141,
  141,  141,  141,  141,  141,   99,  141,  141,  141,  141,
  141,  141,  141,  275,  141,  127,  127,  276,  127,  127,
  127,  127,  127,  127,  127,  127,  127,  127,  127,  385,
  277,  141,  141,  127,  127,  127,  127,  127,  127,  127,
  127,  127,  279,  127,  127,  127,  127,  127,  127,  127,
   17,  127,   41,   41,  379,   82,   82,   41,   41,   41,
   41,   41,   14,   25,   25,  283,   27,   27,  127,  127,
   41,  212,   41,  284,  213,   41,  405,  285,   32,   32,
   41,   41,   41,   41,   41,   41,   41,  286,   41,   18,
  212,  287,   25,  213,  291,   27,  422,   30,   30,   25,
  288,   10,   27,  289,  290,   41,   41,   32,  418,   37,
  293,  294,  432,  150,  150,  295,  150,  150,  150,  150,
  150,  150,  150,  150,  150,  150,  150,  296,   18,  297,
  300,  150,  150,  150,  150,  150,  150,  150,  150,  150,
   38,  150,  150,  150,  150,  150,  150,  150,  302,  150,
  301,  467,   69,   69,  309,  311,  299,   69,   69,   69,
   69,   69,  312,  313,  476,  310,  150,  150,  477,  314,
   69,  315,   69,   84,   84,   69,   84,   84,  316,  327,
   69,   69,   69,   69,   69,   69,   69,  332,   69,  333,
  337,  101,  343,  344,  345,  346,  347,  348,  349,  350,
  355,  356,  357,  359,  358,   69,   69,   97,   97,  360,
   97,   97,   97,   97,   97,   97,   97,   97,   97,   97,
   97,  361,   97,   97,   97,   97,   97,   97,   97,   97,
   97,   97,   97,   97,  104,  102,   97,   97,   97,   97,
  362,  363,  366,   97,  369,  365,  371,  375,  376,  377,
  380,  384,  388,  395,  397,  399,  400,  402,  403,  406,
   97,  404,  408,  410,  411,  412,  413,   99,   99,  154,
   99,   99,   99,   99,   99,   99,   99,   99,   99,   99,
   99,  417,   99,   99,   99,   99,   99,   99,   99,   99,
   99,   99,   99,   99,  423,  425,   99,   99,   99,   99,
  426,  424,  200,   99,  427,  428,  430,  433,  431,  438,
  444,   17,   17,   17,   17,   17,   17,  435,  439,  440,
   99,  442,  452,   14,   14,   14,   14,   17,   17,  454,
  455,  456,  457,  458,  459,   17,  173,  468,  461,   14,
   14,   17,  462,  469,  478,  479,    1,   14,   17,    3,
  226,   18,   18,   14,   18,   18,  409,  434,   14,  445,
   14,  441,   10,   10,   10,   17,   18,   18,  466,  201,
   22,   37,   37,   45,   18,  262,  257,   14,   10,   10,
   18,  329,   15,   49,   79,   81,   10,   18,  429,  107,
   18,   18,   10,  292,   37,  108,  109,  110,  111,   10,
   37,  112,   38,   38,   18,   18,   18,   37,    0,  113,
  114,  174,  473,   18,    0,    0,   10,    0,    0,   18,
    0,    0,    0,    0,   37,   38,   18,    0,    0,    0,
    0,   38,    0,    0,    0,    0,    0,    0,   38,    0,
    0,    0,    0,   18,    0,    0,    0,    0,    0,    0,
    0,    0,    0,  101,  101,   38,  101,  101,  101,  101,
  101,  101,  101,  101,  101,  101,  101,    0,  101,  101,
  101,  101,  101,  101,  101,  101,    0,  101,  101,  101,
    0,    0,    0,  101,  101,  101,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,  104,  104,    0,  104,
  104,  104,  104,  104,  104,  104,  104,  104,  104,  104,
    0,    0,    0,    0,  104,  104,  104,  104,  104,    0,
  104,  104,  104,    0,    0,    0,  104,  104,  104,    0,
    0,  154,  154,    0,  154,  154,  154,  154,  154,  154,
  154,  154,  154,  154,  154,    0,    0,    0,    0,  154,
  154,  154,  154,  154,    0,  154,  154,  154,    0,    0,
    0,  154,  154,  154,  200,  200,    0,  200,  200,  200,
  200,  200,  200,  200,  200,  200,  200,  200,    0,    0,
    0,    0,  200,  200,  200,  200,  200,    0,  200,  200,
  200,    0,    0,    0,  200,  200,  200,    0,  173,  173,
    0,  173,  173,  173,  173,  173,  173,  173,  173,  173,
  173,  173,    0,    0,    0,    0,  173,  173,  173,  173,
  173,    0,  173,  173,  173,    0,    0,    0,  173,  173,
  173,  201,  201,    0,  201,  201,  201,  201,  201,  201,
  201,  201,  201,  201,  201,    0,    0,    0,    0,  201,
  201,  201,  201,  201,    0,  201,  201,  201,    0,    0,
    0,  201,  201,  201,
};
const short yycheck[] = {                                      10,
   11,   40,   40,   40,   40,   40,    0,   40,   86,  195,
   74,   84,  178,  179,   85,  181,  182,  186,  187,  188,
   40,  258,  289,  257,  190,  191,  259,  257,  285,  257,
  462,  297,  258,  258,  200,  201,  267,  268,  269,  270,
  271,  304,  258,    0,  259,   22,  478,  301,  299,  280,
  304,  282,   29,  304,  304,   40,   58,  323,  315,  290,
  323,  292,  293,  294,  295,  296,  302,  304,  304,  323,
  304,  304,  306,  323,  304,  305,  304,  344,  304,  304,
    0,  304,  304,    0,  304,  316,  323,  273,  304,  304,
  313,    0,  277,  278,    0,  300,   58,   40,  475,  304,
  285,  323,  268,  323,  290,  482,    0,  261,  317,  304,
  264,  265,  266,  308,  309,   41,   40,  283,   44,  192,
  262,  263,  193,  304,  323,    0,   58,  308,  309,  261,
  315,  297,  180,  265,  266,  264,  184,  185,  319,  320,
  321,    0,  323,  312,   44,  314,  315,  316,  267,  268,
  269,  270,  271,  272,  273,  274,  275,  277,  278,  279,
  318,  280,  281,  282,  283,  284,   58,  286,  287,  288,
  265,  266,  261,  292,  293,  294,  254,  310,  311,   44,
    0,  311,  312,  265,  266,  303,  304,   89,   90,  257,
   58,  202,   44,  324,   58,  323,  257,  208,   58,  298,
  366,    0,   58,   58,   58,   44,   44,   58,   58,   58,
  257,  257,  291,   58,  259,   58,  260,   58,   58,   40,
   44,   58,   58,   58,   58,  258,   58,   58,   58,  395,
   58,   58,   58,   58,   44,    0,   44,   44,   44,   58,
   58,   58,   58,   58,   58,   58,  258,   58,   40,   58,
  258,   44,  289,   58,  289,  259,   44,   44,   91,   44,
   44,  297,  258,   44,   44,  304,  304,  304,  262,  263,
  307,  265,  266,  267,  268,  269,  270,  271,  272,  273,
  274,  275,  346,  294,  304,  296,  280,  281,  282,  283,
  284,  285,  286,  287,  288,    0,  290,  291,  292,  293,
  294,  295,  296,   44,  298,  262,  263,   40,  265,  266,
  267,  268,  269,  270,  271,  272,  273,  274,  275,  304,
  258,  315,  316,  280,  281,  282,  283,  284,  285,  286,
  287,  288,   44,  290,  291,  292,  293,  294,  295,  296,
    0,  298,  262,  263,  355,  262,  263,  267,  268,  269,
  270,  271,    0,  262,  263,   44,  262,  263,  315,  316,
  280,  304,  282,   44,  307,  285,  377,   44,  262,  263,
  290,  291,  292,  293,  294,  295,  296,   44,  298,    0,
  304,  258,  291,  307,  258,  291,  397,  262,  263,  298,
   44,    0,  298,   44,   44,  315,  316,  291,  322,    0,
   91,   44,  413,  262,  263,   91,  265,  266,  267,  268,
  269,  270,  271,  272,  273,  274,  275,   44,    0,   44,
  303,  280,  281,  282,  283,  284,  285,  286,  287,  288,
    0,  290,  291,  292,  293,  294,  295,  296,   44,  298,
  303,  452,  262,  263,  314,   44,  258,  267,  268,  269,
  270,  271,   44,  314,  465,  258,  315,  316,  469,   44,
  280,   44,  282,  262,  263,  285,  265,  266,   44,   44,
  290,  291,  292,  293,  294,  295,  296,   41,  298,  258,
  258,    0,   93,   44,  258,   44,   91,   44,   44,  258,
   44,  258,   44,   44,  258,  315,  316,  262,  263,  303,
  265,  266,  267,  268,  269,  270,  271,  272,  273,  274,
  275,   44,  277,  278,  279,  280,  281,  282,  283,  284,
  285,  286,  287,  288,    0,   44,  291,  292,  293,  294,
   44,   93,   44,  298,   44,   93,  258,  258,   44,   44,
   44,   44,   44,   44,   44,  258,   44,   93,   44,   44,
  315,  314,  258,   44,  258,  258,   44,  262,  263,    0,
  265,  266,  267,  268,  269,  270,  271,  272,  273,  274,
  275,   44,  277,  278,  279,  280,  281,  282,  283,  284,
  285,  286,  287,  288,   44,  258,  291,  292,  293,  294,
  314,  276,    0,  298,   41,   44,   41,   41,   44,  258,
  258,  261,  262,  263,  264,  265,  266,   44,   44,   44,
  315,   44,   44,  261,  262,  263,  264,  277,  278,   41,
  259,  258,   41,  259,   41,  285,    0,   41,   44,  277,
  278,  291,   44,   44,   44,   44,    0,  285,  298,    0,
   58,  262,  263,  291,  265,  266,  382,  417,    5,  432,
  298,  428,  261,  262,  263,  315,  277,  278,  452,    0,
   14,  262,  263,   34,  285,  207,  206,  315,  277,  278,
  291,  288,    7,   40,   59,   61,  285,  298,  410,   69,
  262,  263,  291,  254,  285,   69,   69,   69,   69,  298,
  291,   69,  262,  263,  315,  277,  278,  298,   -1,   69,
   69,   88,  461,  285,   -1,   -1,  315,   -1,   -1,  291,
   -1,   -1,   -1,   -1,  315,  285,  298,   -1,   -1,   -1,
   -1,  291,   -1,   -1,   -1,   -1,   -1,   -1,  298,   -1,
   -1,   -1,   -1,  315,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,  262,  263,  315,  265,  266,  267,  268,
  269,  270,  271,  272,  273,  274,  275,   -1,  277,  278,
  279,  280,  281,  282,  283,  284,   -1,  286,  287,  288,
   -1,   -1,   -1,  292,  293,  294,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,  262,  263,   -1,  265,
  266,  267,  268,  269,  270,  271,  272,  273,  274,  275,
   -1,   -1,   -1,   -1,  280,  281,  282,  283,  284,   -1,
  286,  287,  288,   -1,   -1,   -1,  292,  293,  294,   -1,
   -1,  262,  263,   -1,  265,  266,  267,  268,  269,  270,
  271,  272,  273,  274,  275,   -1,   -1,   -1,   -1,  280,
  281,  282,  283,  284,   -1,  286,  287,  288,   -1,   -1,
   -1,  292,  293,  294,  262,  263,   -1,  265,  266,  267,
  268,  269,  270,  271,  272,  273,  274,  275,   -1,   -1,
   -1,   -1,  280,  281,  282,  283,  284,   -1,  286,  287,
  288,   -1,   -1,   -1,  292,  293,  294,   -1,  262,  263,
   -1,  265,  266,  267,  268,  269,  270,  271,  272,  273,
  274,  275,   -1,   -1,   -1,   -1,  280,  281,  282,  283,
  284,   -1,  286,  287,  288,   -1,   -1,   -1,  292,  293,
  294,  262,  263,   -1,  265,  266,  267,  268,  269,  270,
  271,  272,  273,  274,  275,   -1,   -1,   -1,   -1,  280,
  281,  282,  283,  284,   -1,  286,  287,  288,   -1,   -1,
   -1,  292,  293,  294,
};
#define YYFINAL 3
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 324
#if YYDEBUG
const char * const yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"'('","')'",0,0,"','",0,0,0,0,0,0,0,0,0,0,0,0,0,"':'",0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"'['",0,"']'",0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"CHAR",
"INTEGER","BOOLEAN","PERCENT","MESSAGE_ID","MAZE_ID","LEVEL_ID","LEV_INIT_ID",
"GEOMETRY_ID","NOMAP_ID","OBJECT_ID","COBJECT_ID","MONSTER_ID","TRAP_ID",
"DOOR_ID","DRAWBRIDGE_ID","MAZEWALK_ID","WALLIFY_ID","REGION_ID","FILLING",
"RANDOM_OBJECTS_ID","RANDOM_MONSTERS_ID","RANDOM_PLACES_ID","ALTAR_ID",
"LADDER_ID","STAIR_ID","NON_DIGGABLE_ID","NON_PASSWALL_ID","ROOM_ID",
"PORTAL_ID","TELEPRT_ID","BRANCH_ID","LEV","CHANCE_ID","CORRIDOR_ID","GOLD_ID",
"ENGRAVING_ID","FOUNTAIN_ID","POOL_ID","SINK_ID","NONE","RAND_CORRIDOR_ID",
"DOOR_STATE","LIGHT_STATE","CURSE_TYPE","ENGRAVING_TYPE","DIRECTION",
"RANDOM_TYPE","O_REGISTER","M_REGISTER","P_REGISTER","A_REGISTER","ALIGNMENT",
"LEFT_OR_RIGHT","CENTER","TOP_OR_BOT","ALTAR_TYPE","UP_OR_DOWN","SUBROOM_ID",
"NAME_ID","FLAGS_ID","FLAG_TYPE","MON_ATTITUDE","MON_ALERTNESS",
"MON_APPEARANCE","CONTAINED","STRING","MAP_ID",
};
const char * const yyrule[] = {
"$accept : file",
"file :",
"file : levels",
"levels : level",
"levels : level levels",
"level : maze_level",
"level : room_level",
"maze_level : maze_def flags lev_init messages regions",
"room_level : level_def flags lev_init messages rreg_init rooms corridors_def",
"level_def : LEVEL_ID ':' string",
"lev_init :",
"lev_init : LEV_INIT_ID ':' CHAR ',' CHAR ',' BOOLEAN ',' BOOLEAN ',' light_state ',' walled",
"walled : BOOLEAN",
"walled : RANDOM_TYPE",
"flags :",
"flags : FLAGS_ID ':' flag_list",
"flag_list : FLAG_TYPE ',' flag_list",
"flag_list : FLAG_TYPE",
"messages :",
"messages : message messages",
"message : MESSAGE_ID ':' STRING",
"rreg_init :",
"rreg_init : rreg_init init_rreg",
"init_rreg : RANDOM_OBJECTS_ID ':' object_list",
"init_rreg : RANDOM_MONSTERS_ID ':' monster_list",
"rooms :",
"rooms : roomlist",
"roomlist : aroom",
"roomlist : aroom roomlist",
"corridors_def : random_corridors",
"corridors_def : corridors",
"random_corridors : RAND_CORRIDOR_ID",
"corridors :",
"corridors : corridors corridor",
"corridor : CORRIDOR_ID ':' corr_spec ',' corr_spec",
"corridor : CORRIDOR_ID ':' corr_spec ',' INTEGER",
"corr_spec : '(' INTEGER ',' DIRECTION ',' door_pos ')'",
"aroom : room_def room_details",
"aroom : subroom_def room_details",
"subroom_def : SUBROOM_ID ':' room_type ',' light_state ',' subroom_pos ',' room_size ',' string roomfill",
"room_def : ROOM_ID ':' room_type ',' light_state ',' room_pos ',' room_align ',' room_size roomfill",
"roomfill :",
"roomfill : ',' BOOLEAN",
"room_pos : '(' INTEGER ',' INTEGER ')'",
"room_pos : RANDOM_TYPE",
"subroom_pos : '(' INTEGER ',' INTEGER ')'",
"subroom_pos : RANDOM_TYPE",
"room_align : '(' h_justif ',' v_justif ')'",
"room_align : RANDOM_TYPE",
"room_size : '(' INTEGER ',' INTEGER ')'",
"room_size : RANDOM_TYPE",
"room_details :",
"room_details : room_details room_detail",
"room_detail : room_name",
"room_detail : room_chance",
"room_detail : room_door",
"room_detail : monster_detail",
"room_detail : object_detail",
"room_detail : trap_detail",
"room_detail : altar_detail",
"room_detail : fountain_detail",
"room_detail : sink_detail",
"room_detail : pool_detail",
"room_detail : gold_detail",
"room_detail : engraving_detail",
"room_detail : stair_detail",
"room_name : NAME_ID ':' string",
"room_chance : CHANCE_ID ':' INTEGER",
"$$1 :",
"room_door : DOOR_ID ':' secret ',' door_state ',' door_wall ',' door_pos $$1 room_door_infos",
"room_door_infos :",
"room_door_infos : room_door_infos room_door_info",
"room_door_info : ',' string",
"secret : BOOLEAN",
"secret : RANDOM_TYPE",
"door_wall : DIRECTION",
"door_wall : RANDOM_TYPE",
"door_pos : INTEGER",
"door_pos : RANDOM_TYPE",
"maze_def : MAZE_ID ':' string ',' filling",
"filling : CHAR",
"filling : RANDOM_TYPE",
"regions : aregion",
"regions : aregion regions",
"aregion : map_definition reg_init map_details",
"map_definition : NOMAP_ID",
"map_definition : map_geometry MAP_ID",
"map_geometry : GEOMETRY_ID ':' h_justif ',' v_justif",
"h_justif : LEFT_OR_RIGHT",
"h_justif : CENTER",
"v_justif : TOP_OR_BOT",
"v_justif : CENTER",
"reg_init :",
"reg_init : reg_init init_reg",
"init_reg : RANDOM_OBJECTS_ID ':' object_list",
"init_reg : RANDOM_PLACES_ID ':' place_list",
"init_reg : RANDOM_MONSTERS_ID ':' monster_list",
"object_list : object",
"object_list : object ',' object_list",
"monster_list : monster",
"monster_list : monster ',' monster_list",
"place_list : place",
"$$2 :",
"place_list : place $$2 ',' place_list",
"map_details :",
"map_details : map_details map_detail",
"map_detail : monster_detail",
"map_detail : object_detail",
"map_detail : door_detail",
"map_detail : trap_detail",
"map_detail : drawbridge_detail",
"map_detail : region_detail",
"map_detail : stair_region",
"map_detail : portal_region",
"map_detail : teleprt_region",
"map_detail : branch_region",
"map_detail : altar_detail",
"map_detail : fountain_detail",
"map_detail : mazewalk_detail",
"map_detail : wallify_detail",
"map_detail : ladder_detail",
"map_detail : stair_detail",
"map_detail : gold_detail",
"map_detail : engraving_detail",
"map_detail : diggable_detail",
"map_detail : passwall_detail",
"$$3 :",
"monster_detail : MONSTER_ID chance ':' monster_c ',' m_name ',' coordinate $$3 monster_infos",
"monster_infos :",
"monster_infos : monster_infos monster_info",
"monster_info : ',' string",
"monster_info : ',' MON_ATTITUDE",
"monster_info : ',' MON_ALERTNESS",
"monster_info : ',' alignment",
"monster_info : ',' MON_APPEARANCE string",
"object_detail : OBJECT_ID object_desc",
"object_detail : COBJECT_ID object_desc",
"$$4 :",
"object_desc : chance ':' object_c ',' o_name $$4 ',' object_where object_infos",
"object_where : coordinate",
"object_where : CONTAINED",
"object_infos :",
"object_infos : ',' curse_state ',' monster_id ',' enchantment optional_name",
"object_infos : ',' curse_state ',' enchantment optional_name",
"object_infos : ',' monster_id ',' enchantment optional_name",
"curse_state : RANDOM_TYPE",
"curse_state : CURSE_TYPE",
"monster_id : STRING",
"enchantment : RANDOM_TYPE",
"enchantment : INTEGER",
"optional_name :",
"optional_name : ',' NONE",
"optional_name : ',' STRING",
"$$5 :",
"door_detail : DOOR_ID ':' door_state ',' coordinate $$5 door_infos",
"door_infos :",
"door_infos : door_infos door_info",
"door_info : ',' string",
"trap_detail : TRAP_ID chance ':' trap_name ',' coordinate",
"drawbridge_detail : DRAWBRIDGE_ID ':' coordinate ',' DIRECTION ',' door_state",
"mazewalk_detail : MAZEWALK_ID ':' coordinate ',' DIRECTION",
"wallify_detail : WALLIFY_ID",
"ladder_detail : LADDER_ID ':' coordinate ',' UP_OR_DOWN",
"stair_detail : STAIR_ID ':' coordinate ',' UP_OR_DOWN",
"$$6 :",
"stair_region : STAIR_ID ':' lev_region $$6 ',' lev_region ',' UP_OR_DOWN",
"$$7 :",
"portal_region : PORTAL_ID ':' lev_region $$7 ',' lev_region ',' string",
"$$8 :",
"$$9 :",
"teleprt_region : TELEPRT_ID ':' lev_region $$8 ',' lev_region $$9 teleprt_detail",
"$$10 :",
"branch_region : BRANCH_ID ':' lev_region $$10 ',' lev_region",
"teleprt_detail :",
"teleprt_detail : ',' UP_OR_DOWN",
"lev_region : region",
"lev_region : LEV '(' INTEGER ',' INTEGER ',' INTEGER ',' INTEGER ')'",
"fountain_detail : FOUNTAIN_ID ':' coordinate",
"sink_detail : SINK_ID ':' coordinate",
"pool_detail : POOL_ID ':' coordinate",
"diggable_detail : NON_DIGGABLE_ID ':' region",
"passwall_detail : NON_PASSWALL_ID ':' region",
"region_detail : REGION_ID ':' region ',' light_state ',' room_type prefilled",
"altar_detail : ALTAR_ID ':' coordinate ',' alignment ',' altar_type",
"gold_detail : GOLD_ID ':' amount ',' coordinate",
"engraving_detail : ENGRAVING_ID ':' coordinate ',' engraving_type ',' string",
"monster_c : monster",
"monster_c : RANDOM_TYPE",
"monster_c : m_register",
"object_c : object",
"object_c : RANDOM_TYPE",
"object_c : o_register",
"m_name : string",
"m_name : RANDOM_TYPE",
"o_name : string",
"o_name : RANDOM_TYPE",
"trap_name : string",
"trap_name : RANDOM_TYPE",
"room_type : string",
"room_type : RANDOM_TYPE",
"prefilled :",
"prefilled : ',' FILLING",
"prefilled : ',' FILLING ',' BOOLEAN",
"coordinate : coord",
"coordinate : p_register",
"coordinate : RANDOM_TYPE",
"door_state : DOOR_STATE",
"door_state : RANDOM_TYPE",
"light_state : LIGHT_STATE",
"light_state : RANDOM_TYPE",
"alignment : ALIGNMENT",
"alignment : a_register",
"alignment : RANDOM_TYPE",
"altar_type : ALTAR_TYPE",
"altar_type : RANDOM_TYPE",
"p_register : P_REGISTER '[' INTEGER ']'",
"o_register : O_REGISTER '[' INTEGER ']'",
"m_register : M_REGISTER '[' INTEGER ']'",
"a_register : A_REGISTER '[' INTEGER ']'",
"place : coord",
"place : NONE",
"monster : CHAR",
"object : CHAR",
"string : STRING",
"amount : INTEGER",
"amount : RANDOM_TYPE",
"chance :",
"chance : PERCENT",
"engraving_type : ENGRAVING_TYPE",
"engraving_type : RANDOM_TYPE",
"coord : '(' INTEGER ',' INTEGER ')'",
"region : '(' INTEGER ',' INTEGER ',' INTEGER ',' INTEGER ')'",
};
#endif
#if YYDEBUG
#include <stdio.h>
#endif
#ifdef YYSTACKSIZE
#undef YYMAXDEPTH
#define YYMAXDEPTH YYSTACKSIZE
#else
#ifdef YYMAXDEPTH
#define YYSTACKSIZE YYMAXDEPTH
#else
#define YYSTACKSIZE 10000
#define YYMAXDEPTH 10000
#endif
#endif
#define YYINITSTACKSIZE 200
int yydebug;
int yynerrs;
int yyerrflag;
int yychar;
short *yyssp;
YYSTYPE *yyvsp;
YYSTYPE yyval;
YYSTYPE yylval;
short *yyss;
short *yysslim;
YYSTYPE *yyvs;
int yystacksize;

/*lev_comp.y*/
/* allocate initial stack or double stack size, up to YYMAXDEPTH */
static int yygrowstack()
{
    int newsize, i;
    short *newss;
    YYSTYPE *newvs;

    if ((newsize = yystacksize) == 0)
        newsize = YYINITSTACKSIZE;
    else if (newsize >= YYMAXDEPTH)
        return -1;
    else if ((newsize *= 2) > YYMAXDEPTH)
        newsize = YYMAXDEPTH;
    i = yyssp - yyss;
    newss = yyss ? (short *)realloc(yyss, newsize * sizeof *newss) :
      (short *)malloc(newsize * sizeof *newss);
    if (newss == NULL)
        return -1;
    yyss = newss;
    yyssp = newss + i;
    newvs = yyvs ? (YYSTYPE *)realloc(yyvs, newsize * sizeof *newvs) :
      (YYSTYPE *)malloc(newsize * sizeof *newvs);
    if (newvs == NULL)
        return -1;
    yyvs = newvs;
    yyvsp = newvs + i;
    yystacksize = newsize;
    yysslim = yyss + newsize - 1;
    return 0;
}

#define YYABORT goto yyabort
#define YYREJECT goto yyabort
#define YYACCEPT goto yyaccept
#define YYERROR goto yyerrlab

#ifndef YYPARSE_PARAM
#if defined(__cplusplus) || __STDC__
#define YYPARSE_PARAM_ARG void
#define YYPARSE_PARAM_DECL
#else	/* ! ANSI-C/C++ */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif	/* ANSI-C/C++ */
#else	/* YYPARSE_PARAM */
#ifndef YYPARSE_PARAM_TYPE
#define YYPARSE_PARAM_TYPE void *
#endif
#if defined(__cplusplus) || __STDC__
#define YYPARSE_PARAM_ARG YYPARSE_PARAM_TYPE YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else	/* ! ANSI-C/C++ */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL YYPARSE_PARAM_TYPE YYPARSE_PARAM;
#endif	/* ANSI-C/C++ */
#endif	/* ! YYPARSE_PARAM */

int
yyparse (YYPARSE_PARAM_ARG)
    YYPARSE_PARAM_DECL
{
    register int yym, yyn, yystate;
#if YYDEBUG
    register const char *yys;

    if ((yys = getenv("YYDEBUG")))
    {
        yyn = *yys;
        if (yyn >= '0' && yyn <= '9')
            yydebug = yyn - '0';
    }
#endif

    yynerrs = 0;
    yyerrflag = 0;
    yychar = (-1);

    if (yyss == NULL && yygrowstack()) goto yyoverflow;
    yyssp = yyss;
    yyvsp = yyvs;
    *yyssp = yystate = 0;

yyloop:
    if ((yyn = yydefred[yystate])) goto yyreduce;
    if (yychar < 0)
    {
        if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, reading %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
    }
    if ((yyn = yysindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: state %d, shifting to state %d\n",
                    YYPREFIX, yystate, yytable[yyn]);
#endif
        if (yyssp >= yysslim && yygrowstack())
        {
            goto yyoverflow;
        }
        *++yyssp = yystate = yytable[yyn];
        *++yyvsp = yylval;
        yychar = (-1);
        if (yyerrflag > 0)  --yyerrflag;
        goto yyloop;
    }
    if ((yyn = yyrindex[yystate]) && (yyn += yychar) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yychar)
    {
        yyn = yytable[yyn];
        goto yyreduce;
    }
    if (yyerrflag) goto yyinrecovery;
#if defined(lint) || defined(__GNUC__)
    goto yynewerror;
#endif
yynewerror:
    yyerror("syntax error");
#if defined(lint) || defined(__GNUC__)
    goto yyerrlab;
#endif
yyerrlab:
    ++yynerrs;
yyinrecovery:
    if (yyerrflag < 3)
    {
        yyerrflag = 3;
        for (;;)
        {
            if ((yyn = yysindex[*yyssp]) && (yyn += YYERRCODE) >= 0 &&
                    yyn <= YYTABLESIZE && yycheck[yyn] == YYERRCODE)
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: state %d, error recovery shifting\
 to state %d\n", YYPREFIX, *yyssp, yytable[yyn]);
#endif
                if (yyssp >= yysslim && yygrowstack())
                {
                    goto yyoverflow;
                }
                *++yyssp = yystate = yytable[yyn];
                *++yyvsp = yylval;
                goto yyloop;
            }
            else
            {
#if YYDEBUG
                if (yydebug)
                    printf("%sdebug: error recovery discarding state %d\n",
                            YYPREFIX, *yyssp);
#endif
                if (yyssp <= yyss) goto yyabort;
                --yyssp;
                --yyvsp;
            }
        }
    }
    else
    {
        if (yychar == 0) goto yyabort;
#if YYDEBUG
        if (yydebug)
        {
            yys = 0;
            if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
            if (!yys) yys = "illegal-symbol";
            printf("%sdebug: state %d, error recovery discards token %d (%s)\n",
                    YYPREFIX, yystate, yychar, yys);
        }
#endif
        yychar = (-1);
        goto yyloop;
    }
yyreduce:
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: state %d, reducing by rule %d (%s)\n",
                YYPREFIX, yystate, yyn, yyrule[yyn]);
#endif
    yym = yylen[yyn];
    yyval = yyvsp[1-yym];
    switch (yyn)
    {
case 7:
{
			unsigned i;

			if (fatal_error > 0) {
				(void) fprintf(stderr,
				"%s : %d errors detected. No output created!\n",
					fname, fatal_error);
			} else {
				maze.flags = yyvsp[-3].i;
				(void) memcpy((genericptr_t)&(maze.init_lev),
						(genericptr_t)&(init_lev),
						sizeof(lev_init));
				maze.numpart = npart;
				maze.parts = NewTab(mazepart, npart);
				for(i=0;i<npart;i++)
				    maze.parts[i] = tmppart[i];
				if (!write_level_file(yyvsp[-4].map, (splev *)0, &maze)) {
					yyerror("Can't write output file!!");
					exit(EXIT_FAILURE);
				}
				npart = 0;
			}
			Free(yyvsp[-4].map);
		  }
break;
case 8:
{
			unsigned i;

			if (fatal_error > 0) {
			    (void) fprintf(stderr,
			      "%s : %d errors detected. No output created!\n",
					fname, fatal_error);
			} else {
				special_lev.flags = (long) yyvsp[-5].i;
				(void) memcpy(
					(genericptr_t)&(special_lev.init_lev),
					(genericptr_t)&(init_lev),
					sizeof(lev_init));
				special_lev.nroom = nrooms;
				special_lev.rooms = NewTab(room, nrooms);
				for(i=0; i<nrooms; i++)
				    special_lev.rooms[i] = tmproom[i];
				special_lev.ncorr = ncorridor;
				special_lev.corrs = NewTab(corridor, ncorridor);
				for(i=0; i<ncorridor; i++)
				    special_lev.corrs[i] = tmpcor[i];
				if (check_subrooms()) {
				    if (!write_level_file(yyvsp[-6].map, &special_lev,
							  (specialmaze *)0)) {
					yyerror("Can't write output file!!");
					exit(EXIT_FAILURE);
				    }
				}
				free_rooms(&special_lev);
				nrooms = 0;
				ncorridor = 0;
			}
			Free(yyvsp[-6].map);
		  }
break;
case 9:
{
			if (index(yyvsp[0].map, '.'))
			    yyerror("Invalid dot ('.') in level name.");
			if ((int) strlen(yyvsp[0].map) > 8)
			    yyerror("Level names limited to 8 characters.");
			yyval.map = yyvsp[0].map;
			special_lev.nrmonst = special_lev.nrobjects = 0;
			n_mlist = n_olist = 0;
		  }
break;
case 10:
{
			/* in case we're processing multiple files,
			   explicitly clear any stale settings */
			(void) memset((genericptr_t) &init_lev, 0,
					sizeof init_lev);
			init_lev.init_present = FALSE;
			yyval.i = 0;
		  }
break;
case 11:
{
			init_lev.init_present = TRUE;
			init_lev.fg = what_map_char((char) yyvsp[-10].i);
			if (init_lev.fg == INVALID_TYPE)
			    yyerror("Invalid foreground type.");
			init_lev.bg = what_map_char((char) yyvsp[-8].i);
			if (init_lev.bg == INVALID_TYPE)
			    yyerror("Invalid background type.");
			init_lev.smoothed = yyvsp[-6].i;
			init_lev.joined = yyvsp[-4].i;
			init_lev.lit = yyvsp[-2].i;
			init_lev.walled = yyvsp[0].i;
			yyval.i = 1;
		  }
break;
case 14:
{
			yyval.i = 0;
		  }
break;
case 15:
{
			yyval.i = lev_flags;
			lev_flags = 0;	/* clear for next user */
		  }
break;
case 16:
{
			lev_flags |= yyvsp[-2].i;
		  }
break;
case 17:
{
			lev_flags |= yyvsp[0].i;
		  }
break;
case 20:
{
			int i, j;

			i = (int) strlen(yyvsp[0].map) + 1;
			j = (int) strlen(tmpmessage);
			if (i + j > 255) {
			   yyerror("Message string too long (>256 characters)");
			} else {
			    if (j) tmpmessage[j++] = '\n';
			    (void) strncpy(tmpmessage+j, yyvsp[0].map, i - 1);
			    tmpmessage[j + i - 1] = 0;
			}
			Free(yyvsp[0].map);
		  }
break;
case 23:
{
			if(special_lev.nrobjects) {
			    yyerror("Object registers already initialized!");
			} else {
			    special_lev.nrobjects = n_olist;
			    special_lev.robjects = (char *) alloc(n_olist);
			    (void) memcpy((genericptr_t)special_lev.robjects,
					  (genericptr_t)olist, n_olist);
			}
		  }
break;
case 24:
{
			if(special_lev.nrmonst) {
			    yyerror("Monster registers already initialized!");
			} else {
			    special_lev.nrmonst = n_mlist;
			    special_lev.rmonst = (char *) alloc(n_mlist);
			    (void) memcpy((genericptr_t)special_lev.rmonst,
					  (genericptr_t)mlist, n_mlist);
			  }
		  }
break;
case 25:
{
			tmproom[nrooms] = New(room);
			tmproom[nrooms]->name = (char *) 0;
			tmproom[nrooms]->parent = (char *) 0;
			tmproom[nrooms]->rtype = 0;
			tmproom[nrooms]->rlit = 0;
			tmproom[nrooms]->xalign = ERR;
			tmproom[nrooms]->yalign = ERR;
			tmproom[nrooms]->x = 0;
			tmproom[nrooms]->y = 0;
			tmproom[nrooms]->w = 2;
			tmproom[nrooms]->h = 2;
			in_room = 1;
		  }
break;
case 31:
{
			tmpcor[0] = New(corridor);
			tmpcor[0]->src.room = -1;
			ncorridor = 1;
		  }
break;
case 34:
{
			tmpcor[ncorridor] = New(corridor);
			tmpcor[ncorridor]->src.room = yyvsp[-2].corpos.room;
			tmpcor[ncorridor]->src.wall = yyvsp[-2].corpos.wall;
			tmpcor[ncorridor]->src.door = yyvsp[-2].corpos.door;
			tmpcor[ncorridor]->dest.room = yyvsp[0].corpos.room;
			tmpcor[ncorridor]->dest.wall = yyvsp[0].corpos.wall;
			tmpcor[ncorridor]->dest.door = yyvsp[0].corpos.door;
			ncorridor++;
			if (ncorridor >= MAX_OF_TYPE) {
				yyerror("Too many corridors in level!");
				ncorridor--;
			}
		  }
break;
case 35:
{
			tmpcor[ncorridor] = New(corridor);
			tmpcor[ncorridor]->src.room = yyvsp[-2].corpos.room;
			tmpcor[ncorridor]->src.wall = yyvsp[-2].corpos.wall;
			tmpcor[ncorridor]->src.door = yyvsp[-2].corpos.door;
			tmpcor[ncorridor]->dest.room = -1;
			tmpcor[ncorridor]->dest.wall = yyvsp[0].i;
			ncorridor++;
			if (ncorridor >= MAX_OF_TYPE) {
				yyerror("Too many corridors in level!");
				ncorridor--;
			}
		  }
break;
case 36:
{
			if ((unsigned) yyvsp[-5].i >= nrooms)
			    yyerror("Wrong room number!");
			yyval.corpos.room = yyvsp[-5].i;
			yyval.corpos.wall = yyvsp[-3].i;
			yyval.corpos.door = yyvsp[-1].i;
		  }
break;
case 37:
{
			store_room();
		  }
break;
case 38:
{
			store_room();
		  }
break;
case 39:
{
			tmproom[nrooms] = New(room);
			tmproom[nrooms]->parent = yyvsp[-1].map;
			tmproom[nrooms]->name = (char *) 0;
			tmproom[nrooms]->rtype = yyvsp[-9].i;
			tmproom[nrooms]->rlit = yyvsp[-7].i;
			tmproom[nrooms]->filled = yyvsp[0].i;
			tmproom[nrooms]->xalign = ERR;
			tmproom[nrooms]->yalign = ERR;
			tmproom[nrooms]->x = current_coord.x;
			tmproom[nrooms]->y = current_coord.y;
			tmproom[nrooms]->w = current_size.width;
			tmproom[nrooms]->h = current_size.height;
			in_room = 1;
		  }
break;
case 40:
{
			tmproom[nrooms] = New(room);
			tmproom[nrooms]->name = (char *) 0;
			tmproom[nrooms]->parent = (char *) 0;
			tmproom[nrooms]->rtype = yyvsp[-9].i;
			tmproom[nrooms]->rlit = yyvsp[-7].i;
			tmproom[nrooms]->filled = yyvsp[0].i;
			tmproom[nrooms]->xalign = current_align.x;
			tmproom[nrooms]->yalign = current_align.y;
			tmproom[nrooms]->x = current_coord.x;
			tmproom[nrooms]->y = current_coord.y;
			tmproom[nrooms]->w = current_size.width;
			tmproom[nrooms]->h = current_size.height;
			in_room = 1;
		  }
break;
case 41:
{
			yyval.i = 1;
		  }
break;
case 42:
{
			yyval.i = yyvsp[0].i;
		  }
break;
case 43:
{
			if ( yyvsp[-3].i < 1 || yyvsp[-3].i > 5 ||
			    yyvsp[-1].i < 1 || yyvsp[-1].i > 5 ) {
			    yyerror("Room position should be between 1 & 5!");
			} else {
			    current_coord.x = yyvsp[-3].i;
			    current_coord.y = yyvsp[-1].i;
			}
		  }
break;
case 44:
{
			current_coord.x = current_coord.y = ERR;
		  }
break;
case 45:
{
			if ( yyvsp[-3].i < 0 || yyvsp[-1].i < 0) {
			    yyerror("Invalid subroom position !");
			} else {
			    current_coord.x = yyvsp[-3].i;
			    current_coord.y = yyvsp[-1].i;
			}
		  }
break;
case 46:
{
			current_coord.x = current_coord.y = ERR;
		  }
break;
case 47:
{
			current_align.x = yyvsp[-3].i;
			current_align.y = yyvsp[-1].i;
		  }
break;
case 48:
{
			current_align.x = current_align.y = ERR;
		  }
break;
case 49:
{
			current_size.width = yyvsp[-3].i;
			current_size.height = yyvsp[-1].i;
		  }
break;
case 50:
{
			current_size.height = current_size.width = ERR;
		  }
break;
case 66:
{
			if (tmproom[nrooms]->name)
			    yyerror("This room already has a name!");
			else
			    tmproom[nrooms]->name = yyvsp[0].map;
		  }
break;
case 67:
{
			if (tmproom[nrooms]->chance)
			    yyerror("This room already assigned a chance!");
			else if (tmproom[nrooms]->rtype == OROOM)
			    yyerror("Only typed rooms can have a chance!");
			else if (yyvsp[0].i < 1 || yyvsp[0].i > 99)
			    yyerror("The chance is supposed to be percentile.");
			else
			    tmproom[nrooms]->chance = yyvsp[0].i;
		   }
break;
case 68:
{
			/* ERR means random here */
			if (yyvsp[-2].i == ERR && yyvsp[0].i != ERR) {
		     yyerror("If the door wall is random, so must be its pos!");
			    tmprdoor[ndoor] = (struct room_door *)0;
			} else {
			    tmprdoor[ndoor] = New(room_door);
			    tmprdoor[ndoor]->secret = yyvsp[-6].i;
			    tmprdoor[ndoor]->mask = yyvsp[-4].i;
			    tmprdoor[ndoor]->wall = yyvsp[-2].i;
			    tmprdoor[ndoor]->pos = yyvsp[0].i;
			    tmprdoor[ndoor]->arti_key = 0;
			}
		  }
break;
case 69:
{
			if (tmprdoor[ndoor]) {
			    ndoor++;
			    if (ndoor >= MAX_OF_TYPE) {
				    yyerror("Too many doors in room!");
				    ndoor--;
			    }
			}
		  }
break;
case 72:
{
			int token = get_artifact_id(yyvsp[0].map);
			if (token == ERR) {
			    char ebuf[100];
			    Sprintf(ebuf, "Undefined artifact key \"%s\"", yyvsp[0].map);
			    yyerror(ebuf);
			}
			else if (tmprdoor[ndoor])
			    tmprdoor[ndoor]->arti_key = token;
		  }
break;
case 79:
{
			maze.filling = (schar) yyvsp[0].i;
			if (index(yyvsp[-2].map, '.'))
			    yyerror("Invalid dot ('.') in level name.");
			if ((int) strlen(yyvsp[-2].map) > 8)
			    yyerror("Level names limited to 8 characters.");
			yyval.map = yyvsp[-2].map;
			in_room = 0;
			n_plist = n_mlist = n_olist = 0;
		  }
break;
case 80:
{
			yyval.i = get_floor_type((char)yyvsp[0].i);
		  }
break;
case 81:
{
			yyval.i = -1;
		  }
break;
case 84:
{
			store_part();
		  }
break;
case 85:
{
			tmppart[npart] = New(mazepart);
			tmppart[npart]->halign = 1;
			tmppart[npart]->valign = 1;
			tmppart[npart]->nrobjects = 0;
			tmppart[npart]->nloc = 0;
			tmppart[npart]->nrmonst = 0;
			tmppart[npart]->xsize = 1;
			tmppart[npart]->ysize = 1;
			tmppart[npart]->map = (char **) alloc(sizeof(char *));
			tmppart[npart]->map[0] = (char *) alloc(1);
			tmppart[npart]->map[0][0] = STONE;
			max_x_map = COLNO-1;
			max_y_map = ROWNO;
		  }
break;
case 86:
{
			tmppart[npart] = New(mazepart);
			tmppart[npart]->halign = yyvsp[-1].i % 10;
			tmppart[npart]->valign = yyvsp[-1].i / 10;
			tmppart[npart]->nrobjects = 0;
			tmppart[npart]->nloc = 0;
			tmppart[npart]->nrmonst = 0;
			scan_map(yyvsp[0].map);
			Free(yyvsp[0].map);
		  }
break;
case 87:
{
			yyval.i = yyvsp[-2].i + (yyvsp[0].i * 10);
		  }
break;
case 94:
{
			if (tmppart[npart]->nrobjects) {
			    yyerror("Object registers already initialized!");
			} else {
			    tmppart[npart]->robjects = (char *)alloc(n_olist);
			    (void) memcpy((genericptr_t)tmppart[npart]->robjects,
					  (genericptr_t)olist, n_olist);
			    tmppart[npart]->nrobjects = n_olist;
			}
		  }
break;
case 95:
{
			if (tmppart[npart]->nloc) {
			    yyerror("Location registers already initialized!");
			} else {
			    register int i;
			    tmppart[npart]->rloc_x = (char *) alloc(n_plist);
			    tmppart[npart]->rloc_y = (char *) alloc(n_plist);
			    for(i=0;i<n_plist;i++) {
				tmppart[npart]->rloc_x[i] = plist[i].x;
				tmppart[npart]->rloc_y[i] = plist[i].y;
			    }
			    tmppart[npart]->nloc = n_plist;
			}
		  }
break;
case 96:
{
			if (tmppart[npart]->nrmonst) {
			    yyerror("Monster registers already initialized!");
			} else {
			    tmppart[npart]->rmonst = (char *) alloc(n_mlist);
			    (void) memcpy((genericptr_t)tmppart[npart]->rmonst,
					  (genericptr_t)mlist, n_mlist);
			    tmppart[npart]->nrmonst = n_mlist;
			}
		  }
break;
case 97:
{
			if (n_olist < MAX_REGISTERS)
			    olist[n_olist++] = yyvsp[0].i;
			else
			    yyerror("Object list too long!");
		  }
break;
case 98:
{
			if (n_olist < MAX_REGISTERS)
			    olist[n_olist++] = yyvsp[-2].i;
			else
			    yyerror("Object list too long!");
		  }
break;
case 99:
{
			if (n_mlist < MAX_REGISTERS)
			    mlist[n_mlist++] = yyvsp[0].i;
			else
			    yyerror("Monster list too long!");
		  }
break;
case 100:
{
			if (n_mlist < MAX_REGISTERS)
			    mlist[n_mlist++] = yyvsp[-2].i;
			else
			    yyerror("Monster list too long!");
		  }
break;
case 101:
{
			if (n_plist < MAX_REGISTERS)
			    plist[n_plist++] = current_coord;
			else
			    yyerror("Location list too long!");
		  }
break;
case 102:
{
			if (n_plist < MAX_REGISTERS)
			    plist[n_plist++] = current_coord;
			else
			    yyerror("Location list too long!");
		  }
break;
case 126:
{
			tmpmonst[nmons] = New(monster);
			tmpmonst[nmons]->x = current_coord.x;
			tmpmonst[nmons]->y = current_coord.y;
			tmpmonst[nmons]->class = yyvsp[-4].i;
			tmpmonst[nmons]->peaceful = -1; /* no override */
			tmpmonst[nmons]->asleep = -1;
			tmpmonst[nmons]->align = - MAX_REGISTERS - 2;
			tmpmonst[nmons]->name.str = 0;
			tmpmonst[nmons]->appear = 0;
			tmpmonst[nmons]->appear_as.str = 0;
			tmpmonst[nmons]->chance = yyvsp[-6].i;
			tmpmonst[nmons]->id = NON_PM;
			if (!in_room)
			    check_coord(current_coord.x, current_coord.y,
					"Monster");
			if (yyvsp[-2].map) {
			    int token = get_monster_id(yyvsp[-2].map, (char) yyvsp[-4].i);
			    if (token == ERR)
				yywarning(
			      "Invalid monster name!  Making random monster.");
			    else
				tmpmonst[nmons]->id = token;
			    Free(yyvsp[-2].map);
			}
		  }
break;
case 127:
{
			if (++nmons >= MAX_OF_TYPE) {
			    yyerror("Too many monsters in room or mazepart!");
			    nmons--;
			}
		  }
break;
case 130:
{
			tmpmonst[nmons]->name.str = yyvsp[0].map;
		  }
break;
case 131:
{
			tmpmonst[nmons]->peaceful = yyvsp[0].i;
		  }
break;
case 132:
{
			tmpmonst[nmons]->asleep = yyvsp[0].i;
		  }
break;
case 133:
{
			tmpmonst[nmons]->align = yyvsp[0].i;
		  }
break;
case 134:
{
			tmpmonst[nmons]->appear = yyvsp[-1].i;
			tmpmonst[nmons]->appear_as.str = yyvsp[0].map;
		  }
break;
case 135:
{
		  }
break;
case 136:
{
			/* 1: is contents of next object with 2 */
			/* 2: is a container */
			/* 0: neither */
			tmpobj[nobj-1]->containment = 2;
		  }
break;
case 137:
{
			tmpobj[nobj] = New(object);
			tmpobj[nobj]->class = yyvsp[-2].i;
			tmpobj[nobj]->corpsenm = NON_PM;
			tmpobj[nobj]->curse_state = -1;
			tmpobj[nobj]->name.str = 0;
			tmpobj[nobj]->chance = yyvsp[-4].i;
			tmpobj[nobj]->id = -1;
			if (yyvsp[0].map) {
			    int token = get_object_id(yyvsp[0].map, yyvsp[-2].i);
			    if (token == ERR)
				yywarning(
				"Illegal object name!  Making random object.");
			     else
				tmpobj[nobj]->id = token;
			    Free(yyvsp[0].map);
			}
		  }
break;
case 138:
{
			if (++nobj >= MAX_OF_TYPE) {
			    yyerror("Too many objects in room or mazepart!");
			    nobj--;
			}
		  }
break;
case 139:
{
			tmpobj[nobj]->containment = 0;
			tmpobj[nobj]->x = current_coord.x;
			tmpobj[nobj]->y = current_coord.y;
			if (!in_room)
			    check_coord(current_coord.x, current_coord.y,
					"Object");
		  }
break;
case 140:
{
			tmpobj[nobj]->containment = 1;
			/* random coordinate, will be overridden anyway */
			tmpobj[nobj]->x = -MAX_REGISTERS-1;
			tmpobj[nobj]->y = -MAX_REGISTERS-1;
		  }
break;
case 141:
{
			tmpobj[nobj]->spe = -127;
	/* Note below: we're trying to make as many of these optional as
	 * possible.  We clearly can't make curse_state, enchantment, and
	 * monster_id _all_ optional, since ",random" would be ambiguous.
	 * We can't even just make enchantment mandatory, since if we do that
	 * alone, ",random" requires too much lookahead to parse.
	 */
		  }
break;
case 142:
{
		  }
break;
case 143:
{
		  }
break;
case 144:
{
		  }
break;
case 145:
{
			tmpobj[nobj]->curse_state = -1;
		  }
break;
case 146:
{
			tmpobj[nobj]->curse_state = yyvsp[0].i;
		  }
break;
case 147:
{
			int token = get_monster_id(yyvsp[0].map, (char)0);
			if (token == ERR)	/* "random" */
			    tmpobj[nobj]->corpsenm = NON_PM - 1;
			else
			    tmpobj[nobj]->corpsenm = token;
			Free(yyvsp[0].map);
		  }
break;
case 148:
{
			tmpobj[nobj]->spe = -127;
		  }
break;
case 149:
{
			tmpobj[nobj]->spe = yyvsp[0].i;
		  }
break;
case 151:
{
		  }
break;
case 152:
{
			tmpobj[nobj]->name.str = yyvsp[0].map;
		  }
break;
case 153:
{
			tmpdoor[ndoor] = New(door);
			tmpdoor[ndoor]->x = current_coord.x;
			tmpdoor[ndoor]->y = current_coord.y;
			tmpdoor[ndoor]->mask = yyvsp[-2].i;
			tmpdoor[ndoor]->arti_key = 0;
			if(current_coord.x >= 0 && current_coord.y >= 0 &&
			   tmpmap[current_coord.y][current_coord.x] != DOOR &&
			   tmpmap[current_coord.y][current_coord.x] != SDOOR)
			    yyerror("Door decl doesn't match the map");
		  }
break;
case 154:
{
			if (++ndoor >= MAX_OF_TYPE) {
			    yyerror("Too many doors in mazepart!");
			    ndoor--;
			}
		  }
break;
case 157:
{
			int token = get_artifact_id(yyvsp[0].map);
			if (token == ERR) {
			    char ebuf[100];
			    Sprintf(ebuf, "Undefined artifact key \"%s\"", yyvsp[0].map);
			    yyerror(ebuf);
			}
			else
			    tmpdoor[ndoor]->arti_key = token;
		  }
break;
case 158:
{
			tmptrap[ntrap] = New(trap);
			tmptrap[ntrap]->x = current_coord.x;
			tmptrap[ntrap]->y = current_coord.y;
			tmptrap[ntrap]->type = yyvsp[-2].i;
			tmptrap[ntrap]->chance = yyvsp[-4].i;
			if (!in_room)
			    check_coord(current_coord.x, current_coord.y,
					"Trap");
			if (++ntrap >= MAX_OF_TYPE) {
				yyerror("Too many traps in room or mazepart!");
				ntrap--;
			}
		  }
break;
case 159:
{
		        int x, y, dir;

			tmpdb[ndb] = New(drawbridge);
			x = tmpdb[ndb]->x = current_coord.x;
			y = tmpdb[ndb]->y = current_coord.y;
			/* convert dir from a DIRECTION to a DB_DIR */
			dir = yyvsp[-2].i;
			switch(dir) {
			case W_NORTH: dir = DB_NORTH; y--; break;
			case W_SOUTH: dir = DB_SOUTH; y++; break;
			case W_EAST:  dir = DB_EAST;  x++; break;
			case W_WEST:  dir = DB_WEST;  x--; break;
			default:
			    yyerror("Invalid drawbridge direction");
			    break;
			}
			tmpdb[ndb]->dir = dir;
			if (current_coord.x >= 0 && current_coord.y >= 0 &&
			    !IS_WALL(tmpmap[y][x])) {
			    char ebuf[60];
			    Sprintf(ebuf,
				    "Wall needed for drawbridge (%02d, %02d)",
				    current_coord.x, current_coord.y);
			    yyerror(ebuf);
			}

			if ( yyvsp[0].i == D_ISOPEN )
			    tmpdb[ndb]->db_open = 1;
			else if ( yyvsp[0].i == D_CLOSED )
			    tmpdb[ndb]->db_open = 0;
			else
			    yyerror("A drawbridge can only be open or closed!");
			ndb++;
			if (ndb >= MAX_OF_TYPE) {
				yyerror("Too many drawbridges in mazepart!");
				ndb--;
			}
		   }
break;
case 160:
{
			tmpwalk[nwalk] = New(walk);
			tmpwalk[nwalk]->x = current_coord.x;
			tmpwalk[nwalk]->y = current_coord.y;
			tmpwalk[nwalk]->dir = yyvsp[0].i;
			nwalk++;
			if (nwalk >= MAX_OF_TYPE) {
				yyerror("Too many mazewalks in mazepart!");
				nwalk--;
			}
		  }
break;
case 161:
{
			wallify_map();
		  }
break;
case 162:
{
			tmplad[nlad] = New(lad);
			tmplad[nlad]->x = current_coord.x;
			tmplad[nlad]->y = current_coord.y;
			tmplad[nlad]->up = yyvsp[0].i;
			if (!in_room)
			    check_coord(current_coord.x, current_coord.y,
					"Ladder");
			nlad++;
			if (nlad >= MAX_OF_TYPE) {
				yyerror("Too many ladders in mazepart!");
				nlad--;
			}
		  }
break;
case 163:
{
			tmpstair[nstair] = New(stair);
			tmpstair[nstair]->x = current_coord.x;
			tmpstair[nstair]->y = current_coord.y;
			tmpstair[nstair]->up = yyvsp[0].i;
			if (!in_room)
			    check_coord(current_coord.x, current_coord.y,
					"Stairway");
			nstair++;
			if (nstair >= MAX_OF_TYPE) {
				yyerror("Too many stairs in room or mazepart!");
				nstair--;
			}
		  }
break;
case 164:
{
			tmplreg[nlreg] = New(lev_region);
			tmplreg[nlreg]->in_islev = yyvsp[0].i;
			tmplreg[nlreg]->inarea.x1 = current_region.x1;
			tmplreg[nlreg]->inarea.y1 = current_region.y1;
			tmplreg[nlreg]->inarea.x2 = current_region.x2;
			tmplreg[nlreg]->inarea.y2 = current_region.y2;
		  }
break;
case 165:
{
			tmplreg[nlreg]->del_islev = yyvsp[-2].i;
			tmplreg[nlreg]->delarea.x1 = current_region.x1;
			tmplreg[nlreg]->delarea.y1 = current_region.y1;
			tmplreg[nlreg]->delarea.x2 = current_region.x2;
			tmplreg[nlreg]->delarea.y2 = current_region.y2;
			if(yyvsp[0].i)
			    tmplreg[nlreg]->rtype = LR_UPSTAIR;
			else
			    tmplreg[nlreg]->rtype = LR_DOWNSTAIR;
			tmplreg[nlreg]->rname.str = 0;
			nlreg++;
			if (nlreg >= MAX_OF_TYPE) {
				yyerror("Too many levregions in mazepart!");
				nlreg--;
			}
		  }
break;
case 166:
{
			tmplreg[nlreg] = New(lev_region);
			tmplreg[nlreg]->in_islev = yyvsp[0].i;
			tmplreg[nlreg]->inarea.x1 = current_region.x1;
			tmplreg[nlreg]->inarea.y1 = current_region.y1;
			tmplreg[nlreg]->inarea.x2 = current_region.x2;
			tmplreg[nlreg]->inarea.y2 = current_region.y2;
		  }
break;
case 167:
{
			tmplreg[nlreg]->del_islev = yyvsp[-2].i;
			tmplreg[nlreg]->delarea.x1 = current_region.x1;
			tmplreg[nlreg]->delarea.y1 = current_region.y1;
			tmplreg[nlreg]->delarea.x2 = current_region.x2;
			tmplreg[nlreg]->delarea.y2 = current_region.y2;
			tmplreg[nlreg]->rtype = LR_PORTAL;
			tmplreg[nlreg]->rname.str = yyvsp[0].map;
			nlreg++;
			if (nlreg >= MAX_OF_TYPE) {
				yyerror("Too many levregions in mazepart!");
				nlreg--;
			}
		  }
break;
case 168:
{
			tmplreg[nlreg] = New(lev_region);
			tmplreg[nlreg]->in_islev = yyvsp[0].i;
			tmplreg[nlreg]->inarea.x1 = current_region.x1;
			tmplreg[nlreg]->inarea.y1 = current_region.y1;
			tmplreg[nlreg]->inarea.x2 = current_region.x2;
			tmplreg[nlreg]->inarea.y2 = current_region.y2;
		  }
break;
case 169:
{
			tmplreg[nlreg]->del_islev = yyvsp[0].i;
			tmplreg[nlreg]->delarea.x1 = current_region.x1;
			tmplreg[nlreg]->delarea.y1 = current_region.y1;
			tmplreg[nlreg]->delarea.x2 = current_region.x2;
			tmplreg[nlreg]->delarea.y2 = current_region.y2;
		  }
break;
case 170:
{
			switch(yyvsp[0].i) {
			case -1: tmplreg[nlreg]->rtype = LR_TELE; break;
			case 0: tmplreg[nlreg]->rtype = LR_DOWNTELE; break;
			case 1: tmplreg[nlreg]->rtype = LR_UPTELE; break;
			}
			tmplreg[nlreg]->rname.str = 0;
			nlreg++;
			if (nlreg >= MAX_OF_TYPE) {
				yyerror("Too many levregions in mazepart!");
				nlreg--;
			}
		  }
break;
case 171:
{
			tmplreg[nlreg] = New(lev_region);
			tmplreg[nlreg]->in_islev = yyvsp[0].i;
			tmplreg[nlreg]->inarea.x1 = current_region.x1;
			tmplreg[nlreg]->inarea.y1 = current_region.y1;
			tmplreg[nlreg]->inarea.x2 = current_region.x2;
			tmplreg[nlreg]->inarea.y2 = current_region.y2;
		  }
break;
case 172:
{
			tmplreg[nlreg]->del_islev = yyvsp[0].i;
			tmplreg[nlreg]->delarea.x1 = current_region.x1;
			tmplreg[nlreg]->delarea.y1 = current_region.y1;
			tmplreg[nlreg]->delarea.x2 = current_region.x2;
			tmplreg[nlreg]->delarea.y2 = current_region.y2;
			tmplreg[nlreg]->rtype = LR_BRANCH;
			tmplreg[nlreg]->rname.str = 0;
			nlreg++;
			if (nlreg >= MAX_OF_TYPE) {
				yyerror("Too many levregions in mazepart!");
				nlreg--;
			}
		  }
break;
case 173:
{
			yyval.i = -1;
		  }
break;
case 174:
{
			yyval.i = yyvsp[0].i;
		  }
break;
case 175:
{
			yyval.i = 0;
		  }
break;
case 176:
{
/* This series of if statements is a hack for MSC 5.1.  It seems that its
   tiny little brain cannot compile if these are all one big if statement. */
			if (yyvsp[-7].i <= 0 || yyvsp[-7].i >= COLNO)
				yyerror("Region out of level range!");
			else if (yyvsp[-5].i < 0 || yyvsp[-5].i >= ROWNO)
				yyerror("Region out of level range!");
			else if (yyvsp[-3].i <= 0 || yyvsp[-3].i >= COLNO)
				yyerror("Region out of level range!");
			else if (yyvsp[-1].i < 0 || yyvsp[-1].i >= ROWNO)
				yyerror("Region out of level range!");
			current_region.x1 = yyvsp[-7].i;
			current_region.y1 = yyvsp[-5].i;
			current_region.x2 = yyvsp[-3].i;
			current_region.y2 = yyvsp[-1].i;
			yyval.i = 1;
		  }
break;
case 177:
{
			tmpfountain[nfountain] = New(fountain);
			tmpfountain[nfountain]->x = current_coord.x;
			tmpfountain[nfountain]->y = current_coord.y;
			if (!in_room)
			    check_coord(current_coord.x, current_coord.y,
					"Fountain");
			nfountain++;
			if (nfountain >= MAX_OF_TYPE) {
			    yyerror("Too many fountains in room or mazepart!");
			    nfountain--;
			}
		  }
break;
case 178:
{
			tmpsink[nsink] = New(sink);
			tmpsink[nsink]->x = current_coord.x;
			tmpsink[nsink]->y = current_coord.y;
			nsink++;
			if (nsink >= MAX_OF_TYPE) {
				yyerror("Too many sinks in room!");
				nsink--;
			}
		  }
break;
case 179:
{
			tmppool[npool] = New(pool);
			tmppool[npool]->x = current_coord.x;
			tmppool[npool]->y = current_coord.y;
			npool++;
			if (npool >= MAX_OF_TYPE) {
				yyerror("Too many pools in room!");
				npool--;
			}
		  }
break;
case 180:
{
			tmpdig[ndig] = New(digpos);
			tmpdig[ndig]->x1 = current_region.x1;
			tmpdig[ndig]->y1 = current_region.y1;
			tmpdig[ndig]->x2 = current_region.x2;
			tmpdig[ndig]->y2 = current_region.y2;
			ndig++;
			if (ndig >= MAX_OF_TYPE) {
				yyerror("Too many diggables in mazepart!");
				ndig--;
			}
		  }
break;
case 181:
{
			tmppass[npass] = New(digpos);
			tmppass[npass]->x1 = current_region.x1;
			tmppass[npass]->y1 = current_region.y1;
			tmppass[npass]->x2 = current_region.x2;
			tmppass[npass]->y2 = current_region.y2;
			npass++;
			if (npass >= 32) {
				yyerror("Too many passwalls in mazepart!");
				npass--;
			}
		  }
break;
case 182:
{
			tmpreg[nreg] = New(region);
			tmpreg[nreg]->x1 = current_region.x1;
			tmpreg[nreg]->y1 = current_region.y1;
			tmpreg[nreg]->x2 = current_region.x2;
			tmpreg[nreg]->y2 = current_region.y2;
			tmpreg[nreg]->rlit = yyvsp[-3].i;
			tmpreg[nreg]->rtype = yyvsp[-1].i;
			if(yyvsp[0].i & 1) tmpreg[nreg]->rtype += MAXRTYPE+1;
			tmpreg[nreg]->rirreg = ((yyvsp[0].i & 2) != 0);
			if(current_region.x1 > current_region.x2 ||
			   current_region.y1 > current_region.y2)
			   yyerror("Region start > end!");
			if(tmpreg[nreg]->rtype == VAULT &&
			   (tmpreg[nreg]->rirreg ||
			    (tmpreg[nreg]->x2 - tmpreg[nreg]->x1 != 1) ||
			    (tmpreg[nreg]->y2 - tmpreg[nreg]->y1 != 1)))
				yyerror("Vaults must be exactly 2x2!");
			if(want_warnings && !tmpreg[nreg]->rirreg &&
			   current_region.x1 > 0 && current_region.y1 > 0 &&
			   current_region.x2 < (int)max_x_map &&
			   current_region.y2 < (int)max_y_map) {
			    /* check for walls in the room */
			    char ebuf[60];
			    register int x, y, nrock = 0;

			    for(y=current_region.y1; y<=current_region.y2; y++)
				for(x=current_region.x1;
				    x<=current_region.x2; x++)
				    if(IS_ROCK(tmpmap[y][x]) ||
				       IS_DOOR(tmpmap[y][x])) nrock++;
			    if(nrock) {
				Sprintf(ebuf,
					"Rock in room (%02d,%02d,%02d,%02d)?!",
					current_region.x1, current_region.y1,
					current_region.x2, current_region.y2);
				yywarning(ebuf);
			    }
			    if (
		!IS_ROCK(tmpmap[current_region.y1-1][current_region.x1-1]) ||
		!IS_ROCK(tmpmap[current_region.y2+1][current_region.x1-1]) ||
		!IS_ROCK(tmpmap[current_region.y1-1][current_region.x2+1]) ||
		!IS_ROCK(tmpmap[current_region.y2+1][current_region.x2+1])) {
				Sprintf(ebuf,
				"NonRock edge in room (%02d,%02d,%02d,%02d)?!",
					current_region.x1, current_region.y1,
					current_region.x2, current_region.y2);
				yywarning(ebuf);
			    }
			} else if(tmpreg[nreg]->rirreg &&
		!IS_ROOM(tmpmap[current_region.y1][current_region.x1])) {
			    char ebuf[60];
			    Sprintf(ebuf,
				    "Rock in irregular room (%02d,%02d)?!",
				    current_region.x1, current_region.y1);
			    yyerror(ebuf);
			}
			nreg++;
			if (nreg >= MAX_OF_TYPE) {
				yyerror("Too many regions in mazepart!");
				nreg--;
			}
		  }
break;
case 183:
{
			tmpaltar[naltar] = New(altar);
			tmpaltar[naltar]->x = current_coord.x;
			tmpaltar[naltar]->y = current_coord.y;
			tmpaltar[naltar]->align = yyvsp[-2].i;
			tmpaltar[naltar]->shrine = yyvsp[0].i;
			if (!in_room)
			    check_coord(current_coord.x, current_coord.y,
					"Altar");
			naltar++;
			if (naltar >= MAX_OF_TYPE) {
				yyerror("Too many altars in room or mazepart!");
				naltar--;
			}
		  }
break;
case 184:
{
			tmpgold[ngold] = New(gold);
			tmpgold[ngold]->x = current_coord.x;
			tmpgold[ngold]->y = current_coord.y;
			tmpgold[ngold]->amount = yyvsp[-2].i;
			if (!in_room)
			    check_coord(current_coord.x, current_coord.y,
					"Gold");
			ngold++;
			if (ngold >= MAX_OF_TYPE) {
				yyerror("Too many golds in room or mazepart!");
				ngold--;
			}
		  }
break;
case 185:
{
			tmpengraving[nengraving] = New(engraving);
			tmpengraving[nengraving]->x = current_coord.x;
			tmpengraving[nengraving]->y = current_coord.y;
			tmpengraving[nengraving]->engr.str = yyvsp[0].map;
			tmpengraving[nengraving]->etype = yyvsp[-2].i;
			if (!in_room)
			    check_coord(current_coord.x, current_coord.y,
					"Engraving");
			nengraving++;
			if (nengraving >= MAX_OF_TYPE) {
			    yyerror("Too many engravings in room or mazepart!");
			    nengraving--;
			}
		  }
break;
case 187:
{
			yyval.i = - MAX_REGISTERS - 1;
		  }
break;
case 190:
{
			yyval.i = - MAX_REGISTERS - 1;
		  }
break;
case 193:
{
			yyval.map = (char *) 0;
		  }
break;
case 195:
{
			yyval.map = (char *) 0;
		  }
break;
case 196:
{
			int token = get_trap_type(yyvsp[0].map);
			if (token == ERR)
				yyerror("Unknown trap type!");
			yyval.i = token;
			Free(yyvsp[0].map);
		  }
break;
case 198:
{
			int token = get_room_type(yyvsp[0].map);
			if (token == ERR) {
				yywarning("Unknown room type!  Making ordinary room...");
				yyval.i = OROOM;
			} else
				yyval.i = token;
			Free(yyvsp[0].map);
		  }
break;
case 200:
{
			yyval.i = 0;
		  }
break;
case 201:
{
			yyval.i = yyvsp[0].i;
		  }
break;
case 202:
{
			yyval.i = yyvsp[-2].i + (yyvsp[0].i << 1);
		  }
break;
case 205:
{
			current_coord.x = current_coord.y = -MAX_REGISTERS-1;
		  }
break;
case 212:
{
			yyval.i = - MAX_REGISTERS - 1;
		  }
break;
case 215:
{
			if ( yyvsp[-1].i >= MAX_REGISTERS )
				yyerror("Register Index overflow!");
			else
				current_coord.x = current_coord.y = - yyvsp[-1].i - 1;
		  }
break;
case 216:
{
			if ( yyvsp[-1].i >= MAX_REGISTERS )
				yyerror("Register Index overflow!");
			else
				yyval.i = - yyvsp[-1].i - 1;
		  }
break;
case 217:
{
			if ( yyvsp[-1].i >= MAX_REGISTERS )
				yyerror("Register Index overflow!");
			else
				yyval.i = - yyvsp[-1].i - 1;
		  }
break;
case 218:
{
			if ( yyvsp[-1].i >= 3 )
				yyerror("Register Index overflow!");
			else
				yyval.i = - yyvsp[-1].i - 1;
		  }
break;
case 220:
{
			current_coord.x = (char)-1;
			current_coord.y = (char)-1;
		  }
break;
case 221:
{
			if (check_monster_char((char) yyvsp[0].i))
				yyval.i = yyvsp[0].i ;
			else {
				yyerror("Unknown monster class!");
				yyval.i = ERR;
			}
		  }
break;
case 222:
{
			char c = yyvsp[0].i;
			if (check_object_char(c))
				yyval.i = c;
			else {
				yyerror("Unknown char class!");
				yyval.i = ERR;
			}
		  }
break;
case 226:
{
			yyval.i = 100;	/* default is 100% */
		  }
break;
case 227:
{
			if (yyvsp[0].i <= 0 || yyvsp[0].i > 100)
			    yyerror("Expected percentile chance.");
			yyval.i = yyvsp[0].i;
		  }
break;
case 230:
{
			if (!in_room && !init_lev.init_present &&
			    (yyvsp[-3].i < 0 || yyvsp[-3].i > (int)max_x_map ||
			     yyvsp[-1].i < 0 || yyvsp[-1].i > (int)max_y_map))
			    yyerror("Coordinates out of map range!");
			current_coord.x = yyvsp[-3].i;
			current_coord.y = yyvsp[-1].i;
		  }
break;
case 231:
{
/* This series of if statements is a hack for MSC 5.1.  It seems that its
   tiny little brain cannot compile if these are all one big if statement. */
			if (yyvsp[-7].i < 0 || yyvsp[-7].i > (int)max_x_map)
				yyerror("Region out of map range!");
			else if (yyvsp[-5].i < 0 || yyvsp[-5].i > (int)max_y_map)
				yyerror("Region out of map range!");
			else if (yyvsp[-3].i < 0 || yyvsp[-3].i > (int)max_x_map)
				yyerror("Region out of map range!");
			else if (yyvsp[-1].i < 0 || yyvsp[-1].i > (int)max_y_map)
				yyerror("Region out of map range!");
			current_region.x1 = yyvsp[-7].i;
			current_region.y1 = yyvsp[-5].i;
			current_region.x2 = yyvsp[-3].i;
			current_region.y2 = yyvsp[-1].i;
		  }
break;
    }
    yyssp -= yym;
    yystate = *yyssp;
    yyvsp -= yym;
    yym = yylhs[yyn];
    if (yystate == 0 && yym == 0)
    {
#if YYDEBUG
        if (yydebug)
            printf("%sdebug: after reduction, shifting from state 0 to\
 state %d\n", YYPREFIX, YYFINAL);
#endif
        yystate = YYFINAL;
        *++yyssp = YYFINAL;
        *++yyvsp = yyval;
        if (yychar < 0)
        {
            if ((yychar = yylex()) < 0) yychar = 0;
#if YYDEBUG
            if (yydebug)
            {
                yys = 0;
                if (yychar <= YYMAXTOKEN) yys = yyname[yychar];
                if (!yys) yys = "illegal-symbol";
                printf("%sdebug: state %d, reading %d (%s)\n",
                        YYPREFIX, YYFINAL, yychar, yys);
            }
#endif
        }
        if (yychar == 0) goto yyaccept;
        goto yyloop;
    }
    if ((yyn = yygindex[yym]) && (yyn += yystate) >= 0 &&
            yyn <= YYTABLESIZE && yycheck[yyn] == yystate)
        yystate = yytable[yyn];
    else
        yystate = yydgoto[yym];
#if YYDEBUG
    if (yydebug)
        printf("%sdebug: after reduction, shifting from state %d \
to state %d\n", YYPREFIX, *yyssp, yystate);
#endif
    if (yyssp >= yysslim && yygrowstack())
    {
        goto yyoverflow;
    }
    *++yyssp = yystate;
    *++yyvsp = yyval;
    goto yyloop;
yyoverflow:
    yyerror("yacc stack overflow");
yyabort:
    return (1);
yyaccept:
    return (0);
}
