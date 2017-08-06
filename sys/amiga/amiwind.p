/*   SCCS Id: @(#)amiwind.p	3.1	93/01/08		  */
/*   Copyright (c) Gregg Wonderly, Naperville, IL, 1992, 1993	  */
/* NetHack may be freely redistributed.  See license for details. */

/* amiwind.c */
#ifdef	INTUI_NEW_LOOK
struct Window *OpenShWindow(struct ExtNewWindow *);
#else
struct Window *OpenShWindow(struct NewWindow *);
#endif
void CloseShWindow(struct Window *);
int NDECL( kbhit );
int NDECL( amikbhit );
int NDECL( WindowGetchar );
WETYPE NDECL( WindowGetevent );
void NDECL( WindowFlush );
void WindowPutchar(char );
void WindowFPuts(const char *);
void WindowPuts(const char *);
void WindowPrintf(char *,... );
void NDECL( CleanUp );
int ConvertKey(struct IntuiMessage * );
#ifndef	SHAREDLIB
void Abort(long );
#endif
void flush_glyph_buffer(struct Window *);
void amiga_print_glyph(winid , int , int );
void start_glyphout(winid );
void amii_end_glyphout(winid );
#ifdef	INTUI_NEW_LOOK
struct ExtNewWindow *DupNewWindow(struct ExtNewWindow *);
void FreeNewWindow(struct ExtNewWindow *);
#else
struct NewWindow *DupNewWindow(struct NewWindow *);
void FreeNewWindow(struct NewWindow *);
#endif
void NDECL( bell );
void NDECL( amii_delay_output );
void amii_number_pad(int );
void amii_cleanup( void );
