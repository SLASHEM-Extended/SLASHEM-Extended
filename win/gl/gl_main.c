/* Copyright (C) 2002 Andrew Apted <ajapted@users.sourceforge.net> */
/* NetHack may be freely redistributed.  See license for details.  */

/*
 * SDL/GL window port for NetHack & Slash'EM.
 *
 * Main code for SDL/GL port.  Contains the interface table
 * (sdlgl_procs) to the main nethack code.  Initialises and shuts down
 * SDL.  Sets up initial GL parameters.  Handles events.  Also
 * contains some miscellaneous utility routines.
 */

#include "hack.h"
#include "dlb.h"
#include "patchlevel.h"

#if defined(GL_GRAPHICS) || defined(SDL_GRAPHICS)

#define WINGL_INTERNAL
#include "winGL.h"

#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <stdarg.h>

#include <string.h>
#include <ctype.h>


/* character munging macros for CTRL and ALT/META */
#ifndef C
#define C(c)   (0x1f & (c))
#endif

#ifndef M
#define M(c)   (0x80 | (c))
#endif


int sdlgl_initialized = 0;

/* main sdl surface */
SDL_Surface *sdlgl_surf;

/* whether running in software or hardware mode */
int sdlgl_software = 0;

/* rendering procedures (either GL or software) */
struct rendering_procs sdlgl_rend_procs;


static struct timeval base_time;

static char err_msg_buf[2048];

static int cur_key_sym = SDLK_LAST;

#ifdef GL_GRAPHICS
static void Sdlgl_init_nhwindows_gl (int *argcp, char **argv);
#endif
#ifdef SDL_GRAPHICS
static void Sdlgl_init_nhwindows_sdl(int *argcp, char **argv);
#endif


#define SDLGL_WC_FLAGS  \
    ( WC_COLOR | WC_HILITE_PET  \
    | WC_TILE_HEIGHT  \
    | WC_FONTSIZ_MAP | WC_FONTSIZ_MENU | WC_FONTSIZ_MESSAGE  \
    | WC_FONTSIZ_STATUS | WC_FONTSIZ_TEXT  \
    | WC_ALIGN_MESSAGE | WC_ALIGN_STATUS  \
    | WC_SCROLL_MARGIN | WC_SPLASH_SCREEN  \
    | WC_VARY_MSGCOUNT)


/* Interface definition, Hardware version */
#ifdef GL_GRAPHICS
struct window_procs sdlgl_hardw_procs = 
{
    "GL", SDLGL_WC_FLAGS,
    Sdlgl_init_nhwindows_gl,
    Sdlgl_player_selection, Sdlgl_askname,
    Sdlgl_get_nh_event,
    Sdlgl_exit_nhwindows,
    Sdlgl_suspend_nhwindows, Sdlgl_resume_nhwindows,
    Sdlgl_create_nhwindow, Sdlgl_clear_nhwindow,
    Sdlgl_display_nhwindow, Sdlgl_destroy_nhwindow,
    Sdlgl_curs,
    Sdlgl_putstr, Sdlgl_display_file,
    Sdlgl_start_menu, Sdlgl_add_menu,
    Sdlgl_end_menu, Sdlgl_select_menu,
    Sdlgl_message_menu,
    Sdlgl_update_inventory,
    Sdlgl_mark_synch, Sdlgl_wait_synch,
#ifdef CLIPPING
    Sdlgl_cliparound,
#endif
#ifdef POSITIONBAR
    Sdlgl_update_positionbar,
#endif
    Sdlgl_print_glyph,
    Sdlgl_raw_print, Sdlgl_raw_print_bold,
    Sdlgl_nhgetch, Sdlgl_nh_poskey,
    Sdlgl_nhbell, Sdlgl_doprev_message,
    Sdlgl_yn_function, Sdlgl_getlin,
    Sdlgl_get_ext_cmd,
    Sdlgl_number_pad, Sdlgl_delay_output,
#ifdef CHANGE_COLOR
    donull, donull,
#endif
    Sdlgl_start_screen, Sdlgl_end_screen,
    Sdlgl_outrip, Sdlgl_preference_update
};
#endif

/* Interface definition, Software version. Only difference is the name
 * field and the init function.
 */
