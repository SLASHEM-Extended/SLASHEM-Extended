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
/*	SCCS Id: @(#)dgn_comp.c	3.4	1996/06/22	*/
/*	Copyright (c) 1989 by Jean-Christophe Collet */
/*	Copyright (c) 1990 by M. Stephenson				  */
/* NetHack may be freely redistributed.  See license for details. */

/*
 * This file contains the Dungeon Compiler code
 */

/* In case we're using bison in AIX.  This definition must be
 * placed before any other C-language construct in the file
 * excluding comments and preprocessor directives (thanks IBM
 * for this wonderful feature...).
 *
 * Note: some cpps barf on this 'undefined control' (#pragma).
 * Addition of the leading space seems to prevent barfage for now,
 * and AIX will still see the directive in its non-standard locale.
 */

#ifdef _AIX
 #pragma alloca		/* keep leading space! */
#endif

#include "config.h"
#include "date.h"
#include "dgn_file.h"

void FDECL(yyerror, (const char *));
void FDECL(yywarning, (const char *));
int NDECL(yylex);
int NDECL(yyparse);
int FDECL(getchain, (char *));
int NDECL(check_dungeon);
int NDECL(check_branch);
int NDECL(check_level);
void NDECL(init_dungeon);
void NDECL(init_branch);
void NDECL(init_level);
void NDECL(output_dgn);

#define Free(ptr)		free((genericptr_t)ptr)

#ifdef AMIGA
# undef	printf
#ifndef	LATTICE
# define    memset(addr,val,len)    setmem(addr,len,val)
#endif
#endif

#define ERR		(-1)

static struct couple couple;
static struct tmpdungeon tmpdungeon[MAXDUNGEON];
static struct tmplevel tmplevel[LEV_LIMIT];
static struct tmpbranch tmpbranch[BRANCH_LIMIT];

static int in_dungeon = 0, n_dgns = -1, n_levs = -1, n_brs = -1;

extern int fatal_error;
extern const char *fname;
extern FILE *yyin, *yyout;	/* from dgn_lex.c */

