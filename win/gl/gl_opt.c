/* Copyright (C) 2002 Andrew Apted <ajapted@users.sourceforge.net> */
/* NetHack may be freely redistributed.  See license for details.  */

/*
 * SDL/GL window port for NetHack & Slash'EM.
 *
 * Parses options, from both the command-line and the config file.
 */

#include "hack.h"
#include "dlb.h"
#include "patchlevel.h"
#include "date.h"

#if defined(GL_GRAPHICS) || defined(SDL_GRAPHICS)

#define WINGL_INTERNAL
#include "winGL.h"

#include <string.h>
#include <ctype.h>
#include <errno.h>


int sdlgl_width  = 0;
int sdlgl_height = 0;
int sdlgl_depth  = 0;

/* other configurable vars: ints... */
int sdlgl_windowed    = DEF_SDLGL_WINDOWED;
int sdlgl_key_repeat  = DEF_SDLGL_KEYREPEAT;
int sdlgl_jail_size   = 0;
int sdlgl_prev_step   = 0;
int sdlgl_gamma       = 0;  /* range is -5 to +5 */

static int fontsize_override = 0;

/* booleans... */
int sdlgl_def_zoom    = 0;
int sdlgl_msg_dim     = 0;
int sdlgl_alt_prev    = 0;
int sdlgl_reformat    = 0;
int sdlgl_shrink_wrap = 0;
int sdlgl_flipping    = 0;
int sdlgl_jump_scroll = 0;
int sdlgl_invis_fx    = 0;


enum OptionValueType
{
  VALTYPE_INTEGER,
  VALTYPE_BOOLEAN,   /* true or false, 1 or 0.  Can be absent */
  VALTYPE_DOUBLE,
  VALTYPE_STRING,
  VALTYPE_VID_MODE,  /* video mode like "800x600" */
  VALTYPE_KEY_REPEAT,
};

enum LocalOptionFlags
{
  /* option is only for backwards compatibility */
  LOPT_BACKW_COMPAT = 0x0001,

  /* option only available on the command line */
  LOPT_CMDLINE_ONLY = 0x0002,

  /* option only available in the config file.
   * (Note that LOPT_BACKW_COMPAT implies this).
   */
  LOPT_FILE_ONLY = 0x0004
};

struct LocalOption
{
  const char *name;
  const char *desc;
  void *val_ptr;    /* pointer to value storage */
  int val_type;     /* one of the VALTYPE_* values */
  int flags;
};

static struct LocalOption local_option_list[] =
{
  { "mode",     "Video mode (screen resolution)",
    NULL, VALTYPE_VID_MODE, 0 },

  { "depth",    "Video depth (color bits per pixel)",
    &sdlgl_depth, VALTYPE_INTEGER, 0 },

  { "windowed", "Runs in a window instead of fullscreen",
    &sdlgl_windowed, VALTYPE_BOOLEAN, 0 },

  { "gamma", "Gamma correction (-5 to +5, where 0 is normal)",
    &sdlgl_gamma, VALTYPE_INTEGER, 0 },

  /* ---- command-line only options ---- */

  { "tileheight", "Which tileset to use (16, 32 or 64)",
    &iflags.wc_tile_height, VALTYPE_INTEGER, LOPT_CMDLINE_ONLY },

  { "fontsize", "Size of text font (8, 14, 20 or 22)",
    &fontsize_override, VALTYPE_INTEGER, LOPT_CMDLINE_ONLY },

  /* ---- config-file only options ---- */

  { "defzoom",  "Default zoom (size of on-screen tiles)",
    &sdlgl_def_zoom, VALTYPE_INTEGER, LOPT_FILE_ONLY },

  { "keyrepeat",  "Key autorepeat (never, partial, or always)",
    &sdlgl_key_repeat, VALTYPE_KEY_REPEAT, LOPT_FILE_ONLY },

#if 0
  { "msgdim",  "Dim the message window rather than clear it",
    &sdlgl_msg_dim, VALTYPE_BOOLEAN, LOPT_FILE_ONLY },
#endif
    
  { "altprev",  "Alternate previous history mode (^P)",
    &sdlgl_alt_prev, VALTYPE_BOOLEAN, LOPT_FILE_ONLY },

  { "prevstep",  "Lines to step through previous history",
    &sdlgl_prev_step, VALTYPE_INTEGER, LOPT_FILE_ONLY },

  { "reformat",  "Reformat text windows that are too wide",
    &sdlgl_reformat, VALTYPE_BOOLEAN, LOPT_FILE_ONLY },

