/*	SCCS Id: @(#)windows.c	3.4	1996/05/19	*/
/* Copyright (c) D. Cohrs, 1993. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#ifdef TTY_GRAPHICS
#include "wintty.h"
#endif
#ifdef CURSES_GRAPHICS
extern struct window_procs curses_procs;
#endif
#ifdef X11_GRAPHICS
/* cannot just blindly include winX.h without including all of X11 stuff */
/* and must get the order of include files right.  Don't bother */
extern struct window_procs X11_procs;
extern void win_X11_init(void);
#endif
#ifdef QT_GRAPHICS
extern struct window_procs Qt_procs;
#endif
#ifdef GTK_GRAPHICS
/*
 * GTK interface (By issei@guru.gr.jp)
 */
extern struct window_procs GTK_procs;
extern void win_GTK_init(void);
#endif
#ifdef GEM_GRAPHICS
#include "wingem.h"
#endif
#ifdef MAC
extern struct window_procs mac_procs;
#endif
#ifdef BEOS_GRAPHICS
extern struct window_procs beos_procs;
extern void be_win_init(void);
#endif
#ifdef AMIGA_INTUITION
extern struct window_procs amii_procs;
extern struct window_procs amiv_procs;
extern void ami_wininit_data(void);
#endif
#ifdef WIN32_GRAPHICS
extern struct window_procs win32_procs;
#endif
#ifdef GNOME_GRAPHICS
#include "winGnome.h"
extern struct window_procs Gnome_procs;
#endif
#ifdef GL_GRAPHICS
#include "winGL.h"
extern struct window_procs sdlgl_hardw_procs;
#endif
#ifdef SDL_GRAPHICS
#include "winGL.h"
extern struct window_procs sdlgl_softw_procs;
#endif
#ifdef PROXY_GRAPHICS
#include "winproxy.h"
extern struct window_procs proxy_procs;
extern void win_proxy_init(void);
#endif
#ifdef MSWIN_GRAPHICS
extern struct window_procs mswin_procs;
#endif

STATIC_DCL void def_raw_print(const char *s);

#ifdef HANGUPHANDLING
volatile
#endif
NEARDATA struct window_procs windowprocs;

static
struct win_choices {
    struct window_procs *procs;
    void (*ini_routine)(void);		/* optional (can be 0(void) */
} winchoices[] = {
#ifdef TTY_GRAPHICS
    { &tty_procs, win_tty_init },
#endif
#ifdef CURSES_GRAPHICS
    { &curses_procs, 0 },
#endif
#ifdef X11_GRAPHICS
    { &X11_procs, win_X11_init },
#endif
#ifdef QT_GRAPHICS
    { &Qt_procs, 0 },
#endif
#ifdef GTK_GRAPHICS
    { &GTK_procs, win_GTK_init },
#endif
#ifdef GEM_GRAPHICS
    { &Gem_procs, win_Gem_init },
#endif
#ifdef MAC
    { &mac_procs, 0 },
#endif
#ifdef BEOS_GRAPHICS
    { &beos_procs, be_win_init },
#endif
#ifdef AMIGA_INTUITION
    { &amii_procs, ami_wininit_data },		/* Old font version of the game */
    { &amiv_procs, ami_wininit_data },		/* Tile version of the game */
#endif
#ifdef WIN32_GRAPHICS
    { &win32_procs, 0 },
#endif
#ifdef GNOME_GRAPHICS
    { &Gnome_procs, 0 },
#endif
#ifdef GL_GRAPHICS
    { &sdlgl_hardw_procs, 0 },
#endif
#ifdef SDL_GRAPHICS
    { &sdlgl_softw_procs, 0 },
#endif
#ifdef PROXY_GRAPHICS
    { &proxy_procs, win_proxy_init },
#endif
#ifdef MSWIN_GRAPHICS
    { &mswin_procs, 0 },
#endif
    { 0, 0 }		/* must be last */
};

/*
 * tty_message_menu() provides a means to get feedback from the
 * --More-- prompt; other interfaces generally don't need that.
 */
/*ARGSUSED*/
char
genl_message_menu(int let,
                  int how,
                  const char *mesg)
{
    pline("%s", mesg);
    return 0;
}

/*ARGSUSED*/
void
genl_preference_update(const char *pref)
{
    /* window ports are expected to provide
       their own preference update routine
       for the preference capabilities that
       they support.
       Just return in this genl one. */
    return;
}

char *
genl_getmsghistory(boolean init)
{
    /* window ports can provide
       their own getmsghistory() routine to
       preserve message history between games.
       The routine is called repeatedly from
       the core save routine, and the window
       port is expected to successively return
       each message that it wants saved, starting
       with the oldest message first, finishing
       with the most recent.
       Return null pointer when finished.
     */
    return (char *) 0;
}