#ifdef SDL_GRAPHICS
struct window_procs sdlgl_softw_procs = 
{
    "SDL", SDLGL_WC_FLAGS,
    Sdlgl_init_nhwindows_sdl,
    Sdlgl_player_selection, Sdlgl_askname,
    Sdlgl_get_nh_event,
    Sdlgl_exit_nhwindows,
    Sdlgl_suspend_nhwindows, Sdlgl_resume_nhwindows,
    Sdlgl_create_nhwindow, Sdlgl_clear_nhwindow,
    Sdlgl_display_nhwindow, Sdlgl_destroy_nhwindow,
    Sdlgl_curs,
    Sdlgl_putstr, Sdlgl_display_file,
    Sdlgl_start_menu, Sdlgl_add_menu,
    Sdlgl_end_menu, Sdlgl_select_menu,
    Sdlgl_message_menu,
    Sdlgl_update_inventory,
    Sdlgl_mark_synch, Sdlgl_wait_synch,
#ifdef CLIPPING
    Sdlgl_cliparound,
#endif
#ifdef POSITIONBAR
    Sdlgl_update_positionbar,
#endif
    Sdlgl_print_glyph,
    Sdlgl_raw_print, Sdlgl_raw_print_bold,
    Sdlgl_nhgetch, Sdlgl_nh_poskey,
    Sdlgl_nhbell, Sdlgl_doprev_message,
    Sdlgl_yn_function, Sdlgl_getlin,
    Sdlgl_get_ext_cmd,
    Sdlgl_number_pad, Sdlgl_delay_output,
#ifdef CHANGE_COLOR
    donull, donull,
#endif
    Sdlgl_start_screen, Sdlgl_end_screen,
    Sdlgl_outrip, Sdlgl_preference_update
};
#endif


/* Display a warning/error.  These should have a trailing \n (unlike
 * the raw_print routines, see below).
 */
void sdlgl_warning(const char *str, ...)
{
  va_list argptr;
   
  err_msg_buf[sizeof(err_msg_buf) - 1] = 0;
  
  va_start(argptr, str);
  vsprintf(err_msg_buf, str, argptr);
  va_end(argptr);

  /* overflow ? */
  assert(err_msg_buf[sizeof(err_msg_buf) - 1] == 0);

  fprintf(stderr, "-- " SDLGL_PROGRAM ": %s", err_msg_buf);
}

void sdlgl_error(const char *str, ...)
{
  if (str[0])
  {
    va_list argptr;
     
    err_msg_buf[sizeof(err_msg_buf) - 1] = 0;
    
    va_start(argptr, str);
    vsprintf(err_msg_buf, str, argptr);
    va_end(argptr);

    /* overflow ? */
    assert(err_msg_buf[sizeof(err_msg_buf) - 1] == 0);

    fprintf(stderr, "** " SDLGL_PROGRAM ": %s", err_msg_buf);
  }

  clearlocks();
  Sdlgl_exit_nhwindows("");
  terminate(EXIT_SUCCESS);

  /*NOTREACHED*/
}

void sdlgl_hangup(const char *str, ...)
{
  if (str[0])
  {
    va_list argptr;
     
    err_msg_buf[sizeof(err_msg_buf) - 1] = 0;
    
    va_start(argptr, str);
    vsprintf(err_msg_buf, str, argptr);
    va_end(argptr);

    /* overflow ? */
    assert(err_msg_buf[sizeof(err_msg_buf) - 1] == 0);

    fprintf(stderr, "!! " SDLGL_PROGRAM ": %s", err_msg_buf);
  }

  hangup(1);

  /*NOTREACHED*/
  
  clearlocks();
  Sdlgl_exit_nhwindows("");
  terminate(EXIT_SUCCESS);

  /*NOTREACHED -- AGAIN!*/
}

void Sdlgl_nhbell(void)
{
  /* does nothing */
}

void Sdlgl_number_pad(int state)
{
  /* does nothing */
}

/* time utilities.  sdlgl_get_time() returns milliseconds since
 * some epoch (e.g. since the program started).  sdlgl_sleep() will
 * delay for a given number of milliseconds (exact amount not
 * guaranteed -- may be longer due to process switches, etc).
 */
