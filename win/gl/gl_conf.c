/* Copyright (C) 2002 Andrew Apted <ajapted@users.sourceforge.net> */
/* NetHack may be freely redistributed.  See license for details.  */

/*
 * SDL/GL window port for NetHack & Slash'EM.
 *
 * Handles the new in-game option screens, including support for
 * saving back options to the config file.
 */

#include "hack.h"
#include "dlb.h"
#include "patchlevel.h"

#if defined(GL_GRAPHICS) || defined(SDL_GRAPHICS)
#if defined(VANILLA_GLHACK)

#define WINGL_INTERNAL
#include "winGL.h"

#include <string.h>
#include <ctype.h>
#include <errno.h>


#define DEPTH_OPT_TITL  41
#define DEPTH_OPT_KEYS  42
#define DEPTH_OPT_MAIN  43

static struct TileWindow *opt_titl;
static struct TileWindow *opt_main;
static struct TileWindow *opt_keys;

/* from src/files.c */
extern const char *NDECL(get_config_write_filename);


enum OptLineFlag
{
  /* whether it is an option, otherwise comments (etc) */
  OFLG_Option = 0x0001,
   
  /* the option is SDL/GL specific (otherwise standard) */
  OFLG_GL = 0x0002,
   
  /* boolean option.  When true, the value string will be either "0"
   * or "1".
   */
  OFLG_Boolean = 0x0010,

  /* the option was on same line as the previous one */
  OFLG_SameLine = 0x0020,

  /* the line used `:' instead of `=' after the "OPTIONS" or
   * "GL_OPTIONS" keyword.
   */
  OFLG_HadColon = 0x0040,
  
  /* the option has been written to the config file */
  OFLG_Written = 0x0100
};

struct OptionLine
{
  /* link in list */
  struct OptionLine *next;

  int flags;

  /* name and value.  Name can be NULL only for blank (non-option)
   * lines.  Value is NULL for all non-option lines, and for options
   * that have an empty value.
   */
  char *name;
  char *value;
};

static struct OptionLine *changed_option_list = NULL;


static void add_opt(struct OptionLine **list,
    struct OptionLine *pred, const char *name, const char *value, 
    int flags)
{
  struct OptionLine *line = (struct OptionLine *)
      alloc(sizeof(struct OptionLine));

  memset(line, 0, sizeof(*line));

  line->flags = flags;
  line->name  = name  ? strdup(name)  : NULL;
  line->value = value ? strdup(value) : NULL;

  /* find tail node when pred is NULL */
  if (! pred)
  {
    pred = (*list);

    while (pred && pred->next)
      pred = pred->next;
  }

  if (pred)
  {
    line->next = pred->next;
    pred->next = line;
  }
  else
  {
    line->next = (*list);
    (*list) = line;
  }
}

static void free_opts(struct OptionLine **list)
{
  assert(list);

  while (*list)
  {
    struct OptionLine *cur = (*list);
    (*list) = cur->next;

    if (cur->name)
      free(cur->name);
    
    if (cur->value)
      free(cur->value);
    
    free(cur);
  }
}

static struct OptionLine *find_changed_option(int flags, const char *name)
{
  struct OptionLine *cur;

  for (cur = changed_option_list; cur; cur = cur->next)
  {
    if (! (cur->flags & OFLG_Option))
      continue;

    if ((cur->flags & OFLG_GL) != (flags & OFLG_GL))
      continue;
    
    /* !!! FIXME: should this be case insensitive ?? */
    if (strcmp(cur->name, name) == 0)
      return cur;

    /* handle the case where the `name' (from the config file) starts with
     * "no" -- for booleans this is allowed.
     */
    if (! (cur->flags & OFLG_Boolean))
      continue;

    if (! (name[0] == 'n' && name[1] == 'o' && letter(name[2])))
      continue;
    
    if (strcmp(cur->name, name + 2) == 0)
      return cur;
  }

  return NULL;
}


/* ---------------------------------------------------------------- */

static struct OptionLine *config_line_list = NULL;
static int config_is_new = 0;

#define match_varname(INP,NAM,LEN)  match_optname(INP,NAM,LEN,TRUE)