typedef union
{
	int	i;
	char*	str;
} YYSTYPE;
#define YYERRCODE 256
#define INTEGER 257
#define A_DUNGEON 258
#define BRANCH 259
#define CHBRANCH 260
#define LEVEL 261
#define RNDLEVEL 262
#define CHLEVEL 263
#define RNDCHLEVEL 264
#define UP_OR_DOWN 265
#define PROTOFILE 266
#define DESCRIPTION 267
#define DESCRIPTOR 268
#define LEVELDESC 269
#define ALIGNMENT 270
#define LEVALIGN 271
#define ENTRY 272
#define STAIR 273
#define NO_UP 274
#define NO_DOWN 275
#define PORTAL 276
#define STRING 277
const short yylhs[] = {                                        -1,
    0,    0,    5,    5,    6,    6,    6,    6,    7,    1,
    1,    8,    8,    8,   12,   13,   15,   15,   14,   10,
   10,   10,   10,   10,   16,   16,   17,   17,   18,   18,
   19,   19,   20,   20,    9,    9,   22,   23,    3,    3,
    3,    3,    3,    2,    2,    4,   21,   11,
};
const short yylen[] = {                                         2,
    0,    1,    1,    2,    1,    1,    1,    1,    6,    0,
    1,    1,    1,    1,    3,    1,    3,    3,    3,    1,
    1,    1,    1,    1,    6,    7,    7,    8,    3,    3,
    7,    8,    8,    9,    1,    1,    8,    9,    0,    1,
    1,    1,    1,    0,    1,    1,    5,    5,
};
const short yydefred[] = {                                      0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    3,    5,    6,    7,    8,
   12,   13,   14,   16,   20,   21,   22,   23,   24,   35,
   36,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    4,    0,    0,    0,    0,    0,
    0,    0,   19,   17,   29,   18,   30,   15,   46,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,   11,    9,    0,   40,
   41,   42,   43,    0,    0,    0,    0,    0,    0,    0,
    0,   45,    0,    0,   27,    0,    0,    0,    0,    0,
   37,    0,   28,   33,    0,   48,   47,   38,   34,
};
const short yydgoto[] = {                                      14,
   78,   93,   84,   60,   15,   16,   17,   18,   19,   20,
   68,   21,   22,   23,   24,   25,   26,   27,   28,   29,
   70,   30,   31,
};
const short yysindex[] = {                                   -236,
  -44,  -43,  -42,  -40,  -37,  -29,  -21,  -20,  -19,  -18,
  -17,  -16,  -15,    0, -236,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0, -233, -232, -231, -229, -228, -227, -226, -225, -215,
 -214, -213, -212, -200,    0, -219,   -5, -217, -219, -219,
 -219, -219,    0,    0,    0,    0,    0,    0,    0,   21,
   23,   22,    2,    3, -209, -208, -193, -187, -186, -270,
   21,   23,   23,   29,   30,   31,    0,    0,   34,    0,
    0,    0,    0, -191, -270, -181, -178,   21,   21, -177,
 -176,    0, -187, -191,    0, -175, -174, -173,   44,   45,
    0, -187,    0,    0, -170,    0,    0,    0,    0,
};
const short yyrindex[] = {                                     88,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,   89,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,   32,    0,    1,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,   17,    1,   47,    0,    0,    0,    0,
    0,    0,   32,   17,    0,   62,   77,    0,    0,    0,
    0,   32,    0,    0,   92,    0,    0,    0,    0,
};
const short yygindex[] = {                                      0,
  -93,   -4,    6,  -39,    0,   78,    0,    0,    0,    0,
  -69,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  -65,    0,    0,
};
#define YYTABLESIZE 364
const short yytable[] = {                                     101,
   39,   85,   80,   81,   82,   83,   86,   87,  108,   63,
   64,   65,   66,   32,   33,   34,   44,   35,   97,   98,
   36,    1,    2,    3,    4,    5,    6,    7,   37,    8,
    9,   10,   10,   11,   12,   13,   38,   39,   40,   41,
   42,   43,   44,   46,   47,   48,   25,   49,   50,   51,
   52,   53,   54,   55,   56,   57,   58,   59,   61,   62,
   67,   26,   69,   76,   71,   72,   73,   74,   75,   77,
   79,   88,   89,   92,   90,   95,   31,   91,   96,   99,
  100,  103,  104,  105,  106,  107,  109,    1,    2,  102,
   94,   32,   45,    0,    0,    0,    0,    0,    0,    0,
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
    0,    0,    0,    0,    0,    0,    0,   39,   39,   39,
   39,   39,   39,   39,   39,   39,   39,   39,    0,   39,
   39,   39,   39,   44,   44,   44,   44,   44,   44,   44,
   44,    0,   44,   44,    0,   44,   44,   44,   44,   10,
   10,   10,   10,   10,   10,   10,    0,   10,   10,    0,
   10,   10,   10,   10,   25,   25,   25,   25,   25,   25,
   25,    0,   25,   25,    0,   25,   25,   25,   25,   26,
   26,   26,   26,   26,   26,   26,    0,   26,   26,    0,
   26,   26,   26,   26,   31,   31,   31,   31,   31,   31,
   31,    0,   31,   31,    0,   31,   31,   31,   31,   32,
   32,   32,   32,   32,   32,   32,    0,   32,   32,    0,
   32,   32,   32,   32,
};
const short yycheck[] = {                                      93,
    0,   71,  273,  274,  275,  276,   72,   73,  102,   49,
   50,   51,   52,   58,   58,   58,    0,   58,   88,   89,
   58,  258,  259,  260,  261,  262,  263,  264,   58,  266,
  267,    0,  269,  270,  271,  272,   58,   58,   58,   58,
   58,   58,   58,  277,  277,  277,    0,  277,  277,  277,
  277,  277,  268,  268,  268,  268,  257,  277,   64,  277,
   40,    0,   40,  257,   43,   64,   64,  277,  277,  257,
  257,   43,   43,  265,   44,  257,    0,   44,  257,  257,
  257,  257,  257,  257,   41,   41,  257,    0,    0,   94,
   85,    0,   15,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
   -1,   -1,   -1,   -1,   -1,   -1,   -1,  257,  258,  259,
  260,  261,  262,  263,  264,  265,  266,  267,   -1,  269,
  270,  271,  272,  257,  258,  259,  260,  261,  262,  263,
  264,   -1,  266,  267,   -1,  269,  270,  271,  272,  258,
  259,  260,  261,  262,  263,  264,   -1,  266,  267,   -1,
  269,  270,  271,  272,  258,  259,  260,  261,  262,  263,
  264,   -1,  266,  267,   -1,  269,  270,  271,  272,  258,
  259,  260,  261,  262,  263,  264,   -1,  266,  267,   -1,
  269,  270,  271,  272,  258,  259,  260,  261,  262,  263,
  264,   -1,  266,  267,   -1,  269,  270,  271,  272,  258,
  259,  260,  261,  262,  263,  264,   -1,  266,  267,   -1,
  269,  270,  271,  272,
};
#define YYFINAL 14
#ifndef YYDEBUG
#define YYDEBUG 0
#endif
#define YYMAXTOKEN 277
#if YYDEBUG
const char * const yyname[] = {
"end-of-file",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"'('","')'",0,"'+'","','",0,0,0,0,0,0,0,0,0,0,0,0,0,"':'",0,0,0,0,0,
"'@'",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"INTEGER",
"A_DUNGEON","BRANCH","CHBRANCH","LEVEL","RNDLEVEL","CHLEVEL","RNDCHLEVEL",
"UP_OR_DOWN","PROTOFILE","DESCRIPTION","DESCRIPTOR","LEVELDESC","ALIGNMENT",
"LEVALIGN","ENTRY","STAIR","NO_UP","NO_DOWN","PORTAL","STRING",
};
const char * const yyrule[] = {
"$accept : file",
"file :",
"file : dungeons",
"dungeons : dungeon",
"dungeons : dungeons dungeon",
"dungeon : dungeonline",
"dungeon : dungeondesc",
"dungeon : branches",
"dungeon : levels",
"dungeonline : A_DUNGEON ':' STRING bones_tag rcouple optional_int",
"optional_int :",
"optional_int : INTEGER",
"dungeondesc : entry",
"dungeondesc : descriptions",
"dungeondesc : prototype",
"entry : ENTRY ':' INTEGER",
"descriptions : desc",
"desc : DESCRIPTION ':' DESCRIPTOR",
"desc : ALIGNMENT ':' DESCRIPTOR",
"prototype : PROTOFILE ':' STRING",
"levels : level1",
"levels : level2",
"levels : levdesc",
"levels : chlevel1",
"levels : chlevel2",
"level1 : LEVEL ':' STRING bones_tag '@' acouple",
"level1 : RNDLEVEL ':' STRING bones_tag '@' acouple INTEGER",
"level2 : LEVEL ':' STRING bones_tag '@' acouple INTEGER",
"level2 : RNDLEVEL ':' STRING bones_tag '@' acouple INTEGER INTEGER",
"levdesc : LEVELDESC ':' DESCRIPTOR",
"levdesc : LEVALIGN ':' DESCRIPTOR",
"chlevel1 : CHLEVEL ':' STRING bones_tag STRING '+' rcouple",
"chlevel1 : RNDCHLEVEL ':' STRING bones_tag STRING '+' rcouple INTEGER",
"chlevel2 : CHLEVEL ':' STRING bones_tag STRING '+' rcouple INTEGER",
"chlevel2 : RNDCHLEVEL ':' STRING bones_tag STRING '+' rcouple INTEGER INTEGER",
"branches : branch",
"branches : chbranch",
"branch : BRANCH ':' STRING '@' acouple branch_type direction optional_int",
"chbranch : CHBRANCH ':' STRING STRING '+' rcouple branch_type direction optional_int",
"branch_type :",
"branch_type : STAIR",
"branch_type : NO_UP",
"branch_type : NO_DOWN",
"branch_type : PORTAL",
"direction :",
"direction : UP_OR_DOWN",
"bones_tag : STRING",
"acouple : '(' INTEGER ',' INTEGER ')'",
"rcouple : '(' INTEGER ',' INTEGER ')'",
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

void
init_dungeon()
{
	if(++n_dgns > MAXDUNGEON) {
	    (void) fprintf(stderr, "FATAL - Too many dungeons (limit: %d).\n",
		    MAXDUNGEON);
	    (void) fprintf(stderr, "To increase the limit edit MAXDUNGEON in global.h\n");
	    exit(EXIT_FAILURE);
	}

	in_dungeon = 1;
	tmpdungeon[n_dgns].lev.base = 0;
	tmpdungeon[n_dgns].lev.rand = 0;
	tmpdungeon[n_dgns].chance = 100;
	Strcpy(tmpdungeon[n_dgns].name, "");
	Strcpy(tmpdungeon[n_dgns].protoname, "");
	tmpdungeon[n_dgns].flags = 0;
	tmpdungeon[n_dgns].levels = 0;
	tmpdungeon[n_dgns].branches = 0;
}

void
init_level()
{
	if(++n_levs > LEV_LIMIT) {

		yyerror("FATAL - Too many special levels defined.");
		exit(EXIT_FAILURE);
	}
	tmplevel[n_levs].lev.base = 0;
	tmplevel[n_levs].lev.rand = 0;
	tmplevel[n_levs].chance = 100;
	tmplevel[n_levs].rndlevs = 0;
	tmplevel[n_levs].flags = 0;
	Strcpy(tmplevel[n_levs].name, "");
	tmplevel[n_levs].chain = -1;
}

void
init_branch()
{
	if(++n_brs > BRANCH_LIMIT) {

		yyerror("FATAL - Too many special levels defined.");
		exit(EXIT_FAILURE);
	}
	tmpbranch[n_brs].lev.base = 0;
	tmpbranch[n_brs].lev.rand = 0;
	Strcpy(tmpbranch[n_brs].name, "");
	tmpbranch[n_brs].chain = -1;
	tmpbranch[n_brs].entry_lev = 0;
}

int
getchain(s)
	char	*s;
{
	int i;

	if(strlen(s)) {

	    for(i = n_levs - tmpdungeon[n_dgns].levels + 1; i <= n_levs; i++)
		if(!strcmp(tmplevel[i].name, s)) return i;

	    yyerror("Can't locate the specified chain level.");
	    return(-2);
	}
	return(-1);
}

/*
 *	Consistancy checking routines:
 *
 *	- A dungeon must have a unique name.
 *	- A dungeon must have a originating "branch" command
 *	  (except, of course, for the first dungeon).
 *	- A dungeon must have a proper depth (at least (1, 0)).
 */

int
check_dungeon()
{
	int i;

	for(i = 0; i < n_dgns; i++)
	    if(!strcmp(tmpdungeon[i].name, tmpdungeon[n_dgns].name)) {
		yyerror("Duplicate dungeon name.");
		return(0);
	    }

	if(n_dgns)
	  for(i = 0; i < n_brs - tmpdungeon[n_dgns].branches; i++) {
	    if(!strcmp(tmpbranch[i].name, tmpdungeon[n_dgns].name)) break;

	    if(i >= n_brs - tmpdungeon[n_dgns].branches) {
		yyerror("Dungeon cannot be reached.");
		return(0);
	    }
	  }

	if(tmpdungeon[n_dgns].lev.base <= 0 ||
	   tmpdungeon[n_dgns].lev.rand < 0) {
		yyerror("Invalid dungeon depth specified.");
		return(0);
	}
	return(1);	/* OK */
}

/*
 *	- A level must have a unique level name.
 *	- If chained, the level used as reference for the chain
 *	  must be in this dungeon, must be previously defined, and
 *	  the level chained from must be "non-probabilistic" (ie.
 *	  have a 100% chance of existing).
 */

int
check_level()
{
	int i;

	if(!in_dungeon) {
		yyerror("Level defined outside of dungeon.");
		return(0);
	}

	for(i = 0; i < n_levs; i++)
	    if(!strcmp(tmplevel[i].name, tmplevel[n_levs].name)) {
		yyerror("Duplicate level name.");
		return(0);
	    }

	if(tmplevel[i].chain == -2) {
		yyerror("Invalid level chain reference.");
		return(0);
	} else if(tmplevel[i].chain != -1) {	/* there is a chain */
		/* KMH -- tmplevel[tmpbranch[i].chain].chance was in error */
	    if(tmplevel[tmplevel[i].chain].chance != 100) {
		yyerror("Level cannot chain from a probabilistic level.");
		return(0);
	    } else if(tmplevel[i].chain == n_levs) {
		yyerror("A level cannot chain to itself!");
		return(0);
	    }
	}
	return(1);	/* OK */
}

/*
 *	- A branch may not branch backwards - to avoid branch loops.
 *	- A branch name need not be unique.
 *	  (ie. You can have many entry points to each dungeon).
 *	- If chained, the level used as reference for the chain
 *	  must be in this dungeon, must be previously defined, and
 *	  the level chained from must be "non-probabilistic" (ie.
 *	  have a 100% chance of existing).
 */

int
check_branch()
{
	int i;

	if(!in_dungeon) {
		yyerror("Branch defined outside of dungeon.");
		return(0);
	}
#if 0
	for(i = 0; i < n_dgns; i++)
	    if(!strcmp(tmpdungeon[i].name, tmpbranch[n_brs].name)) {

		yyerror("Reverse branching not allowed.");
		return(0);
	    }
#endif
	if(tmpbranch[n_dgns].chain == -2) {

		yyerror("Invalid branch chain reference.");
		return(0);
	} else if(tmpbranch[n_dgns].chain != -1) {	/* it is chained */

	    if(tmplevel[tmpbranch[n_dgns].chain].chance != 100) {
		yyerror("Branch cannot chain from a probabilistic level.");
		return(0);
	    }
	}
	return(1);	/* OK */
}

/*
 *	Output the dungon definition into a file.
 *
 *	The file will have the following format:
 *
 *	[ nethack version ID ]
 *	[ number of dungeons ]
 *	[ first dungeon struct ]
 *	[ levels for the first dungeon ]
 *	  ...
 *	[ branches for the first dungeon ]
 *	  ...
 *	[ second dungeon struct ]
 *	  ...
 */

void
output_dgn()
{
	int	nd, cl = 0, nl = 0,
		    cb = 0, nb = 0;
        static struct version_info version_data = {
                        VERSION_NUMBER, VERSION_FEATURES,
                        VERSION_SANITY1, VERSION_SANITY2
        };

	if(++n_dgns <= 0) {
	    yyerror("FATAL - no dungeons were defined.");
	    exit(EXIT_FAILURE);
	}

        if (fwrite((char *)&version_data, sizeof version_data, 1, yyout) != 1) {
	    yyerror("FATAL - output failure.");
	    exit(EXIT_FAILURE);
	}

	(void) fwrite((char *)&n_dgns, sizeof(int), 1, yyout);
	for (nd = 0; nd < n_dgns; nd++) {
	    (void) fwrite((char *)&tmpdungeon[nd], sizeof(struct tmpdungeon),
							1, yyout);

	    nl += tmpdungeon[nd].levels;
	    for(; cl < nl; cl++)
		(void) fwrite((char *)&tmplevel[cl], sizeof(struct tmplevel),
							1, yyout);

	    nb += tmpdungeon[nd].branches;
	    for(; cb < nb; cb++)
		(void) fwrite((char *)&tmpbranch[cb], sizeof(struct tmpbranch),
							1, yyout);
	}
	/* apparently necessary for Think C 5.x, otherwise harmless */
	(void) fflush(yyout);
}

/*dgn_comp.y*/
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
case 2:
{
			output_dgn();
		  }
break;
case 9:
{
			init_dungeon();
			if (strlen(yyvsp[-3].str)+1 > sizeof(tmpdungeon[n_dgns].name))
			    yyerror("Dungeon name is too long");
			Strcpy(tmpdungeon[n_dgns].name, yyvsp[-3].str);
			tmpdungeon[n_dgns].boneschar = (char)yyvsp[-2].i;
			tmpdungeon[n_dgns].lev.base = couple.base;
			tmpdungeon[n_dgns].lev.rand = couple.rand;
			tmpdungeon[n_dgns].chance = yyvsp[0].i;
			Free(yyvsp[-3].str);
		  }
break;
case 10:
{
			yyval.i = 0;
		  }
break;
case 11:
{
			yyval.i = yyvsp[0].i;
		  }
break;
case 15:
{
			/* tmpdungeon[n_dgns].entry_lev = $3; */
                        yyerror("ENTRY is obsolete!");
		  }
break;
case 17:
{
			if(yyvsp[0].i <= TOWN || yyvsp[0].i >= D_ALIGN_CHAOTIC)
			    yyerror("Illegal description - ignoring!");
			else
			    tmpdungeon[n_dgns].flags |= yyvsp[0].i ;
		  }
break;
case 18:
{
			if(yyvsp[0].i && yyvsp[0].i < D_ALIGN_CHAOTIC)
			    yyerror("Illegal alignment - ignoring!");
			else
			    tmpdungeon[n_dgns].flags |= yyvsp[0].i ;
		  }
break;
case 19:
{
			if (strlen(yyvsp[0].str)+1 > sizeof(tmpdungeon[n_dgns].protoname))
			    yyerror("Proto name is too long");
			Strcpy(tmpdungeon[n_dgns].protoname, yyvsp[0].str);
			Free(yyvsp[0].str);
		  }
break;
case 25:
{
			init_level();
			if (strlen(yyvsp[-3].str)+1 > sizeof(tmplevel[n_levs].name))
			    yyerror("Level name is too long");
			Strcpy(tmplevel[n_levs].name, yyvsp[-3].str);
			tmplevel[n_levs].boneschar = (char)yyvsp[-2].i;
			tmplevel[n_levs].lev.base = couple.base;
			tmplevel[n_levs].lev.rand = couple.rand;
			tmpdungeon[n_dgns].levels++;
			Free(yyvsp[-3].str);
		  }
break;
case 26:
{
			init_level();
			if (strlen(yyvsp[-4].str)+1 > sizeof(tmplevel[n_levs].name))
			    yyerror("Level name is too long");
			Strcpy(tmplevel[n_levs].name, yyvsp[-4].str);
			tmplevel[n_levs].boneschar = (char)yyvsp[-3].i;
			tmplevel[n_levs].lev.base = couple.base;
			tmplevel[n_levs].lev.rand = couple.rand;
			tmplevel[n_levs].rndlevs = yyvsp[0].i;
			tmpdungeon[n_dgns].levels++;
			Free(yyvsp[-4].str);
		  }
break;
case 27:
{
			init_level();
			if (strlen(yyvsp[-4].str)+1 > sizeof(tmplevel[n_levs].name))
			    yyerror("Level name is too long");
			Strcpy(tmplevel[n_levs].name, yyvsp[-4].str);
			tmplevel[n_levs].boneschar = (char)yyvsp[-3].i;
			tmplevel[n_levs].lev.base = couple.base;
			tmplevel[n_levs].lev.rand = couple.rand;
			tmplevel[n_levs].chance = yyvsp[0].i;
			tmpdungeon[n_dgns].levels++;
			Free(yyvsp[-4].str);
		  }
break;
case 28:
{
			init_level();
			if (strlen(yyvsp[-5].str)+1 > sizeof(tmplevel[n_levs].name))
			    yyerror("Level name is too long");
			Strcpy(tmplevel[n_levs].name, yyvsp[-5].str);
			tmplevel[n_levs].boneschar = (char)yyvsp[-4].i;
			tmplevel[n_levs].lev.base = couple.base;
			tmplevel[n_levs].lev.rand = couple.rand;
			tmplevel[n_levs].chance = yyvsp[-1].i;
			tmplevel[n_levs].rndlevs = yyvsp[0].i;
			tmpdungeon[n_dgns].levels++;
			Free(yyvsp[-5].str);
		  }
break;
case 29:
{
			if(yyvsp[0].i >= D_ALIGN_CHAOTIC)
			    yyerror("Illegal description - ignoring!");
			else
			    tmplevel[n_levs].flags |= yyvsp[0].i ;
		  }
break;
case 30:
{
			if(yyvsp[0].i && yyvsp[0].i < D_ALIGN_CHAOTIC)
			    yyerror("Illegal alignment - ignoring!");
			else
			    tmplevel[n_levs].flags |= yyvsp[0].i ;
		  }
break;
case 31:
{
			init_level();
			if (strlen(yyvsp[-4].str)+1 > sizeof(tmplevel[n_levs].name))
			    yyerror("Level name is too long");
			Strcpy(tmplevel[n_levs].name, yyvsp[-4].str);
			tmplevel[n_levs].boneschar = (char)yyvsp[-3].i;
			tmplevel[n_levs].chain = getchain(yyvsp[-2].str);
			tmplevel[n_levs].lev.base = couple.base;
			tmplevel[n_levs].lev.rand = couple.rand;
			if(!check_level()) n_levs--;
			else tmpdungeon[n_dgns].levels++;
			Free(yyvsp[-4].str);
			Free(yyvsp[-2].str);
		  }
break;
case 32:
{
			init_level();
			if (strlen(yyvsp[-5].str)+1 > sizeof(tmplevel[n_levs].name))
			    yyerror("Level name is too long");
			Strcpy(tmplevel[n_levs].name, yyvsp[-5].str);
			tmplevel[n_levs].boneschar = (char)yyvsp[-4].i;
			tmplevel[n_levs].chain = getchain(yyvsp[-3].str);
			tmplevel[n_levs].lev.base = couple.base;
			tmplevel[n_levs].lev.rand = couple.rand;
			tmplevel[n_levs].rndlevs = yyvsp[0].i;
			if(!check_level()) n_levs--;
			else tmpdungeon[n_dgns].levels++;
			Free(yyvsp[-5].str);
			Free(yyvsp[-3].str);
		  }
break;
case 33:
{
			init_level();
			if (strlen(yyvsp[-5].str)+1 > sizeof(tmplevel[n_levs].name))
			    yyerror("Level name is too long");
			Strcpy(tmplevel[n_levs].name, yyvsp[-5].str);
			tmplevel[n_levs].boneschar = (char)yyvsp[-4].i;
			tmplevel[n_levs].chain = getchain(yyvsp[-3].str);
			tmplevel[n_levs].lev.base = couple.base;
			tmplevel[n_levs].lev.rand = couple.rand;
			tmplevel[n_levs].chance = yyvsp[0].i;
			if(!check_level()) n_levs--;
			else tmpdungeon[n_dgns].levels++;
			Free(yyvsp[-5].str);
			Free(yyvsp[-3].str);
		  }
break;
case 34:
{
			init_level();
			if (strlen(yyvsp[-6].str)+1 > sizeof(tmplevel[n_levs].name))
			    yyerror("Level name is too long");
			Strcpy(tmplevel[n_levs].name, yyvsp[-6].str);
			tmplevel[n_levs].boneschar = (char)yyvsp[-5].i;
			tmplevel[n_levs].chain = getchain(yyvsp[-4].str);
			tmplevel[n_levs].lev.base = couple.base;
			tmplevel[n_levs].lev.rand = couple.rand;
			tmplevel[n_levs].chance = yyvsp[-1].i;
			tmplevel[n_levs].rndlevs = yyvsp[0].i;
			if(!check_level()) n_levs--;
			else tmpdungeon[n_dgns].levels++;
			Free(yyvsp[-6].str);
			Free(yyvsp[-4].str);
		  }
break;
case 37:
{
			init_branch();
			if (strlen(yyvsp[-5].str)+1 > sizeof(tmpbranch[n_brs].name))
			    yyerror("Dungeon name is too long");
			Strcpy(tmpbranch[n_brs].name, yyvsp[-5].str);
			tmpbranch[n_brs].lev.base = couple.base;
			tmpbranch[n_brs].lev.rand = couple.rand;
			tmpbranch[n_brs].type = yyvsp[-2].i;
			tmpbranch[n_brs].up = yyvsp[-1].i;
                        tmpbranch[n_brs].entry_lev = yyvsp[0].i;
			if(!check_branch()) n_brs--;
			else tmpdungeon[n_dgns].branches++;
			Free(yyvsp[-5].str);
		  }
break;
case 38:
{
			init_branch();
			if (strlen(yyvsp[-6].str)+1 > sizeof(tmpbranch[n_brs].name))
			    yyerror("Dungeon name is too long");
			Strcpy(tmpbranch[n_brs].name, yyvsp[-6].str);
			tmpbranch[n_brs].chain = getchain(yyvsp[-5].str);
			tmpbranch[n_brs].lev.base = couple.base;
			tmpbranch[n_brs].lev.rand = couple.rand;
			tmpbranch[n_brs].type = yyvsp[-2].i;
			tmpbranch[n_brs].up = yyvsp[-1].i;
			tmpbranch[n_brs].entry_lev = yyvsp[0].i;
			if(!check_branch()) n_brs--;
			else tmpdungeon[n_dgns].branches++;
			Free(yyvsp[-6].str);
			Free(yyvsp[-5].str);
		  }
break;
case 39:
{
			yyval.i = TBR_STAIR;	/* two way stair */
		  }
break;
case 40:
{
			yyval.i = TBR_STAIR;	/* two way stair */
		  }
break;
case 41:
{
			yyval.i = TBR_NO_UP;	/* no up staircase */
		  }
break;
case 42:
{
			yyval.i = TBR_NO_DOWN;	/* no down staircase */
		  }
break;
case 43:
{
			yyval.i = TBR_PORTAL;	/* portal connection */
		  }
break;
case 44:
{
			yyval.i = 0;	/* defaults to down */
		  }
break;
case 45:
{
			yyval.i = yyvsp[0].i;
		  }
break;
case 46:
{
			char *p = yyvsp[0].str;
			if (strlen(p) != 1) {
			    if (strcmp(p, "none") != 0)
		   yyerror("Bones marker must be a single char, or \"none\"!");
			    *p = '\0';
			}
			yyval.i = *p;
			Free(p);
		  }
break;
case 47:
{
			if (yyvsp[-3].i < -MAXLEVEL || yyvsp[-3].i > MAXLEVEL) {
			    yyerror("Abs base out of dlevel range - zeroing!");
			    couple.base = couple.rand = 0;
			} else if (yyvsp[-1].i < -1 ||
				((yyvsp[-3].i < 0) ? (MAXLEVEL + yyvsp[-3].i + yyvsp[-1].i + 1) > MAXLEVEL :
					(yyvsp[-3].i + yyvsp[-1].i) > MAXLEVEL)) {
			    yyerror("Abs range out of dlevel range - zeroing!");
			    couple.base = couple.rand = 0;
			} else {
			    couple.base = yyvsp[-3].i;
			    couple.rand = yyvsp[-1].i;
			}
		  }
break;
case 48:
{
			if (yyvsp[-3].i < -MAXLEVEL || yyvsp[-3].i > MAXLEVEL) {
			    yyerror("Rel base out of dlevel range - zeroing!");
			    couple.base = couple.rand = 0;
			} else {
			    couple.base = yyvsp[-3].i;
			    couple.rand = yyvsp[-1].i;
			}
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