static void sdlgl_init_time(void)
{
  gettimeofday(&base_time, NULL);
}

int sdlgl_get_time(void)
{
  int secs, millis;
  struct timeval tv;

  gettimeofday(&tv, NULL);

  secs = tv.tv_sec - base_time.tv_sec;
  millis = (tv.tv_usec - base_time.tv_usec) / 1000;
   
  /* fix for any "time going backwards" kernel bug */
  if (secs < 0 || (secs == 0 && millis < 0))
    return 0;

  return secs * 1000 + millis;
}

void sdlgl_sleep(int millis)
{
  if (millis > 0)
    usleep(millis * 1000);
}

void Sdlgl_delay_output(void)
{
  /* delay 50 ms */
  sdlgl_sleep(50);
}


static void sdlgl_do_init_nhwindows(int *argcp, char **argv)
{
  Uint32 flags;
  
  CO = 80;
  LI = 25;

  /* handle arguments */

  sdlgl_parse_cmdline_options(argcp, argv);

  sdlgl_validate_wincap_options();
  sdlgl_validate_gl_options();


  /* --------- SDL ----------- */

  if (SDL_Init(SDL_INIT_VIDEO | (0*SDL_INIT_NOPARACHUTE)) < 0)
    sdlgl_error("Failed to initialise the SDL library.\n");

  flags = 0;

  if (sdlgl_software)
  {
    flags |= SDL_SWSURFACE;

    if (sdlgl_depth == 8 && ! sdlgl_windowed)
      flags |= SDL_HWPALETTE;
  }
  else
  {
    flags |= SDL_OPENGL;
    flags |= SDL_DOUBLEBUF;  /* not needed ?? */

    switch (sdlgl_depth)
    {
      case 15:
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE,   5);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,  5);
        break;

      case 16:
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE,   5);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,  5);
        break;

      case 24:
      case 32:
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE,   8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,  8);
        break;
    }
    
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,   0);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
  }

  if (! sdlgl_windowed)
    flags |= SDL_FULLSCREEN;

  sdlgl_surf = SDL_SetVideoMode(sdlgl_width, sdlgl_height, sdlgl_depth, flags);

  if (sdlgl_surf == NULL)
  {
    SDL_Quit();

    sdlgl_warning("Failed to set SDL/GL video mode %dx%d (%d bit, %s)\n",
        sdlgl_width, sdlgl_height, sdlgl_depth, 
        sdlgl_windowed ? "windowed" : "fullscreen");

    sdlgl_error("Please try a different mode or depth (see --help).\n");
  }

  SDL_EnableUNICODE(1);
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
  SDL_WM_SetCaption(SDLGL_PROGRAM, SDLGL_ICON);

  /* move the mouse pointer out of the way */
  SDL_WarpMouse(sdlgl_width - 32, sdlgl_height - 32);

  sdlgl_init_time();

  sdlgl_rend_startup();
  sdlgl_tile_startup();
  sdlgl_font_startup();

  /* show the logo & copyright */
  if (iflags.wc_splash_screen)
  {
    sdlgl_start_logo();

    while (! sdlgl_iterate_logo())
      continue;
  }

  sdlgl_tile_load_rest();

  sdlgl_emul_startup();
  sdlgl_win_startup();

  sdlgl_initialized = 1;
}

#ifdef GL_GRAPHICS
void Sdlgl_init_nhwindows_gl(int *argcp, char **argv)
{
  sdlgl_software = 0;
  sdlgl_rend_procs = sdlgl_hardw_rend_procs;

  sdlgl_do_init_nhwindows(argcp, argv);
}
#endif

#ifdef SDL_GRAPHICS
void Sdlgl_init_nhwindows_sdl(int *argcp, char **argv)
{
  sdlgl_software = 1;
  sdlgl_rend_procs = sdlgl_softw_rend_procs;

  sdlgl_do_init_nhwindows(argcp, argv);
}
#endif

void Sdlgl_exit_nhwindows(const char *str)
{
  if (str && str[0])
    Sdlgl_raw_print_bold(str);

  if (! sdlgl_initialized)
    return;

  sdlgl_initialized = 0;

  sdlgl_win_shutdown();
  sdlgl_emul_shutdown();
  sdlgl_font_shutdown();
  sdlgl_tile_shutdown();
  sdlgl_rend_shutdown();

  SDL_Quit();
}