void bbb_parse_single_option(char *opts, int flags)
{
  char *par;
  short is_bool = 0;
  short negated = 0;

  /* strip leading and trailing white space */
  while (isspace(*opts)) opts++;
  par = eos(opts);
  while (--par >= opts && isspace(*par)) *par = 0;

  if (opts[0] == '!')
    opts++, negated = 1;

  /* look for a parameter after `:' char.  When absent, we assume the
   * option was a boolean.  Note that the `:' may be present but have
   * nothing after it -- this means an empty parameter.
   */
  par = strchr(opts, ':');

  if (par)
    *par++ = 0;
  else
    is_bool = 1;

  if (is_bool || negated)
  {
    add_opt(&config_line_list, NULL, opts, negated ? "0" : "1",
        OFLG_Option | OFLG_Boolean | flags);
  }
  else
  {
    add_opt(&config_line_list, NULL, opts, par, OFLG_Option | flags);
  }
}
 
void bbb_parse_options(char *opts, int flags)
{
  while (opts)
  {
    char *par = strchr(opts, ',');

    if (par)
      *par++ = 0;
  
    bbb_parse_single_option(opts, flags);

    opts = par;
  }
}
 
static int bbb_parse_config_line(char *buf)
{
  char *bufp, *altp;

  int flags = 0;

  if (*buf == '#')
  {
    add_opt(&config_line_list, NULL, buf, NULL, 0);
    return 1;
  }

  /* remove trailing whitespace */
  bufp = eos(buf);
  while (--bufp > buf && isspace(*bufp))
  { /* nothing here */ }

  if (bufp <= buf)
  {
    /* skip blank lines */
    buf[0] = 0;
    add_opt(&config_line_list, NULL, buf, NULL, 0);
    return 1;
  }

  *(bufp + 1) = 0;

  /* find the '=' or ':' */

  bufp = index(buf, '=');
  altp = index(buf, ':');

  if (!bufp || (altp && altp < bufp))
  {
    bufp = altp;
    flags |= OFLG_HadColon;
  }

  /* bad line ? */
  if (!bufp) 
    return 0;

  /* skip whitespace between '=' and value */
  for (bufp++; isspace(*bufp); bufp++)
  { /* nothing here */ }

  if (match_varname(buf, "OPTIONS", 4)) 
  {
    bbb_parse_options(bufp, flags);
    return 1;
  } 

  if (match_varname(buf, "GL_OPTIONS", 10))
  {
    bbb_parse_options(bufp, OFLG_GL | flags);
    return 1;
  }

  /* keep other lines intact */
  add_opt(&config_line_list, NULL, buf, NULL, 0);
  return 1;
}

static int read_config_lines(const char *conf_name)
{
  FILE *fp = fopen(conf_name, "r");

  char buf[4 * BUFSZ];

  if (! fp)
  {
    /* allow it to be missing (new user) */
    if (errno == ENOENT)
    {
      config_is_new = 1;
      return 0;
    }

    pline("Error trying to read the config file!");
    return -1;
  }

  while (fgets(buf, sizeof(buf)-1, fp))
  {
    if (! bbb_parse_config_line(buf))
      sdlgl_warning("Bad option line: \"%s\"\n", buf);
  }

  fclose(fp);

  /* OK */
  return 0;
}

static int merge_config_lines(void)
{
  static const char *default_header[] =
  {
    "#",
#ifdef VANILLA_GLHACK
    "# glHack config file.",
#else
    "# Slash'EM config file.",
#endif
    "#",
    "",
    NULL
  };

  struct OptionLine *cur;
  struct OptionLine *opt;
  
  struct OptionLine *last_norm = NULL;
  struct OptionLine *last_gl   = NULL;

  int i;

  for (cur = config_line_list; cur; cur = cur->next)
  {
    if (! (cur->flags & OFLG_Option))
      continue;

    if (cur->flags & OFLG_GL)
      last_gl = cur;
    else
      last_norm = cur;

    opt = find_changed_option(cur->flags, cur->name); 
    
    if (! opt)
      continue;

    /* Note that we copy the _name_ as well.  We do this because
     * booleans that were prefixed with "no" in the config file need
     * to have the "no" removed.
     */
    free(cur->name);
    free(cur->value);

    cur->name  = strdup(opt->name);
    cur->value = strdup(opt->value);
    
    if (opt->flags & OFLG_Written)
      sdlgl_warning("The %soption `%s' occurs twice in config file.\n",
          (opt->flags & OFLG_GL) ? "gl_" : "", opt->name);
     
    opt->flags |= OFLG_Written;
  }

  /* handle left-over options.  If the config file was empty (or is
   * new), add some comments to the top.
   */
  if (config_line_list == NULL)
  {
    for (i=0; default_header[i]; i++)
      add_opt(&config_line_list, NULL, default_header[i], NULL, 0);
  }

  /* do two passes, to keep all the normal and gl options separate.
   */
  for (i = 0; i < 2; i++)
  {
    for (opt = changed_option_list; opt; opt = opt->next)
    {
      if ((opt->flags & OFLG_GL) != (i ? OFLG_GL : 0))
        continue;

      if (opt->flags & OFLG_Written)
        continue;

      add_opt(&config_line_list, i ? last_gl : last_norm, 
          opt->name, opt->value, opt->flags);
          
      opt->flags |= OFLG_Written;
    }
  }

  /* OK */
  return 0;
}

