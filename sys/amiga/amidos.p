/*    SCCS Id: @(#)amidos.p 	3.1    93/01/08
/* Copyright (c) Kenneth Lorber, Bethesda, Maryland, 1992, 1993. */
/* NetHack may be freely redistributed.  See license for details. */

/* amidos.c */
void NDECL(flushout );
#ifndef	getuid
int NDECL(getuid );
#endif
#ifndef	getpid
int NDECL(getpid );
#endif
#ifndef	getlogin
char *NDECL(getlogin );
#endif
#ifndef	abs
int abs(int );
#endif
int NDECL(tgetch );
int NDECL(dosh );
long freediskspace(char *);
long filesize(char *);
void eraseall(const char * , const char *);
char *CopyFile(const char * , const char *);
void copybones(int );
void NDECL(playwoRAMdisk );
int saveDiskPrompt(int );
void NDECL(gameDiskPrompt );
void append_slash(char *);
void getreturn(const char *);
#ifndef msmsg
void msmsg(const char *, ... );
#endif
#if !defined(__SASC_60) && !defined(_DCC)
int chdir(char *);
#endif
#ifndef	strcmpi
int strcmpi(char * , char *);
#endif
#if !defined(memcmp) && !defined(AZTEC_C) && !defined(_DCC) && !defined(__GNUC__)
int memcmp(unsigned char * , unsigned char * , int );
#endif