  { "shrinkwrap",  "Shrink the font when text window too big",
    &sdlgl_shrink_wrap, VALTYPE_BOOLEAN, LOPT_FILE_ONLY },

  { "flipping",  "Flip monster tiles to match their movement",
    &sdlgl_flipping, VALTYPE_BOOLEAN, LOPT_FILE_ONLY },

  { "invisfx",  "Draw invisible but seen monsters translucent",
    &sdlgl_invis_fx, VALTYPE_BOOLEAN, LOPT_FILE_ONLY },

  { "jumpscroll",  "Scroll the map window in large steps",
    &sdlgl_jump_scroll, VALTYPE_BOOLEAN, LOPT_FILE_ONLY },

  { "jailsize", "Percentage of screen which doesn't scroll",
    &sdlgl_jail_size, VALTYPE_INTEGER, LOPT_FILE_ONLY },

  /* ---- options kept for backwards compatibility ---- */

  { "tilesize", "Backwards Compatibility Only",
    &iflags.wc_tile_height, VALTYPE_INTEGER, LOPT_BACKW_COMPAT },

  { "textfont", "Backwards Compatibility Only",
    &iflags.wc_fontsiz_message, VALTYPE_INTEGER, LOPT_BACKW_COMPAT },
 
  { "mapfont", "Backwards Compatibility Only",
    &iflags.wc_fontsiz_map, VALTYPE_INTEGER, LOPT_BACKW_COMPAT },
 
  { "msglines",  "Backwards Compatibility Only",
    &iflags.wc_vary_msgcount, VALTYPE_INTEGER, LOPT_BACKW_COMPAT },

  { NULL, NULL, NULL, 0, 0 }
};

/* returns a valid index, or -1 if not found.  Sets `negated' to 1 if
 * found and the "no" prefix was used, otherwise it is unchanged.
 */
static int find_local_option(const char *word, int *negated)
{
  int i;
  const char *curname;

  for (i=0; (curname = local_option_list[i].name); i++)
  {
    if (strncmp(word, curname, strlen(curname)) == 0)
      return i;
  }

  /* handle the "no" prefix.
   */
  if (! (word[0] == 'n' && word[1] == 'o' && letter(word[2])))
    return -1;

  word += 2;
  
  for (i=0; (curname = local_option_list[i].name); i++)
  {
    if (strncmp(word, curname, strlen(curname)) == 0)
    {
      (*negated) = 1;
      return i;
    }
  }
 
  return -1;
}

static void parse_single_option(int optidx, const char *par,
    int from_file, int negated) 
{
  struct LocalOption *opt = local_option_list + optidx;
  int *intp;
  double *doubp;
  char tmp;

  assert(opt->name);

  if (par && strlen(par) == 0)
    par = NULL;

  if (! par && (opt->val_type != VALTYPE_BOOLEAN))
  {
    sdlgl_error("The %s%s option requires a parameter.\n",
        from_file ? "" : "--", opt->name);
  }
 
  /* strings not handled yet */
  assert(opt->val_type != VALTYPE_STRING);
 
  switch (opt->val_type)
  {
    case VALTYPE_VID_MODE:
      /*
       * the `mode' option must be handled specially.  We assume that
       * it's the only VALTYPE_VID_MODE option in the list.
       */
      assert(par);
      sscanf(par, "%d x %d", &sdlgl_width, &sdlgl_height);
      break;

    case VALTYPE_KEY_REPEAT:
      /*
       * the `keyrepeat' option is also a bit special, it takes the
       * values "never", "partial" and "always".  For backwards
       * compatibility, integer values from 0 to 2 are also accepted.
       */
      intp = (int *) opt->val_ptr;
      assert(intp);
      assert(par);

      tmp = tolower(*par);

      switch (tmp)
      {
        case '0': case 'n':  /* never */
          (*intp) = 0;
          break;

        case '1': case 'p':  /* partial */
          (*intp) = 1;
          break;

        case '2': case 'a':  /* always */
          (*intp) = 2;
          break;

        default:
          sdlgl_warning("Bad %s syntax: %s:%s.\n", 
              from_file ? "GL_OPTIONS" : SDLGL_ENV_VAR,
              opt->name, par);
          Sdlgl_wait_synch();
          break;
      }
      break;

    case VALTYPE_BOOLEAN:
      intp = (int *) opt->val_ptr;
      assert(intp);
      
      if (! par)
        *intp = ! negated;
      else if (par[0] == '0' || lowc(par[0]) == 'f')
        *intp = 0;
      else
        *intp = 1;
      break;

    case VALTYPE_INTEGER:
      intp = (int *) opt->val_ptr;
      assert(intp);

      assert(par);
      sscanf(par, "%d", intp);
      break;
    
    case VALTYPE_DOUBLE:
      doubp = (double *) opt->val_ptr;
      assert(doubp);

      assert(par);
      sscanf(par, "%lf", doubp);
      break;
    
    default:
      sdlgl_error("parse_single_option INTERNAL error on --%s.", 
          opt->name);
  }
}