void Sdlgl_suspend_nhwindows(const char *str)
{
  /* does nothing */
}

void Sdlgl_resume_nhwindows(void)
{
  /* does nothing */
}

void Sdlgl_get_nh_event(void)
{
  /* does nothing */
}

void Sdlgl_update_inventory()
{
  /* does nothing */
}

void Sdlgl_mark_synch()
{
  /* does nothing */
}

void Sdlgl_wait_synch()
{
  /* Unfortately, this routine may be called before we are
   * initialized...
   */
  if (! sdlgl_initialized)
  {
    sdlgl_sleep(1000);
    return;
  }

  sdlgl_flush();
}

/* These raw_print routines don't need a trailing \n.
 */
void Sdlgl_raw_print(const char *str)
{
  fprintf(stderr, "%s\n", str);
}

void Sdlgl_raw_print_bold(const char *str)
{
  Sdlgl_raw_print(str);
}

static int convert_keysym(int sym, int unicode, int mod)
{
  int shift = mod & KMOD_SHIFT;
  int ctrl  = mod & KMOD_CTRL;
  int alt   = mod & KMOD_ALT;

  (void) ctrl;
  
  /* CTRL + letter ? */
  if (0x01 <= unicode && unicode <= 0x1F)
    return unicode;

  /* is it printable ASCII ? */
  if (0x20 <= unicode && unicode <= 0x7E)
  {
    /* ALT + letter ? */
    if (letter(unicode) && alt)
      return M(unicode);

    return unicode;
  }
  
  switch (sym)
  {
    /* standard control keys */
    case SDLK_BACKSPACE: return '\b';
    case SDLK_RETURN:    return '\n';
    case SDLK_ESCAPE:    return '\033';
    case SDLK_TAB:       return '\t';

    /* cursor keys & keypad */
    case SDLK_UP:
      return iflags.num_pad ? '8' : shift ? 'K' : 'k';
     
    case SDLK_DOWN:
      return iflags.num_pad ? '2' : shift ? 'J' : 'j';
     
    case SDLK_LEFT:
      return iflags.num_pad ? '4' : shift ? 'H' : 'h';

    case SDLK_RIGHT:
      return iflags.num_pad ? '6' : shift ? 'L' : 'l';

    /* AJA: no special handling for numeric keypad, always return
     *      digits, and let the main code handle any conversion to
     *      movement via the "number_pad" option.
     */
    case SDLK_KP0: return '0';
    case SDLK_KP1: return '1';
    case SDLK_KP2: return '2';
    case SDLK_KP3: return '3';
    case SDLK_KP4: return '4';
    case SDLK_KP5: return '5';
    case SDLK_KP6: return '6';
    case SDLK_KP7: return '7';
    case SDLK_KP8: return '8';
    case SDLK_KP9: return '9';
    case SDLK_PERIOD: return '.';
     
    /* stuff we should ignore */
    case SDLK_RSHIFT: case SDLK_LSHIFT:
    case SDLK_RCTRL:  case SDLK_LCTRL:
    case SDLK_RALT:   case SDLK_LALT:
    case SDLK_RMETA:  case SDLK_LMETA:
    case SDLK_LSUPER: case SDLK_RSUPER:
    case SDLK_MODE:   case SDLK_COMPOSE:
      return 0;

    default:
      break;
  }

#if 0
  raw_printf("convert_keysym: UNKNOWN SDL KEY %d", sym);
#endif
  
  /* unknown key */
  return 0;
}

/* main internal key/mouse routine.
 */