static int write_config_lines(const char *conf_name)
{
  struct OptionLine *opt;

  FILE *fp;

  int within_line;
  int prev_flags;

  fp = fopen(conf_name, "w");

  if (! fp)
  {
    pline("Error trying to write the config file!");
    return -1;
  }

  /* Invariant: when `within_line' is true, the file position is
   * currently within an option line (needing either a "," separator
   * or a "\n" to start a fresh line).
   */
  within_line = 0;
  prev_flags = 0;
  
  for (opt = config_line_list; opt; opt = opt->next)
  {
    if (! (opt->flags & OFLG_Option))
    {
      if (within_line)
      {
        fprintf(fp, "\n");
        within_line = 0;
      }

      fprintf(fp, "%s\n", opt->name ? opt->name : "");
      continue;
    }

    if (within_line && (prev_flags & OFLG_GL) != (opt->flags & OFLG_GL))
    {
      fprintf(fp, "\n");
      within_line = 0;
    }

    if (within_line)
      fprintf(fp, ",");
    else
      fprintf(fp, "%sOPTIONS%c", (opt->flags & OFLG_GL) ? "GL_" : "",
          (opt->flags & OFLG_HadColon) ? ':' : '=');

    /* handle booleans */
    if ((opt->flags & OFLG_Boolean) && opt->value[0] == '0')
      fprintf(fp, "!");

    assert(opt->name);
    fprintf(fp, opt->name);

    if (! (opt->flags & OFLG_Boolean))
      fprintf(fp, ":%s", opt->value ? opt->value : "");

    within_line = 1;
    prev_flags = opt->flags;
  }

  if (within_line)
    fprintf(fp, "\n");

  fclose(fp);

  /* OK */
  return 0;
}

void sdlgl_update_config_file(const char *conf_name)
{
  config_is_new = 0;

  if (read_config_lines(conf_name) == 0 &&
      merge_config_lines() == 0 &&
      write_config_lines(conf_name) == 0)
  {
    if (config_is_new)
      pline("Successfully wrote new config file.");
    else
      pline("Successfully updated the config file.");
  }

  free_opts(&config_line_list);
}
 

/* ---------------------------------------------------------------- */

static void create_opt_titl(int what)
{
  const char *str = NULL;
  int width;

  switch (what)
  {
    case 1: str = "NetHack Options"; break;
    case 2: str = "SDL/GL Options"; break;
    case 3: str = "Miscellaneous Info"; break;
    default: return;
  }

  width = strlen(str);

  opt_titl = sdlgl_new_tilewin(sdlgl_font_text, width + 2, 1, 1,0);

  sdlgl_store_str(opt_titl, 1, 0, str, 99, WHITE);

  sdlgl_map_tilewin(opt_titl, 
      sdlgl_width - (width + 2) * opt_titl->scale_w,
      sdlgl_height - opt_titl->scale_h, 
      (width + 2) * opt_titl->scale_w, 
      opt_titl->scale_h, 
      DEPTH_OPT_TITL);
}

