/*      SCCS Id: @(#)file.h       3.2     96/11/19        */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* WAC New file used by Makedefs*/
/* This file is read by makedefs and is turned into file.h*/
/* Various File Names */
/*Makedefs will hunt for the @ sign and then remove the next 6 chars*/
/*replacing with DEF_GAME_NAME in patchlev.h*/
/*Makedefs starts reading after a line with # START*/
/* @Lxxxxx makes DEF_GAME_NAME lowercase*/

# START

/* Config Filename */
#ifdef UNIX                
# define NH_CONFIG_FILE     ".@LOWER@rc"
#else
# if defined(MAC) || defined(__BEOS__)
#  define NH_CONFIG_FILE     "Slash'EM Defaults"
# else                     
#  ifdef VMS                
#   define NH_CONFIG_FILE     "@LOWER@ini"
/*file2 should be the same as file except with sys$login: prepend*/
#   define NH_CONFIG_FILE2    "sys$login:@LOWER@.ini"
#   define NH_CONFIG_FILE3    "@UPPER@.cnf"
#  else                     
#   if defined(MSDOS) || defined(WIN32)
#    define NH_CONFIG_FILE     "defaults.nh";
#   else                     
#    define NH_CONFIG_FILE     "@UPPER@.cnf"
#   endif
#  endif
# endif
#endif

/* Tile Files*/
#define NETHACK_PLANAR_TILEFILE "@UPPER@1.tib"  /* Planar style tiles */
#define NETHACK_PACKED_TILEFILE "@UPPER@2.tib"  /* Packed style tiles */
#define NETHACK_OVERVIEW_TILEFILE "@UPPER@o.tib" /* thin overview tiles */

/* Environment Options Name */
#define NETHACK_ENV_OPTIONS "@UPPER@OPTIONS"