static void show_help_message(void)
{
  fprintf(stderr,
#ifdef VANILLA_GLHACK
      "glHack " GLHACK_VER_STR " (C) 2002 Andrew Apted\n"
      "See the homepage at http://glhack.sourceforge.net/\n"
      "This is port of NetHack -- see http://www.nethack.org/\n"
      "\n"
      "Some standard NetHack options:\n"
#else
      "Slash'EM " VERSION_STRING " - SDL/GL window port\n"
      "See the homepage at http://slashem.sourceforge.net/\n"
      "\n"
      "Some standard Slash'EM options:\n"
#endif
      "    -u UserName     : name your character in the game.\n"
      "    -p Profession   : profession (role) of your character.\n"
      "    -r Race         : race of your character.\n"
      "    -@              : choose random character attributes.\n"
      "    -X              : enter non-scoring eXplore mode.\n"
      "    -s              : show list of high-scores and exit.\n"
      "\n"
      
#ifdef VANILLA_GLHACK
      "Some glHack specific options:\n"
#else
      "Some SDL/GL specific options:\n"
#endif
      "    --mode 640x480  : specify the video mode.\n"
      "    --depth 24      : the video depth (bits per pixel).\n"
      "    --windowed      : run in a window (instead of fullscreen).\n"
      "    --tileheight 16 : select tile height (16 or 32).\n"
      "    --fontsize 22   : choose text font (8, 14, 20 or 22).\n"
      "\n"
      "Please view the docs for more information, including\n"
      "the complete list of available options.\n"
  );

  sdlgl_error("");
  /* NOT REACHED */
}

static void show_version_message(void)
{
#ifdef VANILLA_GLHACK
  fprintf(stderr, "glHack version " GLHACK_VER_STR "\n");
#else
  fprintf(stderr, "Slash'EM version " VERSION_STRING
          " (SDL/GL window port)\n");
#endif

  sdlgl_error("");
  /* NOT REACHED */
}

/* this is called from src/files.c when parsing the config file.
 */
void Sdlgl_parse_options(char *opts, int initial, int from_file)
{
  char *par;
  int optidx;
  int negated = 0;

  /* note the clever trick here using recursion to split and handle
   * NUL-terminated option strings.  Copied from src/options.c.
   */
  if ((par = strchr(opts, ',')) != 0) 
  {
    *par++ = 0;
    Sdlgl_parse_options(par, initial, from_file);
  }

  /* strip leading and trailing white space */
  while (isspace(*opts)) opts++;
  par = eos(opts);
  while (--par >= opts && isspace(*par)) *par = 0;

  if (opts[0] == '!')
    opts++, negated = 1;

  optidx = find_local_option(opts, &negated);
  par = NULL;

  if (optidx >= 0 && 
      (local_option_list[optidx].flags & LOPT_CMDLINE_ONLY))
  {
    /* treat as though it wasn't found */
    optidx = -1;
  }

  if (optidx < 0)
  {
    /* same logic as badoption() in src/option.c */

    if (!initial)
    {
      pline("Bad GL_OPTIONS syntax: %s.", opts);
      return;
    }

    if (from_file)
      sdlgl_warning("Bad GL_OPTIONS syntax: %s.\n", opts);
    else
      sdlgl_warning("Bad " SDLGL_ENV_VAR " syntax: %s.\n", opts);

    Sdlgl_wait_synch();
    return;
  }

  if (local_option_list[optidx].val_type != VALTYPE_BOOLEAN)
  {
    /* if the option requires a parameter, find it after a `:'
     * character (NOT an `=' char like for command line args).
     */
    par = strchr(opts, ':');

    if (par) par++;
  }

  parse_single_option(optidx, par, from_file, negated);
}
 