static void create_opt_keys(void)
{
  static int w = 50;
  static int h = 4;
  
  static const char *key_lines[] =
  {
    "Use cursor keys or letters to select an option",
    "Use Enter/Return to prompt for a new value",
    "Use `+' and `-' to change booleans and integers",
    "Use Escape key to exit"
  };

  int i;
  
  opt_keys = sdlgl_new_tilewin(sdlgl_font_menu, w, h, 1,0);
  opt_keys->background = MENU_NONE_COL;  /* PREV_BACK_COL */

  for (i=0; i < h; i++)
    sdlgl_store_str(opt_keys, 1, i, key_lines[h-1-i], 99, L_GREY);

  sdlgl_map_tilewin(opt_keys, 
      sdlgl_width - w * opt_keys->scale_w, 0, 
      w * opt_keys->scale_w, 
      h * opt_keys->scale_h, DEPTH_OPT_KEYS);
}

static void create_opt_main(void)
{
  int w = 60;
  int h = 15;
  
  opt_main = sdlgl_new_tilewin(sdlgl_font_menu, w, h, 1,0);
  opt_main->background = TEXT_BACK_COL;

  sdlgl_map_tilewin(opt_main, 
      sdlgl_width - w * opt_main->scale_w, opt_keys->scr_h + 20, 
      w * opt_main->scale_w, h * opt_main->scale_h, 
      DEPTH_OPT_MAIN);
}

static void destroy_opt_titl(void)
{
  sdlgl_unmap_tilewin(opt_titl);
  sdlgl_free_tilewin(opt_titl);
  opt_titl = NULL;
}

static void destroy_opt_keys(void)
{
  sdlgl_unmap_tilewin(opt_keys);
  sdlgl_free_tilewin(opt_keys);
  opt_keys = NULL;
}

static void destroy_opt_main(void)
{
  sdlgl_unmap_tilewin(opt_main);
  sdlgl_free_tilewin(opt_main);
  opt_main = NULL;
}

static void test_opt(int what)
{
  create_opt_titl(what);
  create_opt_keys();
  create_opt_main();

  sdlgl_set_start_depth(DEPTH_OPT_TITL);
  sdlgl_flush();

  /* ..... */

  sdlgl_get_key(0);

  destroy_opt_titl();
  destroy_opt_keys();
  destroy_opt_main();
 
  sdlgl_set_start_depth(0);
  sdlgl_flush();
}

static int handle_option_main_menu(void)
{
  static const char *items[] = 
  {
    /* 1 */ "Nethack options.",
    /* 2 */ "SDL/GL options.",
    /* 3 */ "Miscellaneous info.",
    NULL
  };

  int i, n;
  menu_item *selected = NULL;
  anything any;

  winid tmpwin = create_nhwindow(NHW_MENU);

  any.a_void = 0;    /* zero all bits */
  start_menu(tmpwin);

  for (i=0; items[i]; i++)
  {
    any.a_int = i+1;
    add_menu(tmpwin, NO_GLYPH, &any, 0, 0,
        ATR_NONE, items[i], MENU_UNSELECTED);
  }

  end_menu(tmpwin, "Option Main Menu");
  n = select_menu(tmpwin, PICK_ONE, &selected);
  destroy_nhwindow(tmpwin);

  if (n <= 0)
    return 0;

  assert(selected);

  n = selected[0].item.a_int;
  free((genericptr_t)selected);

  return n;
}

/* called from src/options.c 
 */
int Sdlgl_doset()
{
  int what;
  const char *conf_name = get_config_write_filename();
 
  what = handle_option_main_menu();

  if (what == 0)
    return 0;

  switch (what)
  {
    case 1:
      /* !!! NetHack options */
      test_opt(1);
      break;
    
    case 2:
      /* !!! SDL/GL options */
      test_opt(2);
      break;
    
    case 3:
      /* !!! View miscellaneous */
      test_opt(3);
      break;
  }

/*   clear_nhwindow(WIN_MESSAGE);   */

  if (conf_name && changed_option_list)
  {
    what = yn("Write changes to config file?");
    clear_nhwindow(WIN_MESSAGE);

#if 0  /* !!! */
    if (what == 'y')
      sdlgl_update_config_file(conf_name);
#endif
  }
  
  free_opts(&changed_option_list);
  return 0;
}

#endif /* VANILLA_GLHACK */
#endif /* GL_GRAPHICS */
/*gl_conf.c*/