void
genl_putmsghistory(const char *msg, boolean is_restoring)
{
    /* window ports can provide
       their own putmsghistory() routine to
       load message history from a saved game.
       The routine is called repeatedly from
       the core restore routine, starting with
       the oldest saved message first, and
       finishing with the latest.
       The window port routine is expected to
       load the message recall buffers in such
       a way that the ordering is preserved.
       The window port routine should make no
       assumptions about how many messages are
       forthcoming, nor should it assume that
       another message will follow this one,
       so it should keep all pointers/indexes
       intact at the end of each call.
    */

    /* this doesn't provide for reloading the message window with the
       previous session's messages upon restore, but it does put the quest
       message summary lines there by treating them as ordinary messages */
    if (!is_restoring)
        pline("%s", msg);
    return;
}

#ifdef HANGUPHANDLING
/*
 * Dummy windowing scheme used to replace current one with no-ops
 * in order to avoid all terminal I/O after hangup/disconnect.
 */

static int hup_nhgetch(void);
static char hup_yn_function(const char *, const char *, int);
static int hup_nh_poskey(int *, int *, int *);
static void hup_getlin(const char *, char *);
static void hup_init_nhwindows(int *, char **);
static void hup_exit_nhwindows(const char *);
static winid hup_create_nhwindow(int);
static int hup_select_menu(winid, int, MENU_ITEM_P **);
static void hup_add_menu(winid, int, const anything *, int,
                         int, int, const char *, int);
static void hup_end_menu(winid, const char *);
static void hup_putstr(winid, int, const char *);
static void hup_print_glyph(winid, int, int, int);
static void hup_outrip(winid, int);
static void hup_curs(winid, int, int);
static void hup_display_nhwindow(winid, int);
static void hup_display_file(const char *, int);
#ifdef CLIPPING
static void hup_cliparound(int, int);
#endif
#ifdef CHANGE_COLOR
static void hup_change_color(int, long, int);
#ifdef MAC
static short hup_set_font_name(winid, char *);
#endif
static char *hup_get_color_string(void);
#endif /* CHANGE_COLOR */
static void hup_status_update(int, void *, int, int, int,
                              unsigned long *);

static int hup_int_ndecl(void);
static void hup_void_ndecl(void);
static void hup_void_fdecl_int(int);
static void hup_void_fdecl_winid(winid);
static void hup_void_fdecl_winid_ulong(winid, unsigned long);
static void hup_void_fdecl_constchar_p(const char *);

static struct window_procs hup_procs = {
    "hup", 0L, 0L,
    hup_init_nhwindows,
    hup_void_ndecl,                                    /* player_selection */
    hup_void_ndecl,                                    /* askname */
    hup_void_ndecl,                                    /* get_nh_event */
    hup_exit_nhwindows, hup_void_fdecl_constchar_p,    /* suspend_nhwindows */
    hup_void_ndecl,                                    /* resume_nhwindows */
    hup_create_nhwindow, hup_void_fdecl_winid,         /* clear_nhwindow */
    hup_display_nhwindow, hup_void_fdecl_winid,        /* destroy_nhwindow */
    hup_curs, hup_putstr,
    hup_display_file, hup_void_fdecl_winid,      /* start_menu */
    hup_add_menu, hup_end_menu, hup_select_menu, genl_message_menu,
    hup_void_ndecl,                                /* update_inventory */
    hup_void_ndecl,                                    /* mark_synch */
    hup_void_ndecl,                                    /* wait_synch */
#ifdef CLIPPING
    hup_cliparound,
#endif
#ifdef POSITIONBAR
    (void (*)(char *)) hup_void_fdecl_constchar_p,    /* update_positionbar */
#endif
    hup_print_glyph,
    hup_void_fdecl_constchar_p,                       /* raw_print */
    hup_void_fdecl_constchar_p,                       /* raw_print_bold */
    hup_nhgetch, hup_nh_poskey, hup_void_ndecl,       /* nhbell  */
    hup_int_ndecl,                                    /* doprev_message */
    hup_yn_function, hup_getlin, hup_int_ndecl,       /* get_ext_cmd */
    hup_void_fdecl_int,                               /* number_pad */
    hup_void_ndecl,                                   /* delay_output  */
#ifdef CHANGE_COLOR
    hup_change_color,
#ifdef MAC
    hup_void_fdecl_int,                               /* change_background */
    hup_set_font_name,
#endif
    hup_get_color_string,
#endif /* CHANGE_COLOR */
    hup_void_ndecl,                                   /* start_screen */
    hup_void_ndecl,                                   /* end_screen */
    hup_outrip, genl_preference_update,
};

static void (*previnterface_exit_nhwindows)(const char *) = 0;

/* hangup has occurred; switch to no-op user interface */
void
nhwindows_hangup(void)
{
    char *(*previnterface_getmsghistory)(boolean) = 0;

#ifdef ALTMETA
    /* command processor shouldn't look for 2nd char after seeing ESC */
    iflags.altmeta = FALSE;
#endif

    /* don't call exit_nhwindows() directly here; if a hangup occurs
       while interface code is executing, exit_nhwindows could knock
       the interface's active data structures out from under itself */
    if (iflags.window_inited
        && windowprocs.win_exit_nhwindows != hup_exit_nhwindows)
        previnterface_exit_nhwindows = windowprocs.win_exit_nhwindows;

    windowprocs = hup_procs;
}