void sdlgl_parse_cmdline_options(int *argcp, char **argv)
{
  int num, opt;
  int negated;

  char **dest_argv;
  const char *word, *par;
  
  /* skip the program name */
  argv++;
  dest_argv = argv;
  num = (*argcp) - 1;

  while (num > 0)
  {
    num--;

    /* we require options to start with a double dash, not only to be
     * all GNU-ish (which is nice), but so we don't clash with the
     * single dash / single letter options in unixmain.c (etc).
     */
    if (argv[0][0] != '-' || argv[0][1] != '-')
    {
      /* not an option */
      *dest_argv++ = *argv++;
      continue;
    }

    word = argv[0] + 2;

    if (strcmp(word, "help") == 0)
    {
      /* this exits the program */
      show_help_message();
    }
     
    if (strcmp(word, "version") == 0)
    {
      /* this exits the program */
      show_version_message();
    }
     
    negated = 0;
    opt = find_local_option(word, &negated);
    par = NULL;

    if (opt >= 0 && (local_option_list[opt].flags & LOPT_BACKW_COMPAT))
    {
      /* backwards compatibility option */
      sdlgl_warning("The --%s option has been replaced or removed.\n",
          local_option_list[opt].name);
      sdlgl_warning("Please see the docs for more information.\n");
      Sdlgl_wait_synch();

      *dest_argv++ = *argv++;
      continue;
    }

    if (opt >= 0 && (local_option_list[opt].flags & LOPT_FILE_ONLY))
    {
      /* treat as though it wasn't found */
      opt = -1;
    }

    if (opt < 0)
    {
      /* unrecognised option */
      *dest_argv++ = *argv++;
      continue;
    }

    /* swallow arg string */
    argv++;

    if (local_option_list[opt].val_type != VALTYPE_BOOLEAN)
    {
      /* the following options require a parameter.  Find it either in
       * the same argument string (after an `=' char), or as the next
       * argument.
       */
      par = strchr(word, '=');

      if (par)
        par++;
      else if (num > 0 && argv[0][0] != '-')
      {
        par = argv[0];

        /* swallow the parameter */
        num--;
        argv++;
      }
    }

    parse_single_option(opt, par, 0 /* from_file */, negated);
  }

  assert(dest_argv <= argv);
  dest_argv[0] = NULL;

  /* compute number of deleted argument strings */
  (*argcp) -= (argv - dest_argv);
}


#undef  SET_WARN
#define SET_WARN(str,var,defval)  \
    sdlgl_warning(str " %d not supported. Using default (%d).\n",  \
        var, (defval));  \
    Sdlgl_wait_synch();  \
    var = (defval);

#undef  RANGE_CHK
#define RANGE_CHK(str,var,mini,maxi)  \
    if (var < (mini))  \
    {  \
      sdlgl_warning(str " %d too low. Using minimum (%d).\n",  \
          var, (mini));  \
      Sdlgl_wait_synch();  \
      var = (mini);  \
    }  \
    else if (var > (maxi))  \
    {  \
      sdlgl_warning(str " %d too high. Using maximum (%d).\n",  \
          var, (maxi));  \
      Sdlgl_wait_synch();  \
      var = (maxi);  \
    }

void sdlgl_validate_wincap_options(void)
{
  set_wc_option_mod_status(WC_SCROLL_MARGIN, SET_IN_GAME);
 
  if (iflags.wc_align_message == 0)
  {
    iflags.wc_align_message = ALIGN_TOP;
  }

  if (iflags.wc_align_status == 0)
  {
    iflags.wc_align_status = ALIGN_BOTTOM;
  }

  if (iflags.wc_scroll_margin <= 0)
  {
    iflags.wc_scroll_margin = DEF_SCROLL_MARGIN;
  }

  if (iflags.wc_tile_height <= 0)
  {
    iflags.wc_tile_height = DEF_TILE_HEIGHT;
  }
  else if (iflags.wc_tile_height != 16 && iflags.wc_tile_height != 32 
           && iflags.wc_tile_height != 64
           )
  {
    SET_WARN("Tile height", iflags.wc_tile_height, DEF_TILE_HEIGHT);
  }

  /* set the tileset name if not already set.
   */
#ifndef VANILLA_GLHACK
  if (tileset[0] == 0)
  {
    if (iflags.wc_tile_height == 16)
      strcpy(tileset, "Small tiles");
    else
      strcpy(tileset, "Big tiles");
  }
#endif

  if (iflags.wc_vary_msgcount <= 0)
  {
    iflags.wc_vary_msgcount = DEF_VARY_MSGCOUNT;
  }
  else
  {
    RANGE_CHK("Message count", iflags.wc_vary_msgcount,
        MIN_MESSAGE, MAX_MESSAGE);
  }

  /* ---- handle font sizes ---- */

#undef  FONT_CHK
#define FONT_CHK(str,var,defval) \
    if (var <= 0)  \
      var = (defval);  \
    else if (var != sdlgl_quantize_font(var))  \
    {  \
      SET_WARN(str,var,defval);  \
    }

  FONT_CHK("Map font size", iflags.wc_fontsiz_map, DEF_FONTSIZ_MAP);

  if (fontsize_override > 0)
  {
    FONT_CHK("Font size", fontsize_override, DEF_FONTSIZ_TEXT);

    iflags.wc_fontsiz_message = fontsize_override;
    iflags.wc_fontsiz_status  = fontsize_override;
    iflags.wc_fontsiz_menu    = fontsize_override;
    iflags.wc_fontsiz_text    = fontsize_override;
  }
  else
  {
    FONT_CHK("Message font size", iflags.wc_fontsiz_message,
         DEF_FONTSIZ_TEXT);

    FONT_CHK("Status font size", iflags.wc_fontsiz_status, 
         iflags.wc_fontsiz_message);
    FONT_CHK("Menu font size", iflags.wc_fontsiz_menu, 
         iflags.wc_fontsiz_message);
    FONT_CHK("Text font size", iflags.wc_fontsiz_text, 
         iflags.wc_fontsiz_menu);
  }
}

