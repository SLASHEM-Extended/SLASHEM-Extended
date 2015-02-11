/*	SCCS Id: @(#)patchlevel.h	3.4	2003/12/06	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/*The name of the compiled game- should be same as stuff in makefile*/
/*for makedefs*/
/* KMH -- Made it mixed case, from which upper & lower case versions are made */
#define DEF_GAME_NAME   "SlashEM Extended"
/*#define DEF_GAME_NAME   "NetHack"*/

/* Version */
/* edit by Amy - version_minor is updated for each release, version_major increases by 1 if savegame compatibility is broken */
/* if version_major increases, version_minor resets to 0 */
#define VERSION_MAJOR   98
#define VERSION_MINOR   0
/*
 * PATCHLEVEL is updated for each release.
 * edit by Amy - I'm keeping patch, edit and fixlevel to show the game's based on SLASH'EM 0.0.7E7F2.
 */
#define PATCHLEVEL      7
#define EDITLEVEL	7
#define FIXLEVEL        2

#define COPYRIGHT_BANNER_A \
"This is SuperLotsoAddedStuffHack-Extended Magic 1997-2005"

#define COPYRIGHT_BANNER_B \
"NetHack, Copyright 1985-2003 Stichting Mathematisch Centrum, M. Stephenson."

#define COPYRIGHT_BANNER_C \
"Slash'EM Extended version by Amy (user Bluescreenofdeath at Nethackwiki)."

#define COPYRIGHT_BANNER_D \
"See license for details. Bug reports to Bluescreenofdeath's user talk page."

#if 1
/*
 * If two or more successive releases have compatible data files, define
 * this with the version number of the oldest such release so that the
 * new release will accept old save and bones files.  The format is
 *	0xMMmmPPeeL
 * 0x = literal prefix "0x", MM = major version, mm = minor version,
 * PP = patch level, ee = edit level, L = literal suffix "L",
 * with all four numbers specified as two hexadecimal digits.
 */
#define VERSION_COMPATIBILITY 0x00000702L
#endif

/*patchlevel.h*/