int sdlgl_get_poskey(int flags, int *x, int *y, int *mod)
{
  SDL_Event ev;

  for (;;)
  {
    if (SDL_PollEvent(&ev) == 0)
    {
      /* wait for input to come */
      sdlgl_sleep(40);
      continue;
    }

    switch (ev.type)
    {
      case SDL_KEYDOWN:
      {
        int sym = ev.key.keysym.sym;
        int unicode = ev.key.keysym.unicode;
        int mod = ev.key.keysym.mod;
        int repeat = 0;

        int key;
        
        /* check for auto-repeat */
        if (cur_key_sym == sym)
          repeat = 1;
        else
          cur_key_sym = sym;
        
        if (repeat && sdlgl_key_repeat == 0)
          break;

        /* we eat some keys for our own purposes, like zooming the map
         * in/out or scrolling through text or menu windows.
         */
        if (sdlgl_internal_key_handler(&ev.key.keysym, repeat))
          break;

        if (repeat && sdlgl_key_repeat == 1 &&
            ! (flags & POSKEY_ALLOW_REPEAT) && ! sdlgl_cursor_visible())
        {
          break;
        }
  
        key = convert_keysym(sym, unicode, mod);
        
        if (key == 0)
          break;

        if (sdlgl_alt_prev)
        {
          /* handle alternate scrollback mode.  When ^P is pressed and
           * the scrollback window is mapped, we simply update it here
           * (rather than pass the ^P to the main NetHack code).  When
           * some other key is pressed, we remove the scrollback win.
           */
          struct TextWindow *win = 0;

          if (WIN_MESSAGE != WIN_ERR)
            win = text_wins[WIN_MESSAGE];

          if (key != 16 /* ^P */ && key != 15 /* ^O */)
          {
            sdlgl_remove_scrollback();
          }
          else if (win && win->scrollback_pos > 0)
          {
            sdlgl_adjust_scrollback(win, (key == 15) ? -1 : +1);
            continue;
          }
        }
            
        /* AJA: filter out ^Z -- causes an apparent lock-up when in
         *      fullscreen mode (ugh).
         */
        if (key != 26 /* ^Z */)
          return key;

        break;
      }

      case SDL_QUIT:
        sdlgl_hangup("Received QUIT signal from SDL.\n");
        break;

      case SDL_KEYUP:
        cur_key_sym = SDLK_LAST;
        break;

      case SDL_MOUSEBUTTONDOWN:
        switch (ev.button.button)
        {
          case SDL_BUTTON_LEFT: 
            *mod = CLICK_1;
            break;

          case SDL_BUTTON_MIDDLE: 
          case SDL_BUTTON_RIGHT: 
            *mod = CLICK_2;
            break;

          default:
            continue;
        }

        *x = ev.button.x;
        *y = ev.button.y;

        if (! sdlgl_map_find_click(x, y))
          continue;

        /* return the mouse click */
        return 0;

      case SDL_ACTIVEEVENT:
        if (ev.active.gain)
          sdlgl_flush();
        break;

#if (SDL_MAJOR_VERSION * 100 + SDL_MINOR_VERSION) >= 102
      /* only exists in SDL 1.2 and higher */
      case SDL_VIDEOEXPOSE:
        sdlgl_flush();
        break;
#endif

      default:
        break;
    }
  }

  /* not actually reached */
  return -1;
}

int sdlgl_get_key(int flags)
{
  int x, y, mod;
  
  for (;;)
  {
    int key = sdlgl_get_poskey(flags, &x, &y, &mod);

    /* just ignore any mouse events */
    if (key)
      return key;
  }
}

int Sdlgl_nhgetch(void)
{
  /* update status bar */
  {
    sdlgl_update_status(1);

    if (WIN_STATUS != WIN_ERR && text_wins[WIN_STATUS] != NULL)
      sdlgl_write_status(text_wins[WIN_STATUS]);
  }

  /* we rely on this flush in many places */
  sdlgl_flush();

  return sdlgl_get_key(POSKEY_DO_SCROLLBACK);
}

/*
 * return a key, or 0, in which case a mouse button was pressed
 * mouse events should be returned as character postitions in the map
 * window.
 */
int Sdlgl_nh_poskey(int *x, int *y, int *mod)
{
  /* update status bar */
  {
    sdlgl_update_status(1);

    if (WIN_STATUS != WIN_ERR && text_wins[WIN_STATUS] != NULL)
      sdlgl_write_status(text_wins[WIN_STATUS]);
  }

  sdlgl_flush();

  return sdlgl_get_poskey(POSKEY_DO_SCROLLBACK, x, y, mod);
}

#endif /* GL_GRAPHICS */
/*gl_main.c*/