void sdlgl_validate_gl_options(void)
{
  if (sdlgl_width <= 0 || sdlgl_height <= 0)
  {
    sdlgl_width = sdlgl_height = 0;  /* autodetect */
  }
  else
  {
    if (sdlgl_width  < MIN_SDLGL_WIDTH || 
        sdlgl_height < MIN_SDLGL_HEIGHT)
    {
      sdlgl_warning("Video mode %dx%d too small. Using %dx%d.\n",
          sdlgl_width, sdlgl_height, MIN_SDLGL_WIDTH, MIN_SDLGL_HEIGHT);

      sdlgl_width  = MIN_SDLGL_WIDTH;
      sdlgl_height = MIN_SDLGL_HEIGHT;
    }
    else if (sdlgl_width  > MAX_SDLGL_WIDTH || 
             sdlgl_height > MAX_SDLGL_HEIGHT)
    {
      sdlgl_warning("Video mode %dx%d too large. Using %dx%d.\n",
          sdlgl_width, sdlgl_height, MAX_SDLGL_WIDTH, MAX_SDLGL_HEIGHT);

      sdlgl_width  = MAX_SDLGL_WIDTH;
      sdlgl_height = MAX_SDLGL_HEIGHT;
    }
  }

  if (sdlgl_depth <= 0)
  {
    sdlgl_depth = 0;  /* autodetect */
  }
  else switch (sdlgl_depth)
  {
    case 8: case 15: case 16: case 24: case 32:
      break;

    default:
      SET_WARN("Video depth", sdlgl_depth, DEF_SDLGL_DEPTH);
      break;
  }

  RANGE_CHK("Gamma", sdlgl_gamma, -5, +5);

  if (sdlgl_def_zoom <= 0)
  {
    sdlgl_def_zoom = iflags.wc_tile_height;
  }
  else if (sdlgl_def_zoom <= 8)
  {
    sdlgl_def_zoom = TEXT_ZOOM;
  }

  sdlgl_def_zoom = sdlgl_quantize_zoom(sdlgl_def_zoom);

  if (sdlgl_jail_size <= 0)
  {
    sdlgl_jail_size = DEF_SDLGL_JAILSIZE;
  }
  else
  {
    RANGE_CHK("Jail size", sdlgl_jail_size, 1, 100);
  }

  if (sdlgl_prev_step <= 0)
  {
    sdlgl_prev_step = sdlgl_alt_prev ? iflags.wc_vary_msgcount : 
        DEF_SDLGL_PREVSTEP;
  }
  else if (sdlgl_alt_prev)
  {
    sdlgl_prev_step = min(sdlgl_prev_step, iflags.wc_vary_msgcount);
  }
  else if (sdlgl_prev_step > MAX_SDLGL_PREVSTEP)
  {
    sdlgl_prev_step = MAX_SDLGL_PREVSTEP;
  }
}

void Sdlgl_preference_update(const char *pref)
{
  if (strcmpi(pref, "scroll_margin") == 0)
  {
    Sdlgl_cliparound(u.ux, u.uy);
    sdlgl_flush();
    return;
  }
}


#endif /* GL_GRAPHICS */
/*gl_opt.c*/