static void
hup_exit_nhwindows(const char *lastgasp)
{
    /* core has called exit_nhwindows(); call the previous interface's
       shutdown routine now; xxx_exit_nhwindows() needs to call other
       xxx_ routines directly rather than through windowprocs pointers */
    if (previnterface_exit_nhwindows) {
        lastgasp = 0; /* don't want exit routine to attempt extra output */
        (*previnterface_exit_nhwindows)(lastgasp);
        previnterface_exit_nhwindows = 0;
    }
    iflags.window_inited = 0;
}

static int
hup_nhgetch(void)
{
    return '\033'; /* ESC */
}

/*ARGSUSED*/
static char
hup_yn_function(const char *prompt,
                const char *resp,
                int deflt)
{
    if (!deflt)
        deflt = '\033';
    return deflt;
}

/*ARGSUSED*/
static int
hup_nh_poskey(int *x, int *y, int *mod)
{
    return '\033';
}

/*ARGSUSED*/
static void
hup_getlin(const char *prompt, char *outbuf)
{
    strcpy(outbuf, "\033");
}

/*ARGSUSED*/
static void
hup_init_nhwindows(int *argc_p, char **argv)
{
    iflags.window_inited = 1;
}

/*ARGUSED*/
static winid
hup_create_nhwindow(int type)
{
    return WIN_ERR;
}

/*ARGSUSED*/
static int
hup_select_menu(winid window, int how,
                struct mi **menu_list)
{
    return -1;
}

/*ARGSUSED*/
static void
hup_add_menu(winid window,
             int intglyphinfo,
             const anything *identifier,
             int sel,
             int grpsel,
             int attr,
             const char *txt,
             int itemflags)
{
    return;
}

/*ARGSUSED*/
static void
hup_end_menu(winid window, const char *prompt)
{
    return;
}

/*ARGSUSED*/
static void
hup_putstr(winid window, int attr, const char *text)
{
    return;
}

/*ARGSUSED*/
static void
hup_print_glyph(winid window,
                int x, int y,
                int glyphinfo)
{
    return;
}

/*ARGSUSED*/
static void
hup_outrip(winid tmpwin, int how)
{
    return;
}

/*ARGSUSED*/
static void
hup_curs(winid window, int x, int y)
{
    return;
}

/*ARGSUSED*/
static void
hup_display_nhwindow(winid window, int blocking)
{
    return;
}

/*ARGSUSED*/
static void
hup_display_file(const char *fname, int complain)
{
    return;
}

#ifdef CLIPPING
/*ARGSUSED*/
static void
hup_cliparound(int x, int y)
{
    return;
}
#endif

#ifdef CHANGE_COLOR
/*ARGSUSED*/
static void
hup_change_color(int color, int reverse, long rgb)
{
    return;
}

#ifdef MAC
/*ARGSUSED*/
static short
hup_set_font_name(winid window, char *fontname)
{
    return 0;
}
#endif /* MAC */

static char *
hup_get_color_string(void)
{
    return (char *) 0;
}
#endif /* CHANGE_COLOR */

/*ARGSUSED*/
static void
hup_status_update(int idx, void * ptr, int chg,
                  int pc, int color,
                  unsigned long *colormasks)
{
    return;
}

/*
 * Non-specific stubs.
 */

static int
hup_int_ndecl(void)
{
    return -1;
}

static void
hup_void_ndecl(void)
{
    return;
}

/*ARGUSED*/
static void
hup_void_fdecl_int(int arg)
{
    return;
}

/*ARGUSED*/
static void
hup_void_fdecl_winid(winid window)
{
    return;
}

/*ARGUSED*/
static void
hup_void_fdecl_winid_ulong(winid window,
                           unsigned long mbehavior)
{
    return;
}

/*ARGUSED*/
static void
hup_void_fdecl_constchar_p(const char *string)
{
    return;
}

#endif /* HANGUPHANDLING */

STATIC_OVL
void
def_raw_print(s)
const char *s;
{
    puts(s);
}

static int windows_lock = FALSE;

int
lock_windows(flag)
int flag;
{
    int retval = windows_lock;
    windows_lock = flag;
    return retval;
}

void
choose_windows(s)
const char *s;
{
    register int i;

    if (windows_lock)
	return;

    for(i=0; winchoices[i].procs; i++)
	if (!strcmpi(s, winchoices[i].procs->name)) {
	    windowprocs = *winchoices[i].procs;
	    if (winchoices[i].ini_routine) (*winchoices[i].ini_routine)();
	    return;
	}

    if (!windowprocs.win_raw_print)
	windowprocs.win_raw_print = def_raw_print;

    raw_printf("Window type %s not recognized.  Choices are:", s);
    for(i=0; winchoices[i].procs; i++)
	raw_printf("        %s", winchoices[i].procs->name);

    if (windowprocs.win_raw_print == def_raw_print)
	terminate(EXIT_SUCCESS);
    wait_synch();
}

/*windows.c*/
