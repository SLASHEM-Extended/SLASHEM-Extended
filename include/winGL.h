/* Copyright (C) 2002 Andrew Apted <ajapted@users.sourceforge.net> */
/* NetHack may be freely redistributed.  See license for details.  */

/*
 * SDL/GL window port for NetHack & Slash'EM.
 *
 * This file contains the common definitions.
 */

#ifndef WINGL_H
#define WINGL_H

#ifdef GL_GRAPHICS
extern struct window_procs sdlgl_hardw_procs;
#endif

#ifdef SDL_GRAPHICS
extern struct window_procs sdlgl_softw_procs;
#endif

extern void FDECL(Sdlgl_parse_options, (char *, int, int));

#ifdef VANILLA_GLHACK
#define GLHACK_VER_HEX  0x110
#define GLHACK_VER_STR  "1.1"
#define SDLGL_PROGRAM  "glHack"
#define SDLGL_ICON     "glHack"
#define SDLGL_ENV_VAR  "GLHACKOPTIONS"
#else
#define SDLGL_PROGRAM  "Slash'EM SDL/GL"
#define SDLGL_ICON     "Slash'EM"
#define SDLGL_ENV_VAR  "SLASHEMGLOPTIONS"
#endif


/* ------------ internal definitions from here on ---------- */

#ifdef WINGL_INTERNAL

/* sanity check the defines */
#ifndef USE_TILES
#error USE_TILES not defined
#endif
#ifndef CLIPPING
#error CLIPPING not defined
#endif
#ifdef CHANGE_COLOR
#error CHANGE_COLOR defined
#endif
#ifdef OVERLAY
#error OVERLAY defined
#endif
#ifdef MICRO
#error MICRO defined
#endif

/* N.H uses compress/uncompress, conflicting with ZLIB headers */
#undef compress
#undef uncompress

/* certain N.H #defines conflict with the SDL header files */
#undef red
#undef green
#undef blue

/* From tile.c */
extern short glyph2tile[];

/* extra includes we need */

#include <SDL/SDL.h>
#include <png.h>
#include <assert.h>

#ifdef GL_GRAPHICS
#include <GL/gl.h>
#else
typedef unsigned int GLuint;
#endif

#define E extern

/* inlining */
#ifdef GH_INLINE 
/* keep current definition (from Makefile) */
#elif defined(__GNUC__)
#define GH_INLINE  __inline__
#elif defined(_MSC_VER)
#define GH_INLINE  __inline
#else
#define GH_INLINE  /* nothing */
#endif

/* character munging macros for CTRL and ALT/META */
#ifndef C
#define C(c)   (0x1f & (c))
#endif
#ifndef M
#define M(c)   (0x80 | (c))
#endif


/*
 *  GL_MAIN
 */

extern int sdlgl_initialized;
extern int sdlgl_software;

extern SDL_Surface *sdlgl_surf;

struct MenuItem
{
  /* link in list */
  struct MenuItem *next;
  
  /* stuff given by add_menu().  The string is copied */
  int glyph;
  anything identifier;
  char accelerator;
  char groupacc;
  int attr;
  char *str;
  
  /* whether or not this item is selected.  When 0, the `count' field
   * will be ignored.
   */
  int selected;
  
  /* count given by user.  -1 when no count given */
  int count;
};

struct TextItem
{
  /* link in list */
  struct TextItem *next;
  
  /* stuff given by putstr().  The string is copied */
  int attr;
  char *str;

  /* alignment position (used for reformatting) */
  int align;
};

#define POSKEY_ALLOW_REPEAT   0x0001
#define POSKEY_DO_SCROLLBACK  0x0002

/* Note: routines that are in the interface table (sdlgl_procs) begin
 * with a capital `S'.  Everything else has a lowercase `s'.
 */
E void FDECL(Sdlgl_init_nhwindows, (int *, char **));
E void NDECL(Sdlgl_get_nh_event) ;
E void FDECL(Sdlgl_exit_nhwindows, (const char *));
E void FDECL(Sdlgl_suspend_nhwindows, (const char *));
E void NDECL(Sdlgl_resume_nhwindows);
E void NDECL(Sdlgl_update_inventory);
E void NDECL(Sdlgl_mark_synch);
E void NDECL(Sdlgl_wait_synch);
E void FDECL(Sdlgl_raw_print, (const char *));
E void FDECL(Sdlgl_raw_print_bold, (const char *));
E int NDECL(Sdlgl_nhgetch);
E int FDECL(Sdlgl_nh_poskey, (int *, int *, int *));
E void NDECL(Sdlgl_nhbell);
E void FDECL(Sdlgl_number_pad, (int));
E void NDECL(Sdlgl_delay_output);

E void VDECL(sdlgl_warning, (const char *, ...)) PRINTF_F(1,2);
E void VDECL(sdlgl_error, (const char *, ...)) PRINTF_F(1,2);
E void VDECL(sdlgl_hangup, (const char *, ...)) PRINTF_F(1,2);

E void FDECL(sdlgl_sleep, (int));
E int NDECL(sdlgl_get_time);
E int FDECL(sdlgl_get_poskey, (int, int *, int *, int *));
E int FDECL(sdlgl_get_key, (int));


/*
 * GL_OPT
 */

#define DEF_SDLGL_WIDTH   800
#define DEF_SDLGL_HEIGHT  600
#define DEF_SDLGL_DEPTH   16

#define MIN_SDLGL_WIDTH   640
#define MIN_SDLGL_HEIGHT  400
#define MAX_SDLGL_WIDTH   2048
#define MAX_SDLGL_HEIGHT  1536

extern int sdlgl_width;
extern int sdlgl_height;
extern int sdlgl_depth;

#define DEF_SDLGL_WINDOWED   0
#define DEF_SDLGL_JAILSIZE   35
#define DEF_SDLGL_KEYREPEAT  2  /* always */

#define DEF_SDLGL_PREVSTEP   1
#define MAX_SDLGL_PREVSTEP   20

#define DEF_TILE_HEIGHT      32
#define DEF_FONTSIZ_MAP      14
#define DEF_FONTSIZ_TEXT     20
#define DEF_VARY_MSGCOUNT    3
#define DEF_SCROLL_MARGIN    3

/* sdlgl_def_zoom can take on this value (text-mode view) */
#define TEXT_ZOOM  -1

extern int sdlgl_windowed;
extern int sdlgl_jail_size;
extern int sdlgl_def_zoom;
extern int sdlgl_key_repeat;
extern int sdlgl_alt_prev;
extern int sdlgl_prev_step;
extern int sdlgl_msg_dim;
extern int sdlgl_reformat;
extern int sdlgl_shrink_wrap;
extern int sdlgl_flipping;
extern int sdlgl_jump_scroll;
extern int sdlgl_invis_fx;
extern int sdlgl_gamma;

E void FDECL(sdlgl_parse_cmdline_options, (int *, char **));
E void NDECL(sdlgl_validate_wincap_options);
E void NDECL(sdlgl_validate_gl_options);
E void FDECL(Sdlgl_preference_update, (const char *));


/*
 * GL_CONF
 */

/* (nothing here yet) */


/*
 * GL_ROLE
 */

E void NDECL(Sdlgl_player_selection);


/*
 * GL_UNIT
 */

extern int sdlgl_tex_max;

typedef unsigned int rgbcol_t;

#define RGB_MAKE(r,g,b)  (((r) << 16) + ((g) << 8) + (b))
 
#define RGB_RED(rgb)  ((rgb >> 16) & 0xFF)
#define RGB_GRN(rgb)  ((rgb >>  8) & 0xFF)
#define RGB_BLU(rgb)  ((rgb      ) & 0xFF)

struct GraphicUnit
{
  /* texture used */
  GLuint tex_id;

  /* texture coords */
  float tx1, ty1, tx2, ty2;
    
  /* screen coordinates */
  short x, y, w, h;

  /* rendering pass (lower ones are drawn first) */
  short pass;

  /* image color */
  rgbcol_t color;

  /* translucency, range is 0.0 - 1.0 */
  float trans;
};

E void NDECL(sdlgl_unit_startup);
E void NDECL(sdlgl_unit_shutdown);

E void FDECL(sdlgl_begin_units, (int, int));
E void FDECL(sdlgl_add_unit, (GLuint,float,float,float,float,
      short,short,short,short,short,rgbcol_t,float));
E void NDECL(sdlgl_finish_units);

E void FDECL(sdlgl_hw_make_screenshot, (const char *));


/*
 * GL_TILE
 */

typedef unsigned short tileidx_t;
typedef unsigned short tilecol_t;
typedef unsigned short tileflags_t;

#define TILE_EMPTY  0x7FFF

#define TILE_F_FLIPX    0x0001  /* horizontally flip the fg */
#define TILE_F_TRANS50  0x0010  /* draw fg 50% translucent */

struct TilePair
{
  /* What tiles to draw at each location.  Any of these can be
   * TILE_EMPTY.  BG contains the actual background (usually a floor,
   * maybe water).  MG is for dungeon parts (walls, stairs, sinks,
   * traps, and so forth).  FG contains everything else: objects,
   * monsters, zaps and swallows.
   */
 
  tileidx_t fg;
  tileidx_t mg;  /* mid-ground */

  union
  {
    tilecol_t col;
    tileidx_t bg;
  }
  u;

  tileflags_t flags;
};

struct FontCache;

struct TileSet
{
  /* total number of tiles in the set */
  int tile_num;

  /* size of each tile */
  int tile_w, tile_h;

  /* overlap size.  Only non-zero for the pseudo 3D tileset.  lap_skew
   * is the difference (of horizontal pixels) between to rows.
   */
  int lap_w, lap_h;
  int lap_skew;

  /* number of tiles horizontal/vertical that fit in each 256x256
   * image (or whatever size the 3D card supports).  Tiles are packed
   * from left->right, bottom->top.  For software mode, the tiles are
   * all packed in a single surface.
   */
  int pack_w, pack_h;

  /* number of 256x256 images */
  int tex_num;

  /* the GL texture ids.  Unused in software mode */
  GLuint *tex_ids;

  /* for GL, the size of all textures in the array.  Usually these
   * will be the same as sdlgl_tex_max, but when sdlgl_tex_max is high
   * (like 1024) and the tileset is small (like the fonts), then the
   * size can be reduced to save memory.
   */
  int tex_size_w, tex_size_h;

  /* which tiles have alpha parts.  Valid for both GL and software
   * rendering.
   */
  unsigned char *has_alpha;

  /* for Software mode, this surface contains the tiles.  When using
   * large tilesets (32x32 and 48x64), surf_small contains the
   * shrunk-down version (otherwise it is NULL).
   */
  SDL_Surface *surf;
  SDL_Surface *surf_small;

  /* when using the Isometric tileset in software mode, these contain
   * some extra tiles to draw the map borders.  (Needed since the
   * dirty matrix code doesn't support drawing diagonal lines).
   */
  SDL_Surface *borders;
  SDL_Surface *borders_small;

  /* fonts are normally bright white.  For software mode, we need to
   * colorise them, and this is done by the font cache.  This is
   * unused in GL mode.
   */
  struct FontCache *font_cache;
};

struct ExtraShape
{
  /* type of extra shape */
  enum ShapeType
  {
    SHAPE_None = 0,
    SHAPE_Heart,
    SHAPE_Ridden
  }
  type;
 
  /* position of shape, in terms of whole tiles */
  short x, y;
  
  /* type-dependent parameters.
   * For SHAPE_Heart and SHAPE_Ridden, par1 means "on_left".
   */
  int par1, par2;
};

struct TileWindow
{
  /* types of tiles drawn in this window */
  struct TileSet *set;

  /* width and height of this window (in tiles).  Can be larger than
   * the screen.
   */
  int total_w, total_h;

  /* on-screen position of window (in pixels).  All values are 0 when
   * the window is unmapped.  Mapped windows are drawn in order of
   * their depth value (lower depths are drawn first).
   */
  int scr_x, scr_y, scr_w, scr_h, scr_depth;

  /* private field -- index of window in mapped array */
  int mapped_idx;
 
  /* text window ? */
  int is_text;

  /* this is the map window ? */
  int is_map;

  /* does the window have see-through parts ?  This is normally false.
   * Only used for text on the tombstone currently.  When true, lower
   * windows don't get clipped against this one, and _no_ background
   * is drawn.
   */
  int see_through;

  /* for see-through windows, whether to blend in the text (needed for
   * the RIP image).
   */
  int is_blended;

  /* background color.  When the tiles don't take up all of the
   * window's area, this color fills in the unused portions.  Also
   * used for text background.
   */
  rgbcol_t background;

  /* used for panning.  The values represent an offset (in screen
   * pixels) into the array.  Larger pan_x values shift the tiles
   * further left.  Larger pan_y values shift the tiles further
   * downwards.  (negative values are useful too).
   */
  int pan_x, pan_y;
  
  /* used for zooming.  The values represent the displayed size of
   * each tile (in screen pixels).  For NHW_MAP windows using the
   * pseudo 3D tileset, scale_w/h *ignore* the overlap portion of the
   * tiles (i.e. they give the stepping distance), and scale_full_w/h
   * give the size of the full tile.  For all tilesets except the
   * pseudo 3D one, the full_* values equal the plain values.
   */
  int scale_w, scale_h;
  int scale_full_w, scale_full_h;
  int scale_skew;

  /* the position (in terms of tiles) where to draw a cursor.
   * Negative values mean that no cursor should be drawn.
   */
  int curs_x, curs_y, curs_w;
 
  /* cursor type: 1 for block, 0 for outline */
  int curs_block;
   
  /* cursor color */
  rgbcol_t curs_color;

  /* the array of tile indexes.  Order is left->right, bottom->top.
   * For text windows, the values are characters (the charset is IBM
   * cp437, i.e. standard CGA/VGA), and the color field is used.  For
   * the MAP window, the `bg' field in the TilePair is used as the
   * background tile.
   *
   * The color field contains the foreground in the least significant
   * 8 bits, background in the most significant 8 bits.  Both are
   * indices into sdlgl_text_colors[].  NOTE: background color not
   * actually used yet (if ever).
   */
  struct TilePair *tiles;

  /* set of extra shapes to draw */
  struct ExtraShape *extra_shapes;
  int extra_max;
  int extra_num;

  /* used for the map window to draw a border around it */
  int has_border;
};

extern struct TileSet *sdlgl_tiles;

E void NDECL(sdlgl_tile_startup);
E void NDECL(sdlgl_tile_shutdown);
E void NDECL(sdlgl_tile_load_rest);
E void NDECL(sdlgl_start_logo);
E int NDECL(sdlgl_iterate_logo);
E int FDECL(sdlgl_display_RIP, (int));
E void NDECL(sdlgl_dismiss_RIP);

E struct TileWindow *FDECL(sdlgl_new_tilewin,
    (struct TileSet *, int, int, int, int));
E void FDECL(sdlgl_map_tilewin,
    (struct TileWindow *, int, int, int, int, int));
E void FDECL(sdlgl_unmap_tilewin, (struct TileWindow *));
E void FDECL(sdlgl_free_tilewin, (struct TileWindow *));
E void FDECL(sdlgl_change_tileset, (struct TileWindow *,
      struct TileSet *, int));
E void NDECL(sdlgl_flush);

E void FDECL(sdlgl_add_extrashape, (struct TileWindow *,
      enum ShapeType, short, short, int, int));
E void FDECL(sdlgl_remove_extrashapes, (struct TileWindow *,
      short, short));
E int FDECL(sdlgl_test_tile_visible, (int, int, int, int, int));
E void FDECL(sdlgl_set_start_depth, (int));

E void FDECL(sdlgl_store_char, (struct TileWindow *, int, int,
      char, rgbcol_t));
E int FDECL(sdlgl_store_str, (struct TileWindow *, int, int,
      const char *, int, rgbcol_t));
E void FDECL(sdlgl_store_tile, (struct TileWindow *, int, int,
      tileidx_t, tileidx_t, tileidx_t, tileflags_t));
E void FDECL(sdlgl_blank_area, (struct TileWindow *, int, int, int, int));
E void FDECL(sdlgl_copy_area, (struct TileWindow *, int, int,
      int, int, int, int));
E void FDECL(sdlgl_transfer_area, (struct TileWindow *, int, int,
      int, int, struct TileWindow *, int, int));
E void FDECL(sdlgl_transfer_line, (struct TileWindow *, int, int,
      int, struct TilePair *, int));
E void FDECL(sdlgl_set_scale, (struct TileWindow *, int));
E void FDECL(sdlgl_set_cursor, (struct TileWindow *, int, int, int));


/*
 * GL_IMAGE
 */

extern unsigned char pet_mark_bits[8];
extern unsigned char ridden_mark_bits[8];

extern char tile_16_face_dirs[1000];
extern char tile_32_face_dirs[1000];

extern SDL_Cursor *sdlgl_cursor_main;
extern SDL_Cursor *sdlgl_cursor_left;
extern SDL_Cursor *sdlgl_cursor_right;
extern SDL_Cursor *sdlgl_cursor_up;
extern SDL_Cursor *sdlgl_cursor_down;
extern SDL_Cursor *sdlgl_cursor_hand;
extern SDL_Cursor *sdlgl_cursor_cross;

extern unsigned char sdlgl_gamma_table[256];

#define GAMMA(n)    (sdlgl_gamma_table[n])
#define GAMMA_F(n)  ((float)GAMMA(n) / 255.0)

E int FDECL(sdlgl_quantize_tile_size, (int));
E int FDECL(sdlgl_mon_tile_face_dir, (tileidx_t));
E void NDECL(sdlgl_generate_gamma_table);
E void NDECL(sdlgl_init_mouse_cursors);
E void NDECL(sdlgl_free_mouse_cursors);

E void FDECL(sdlgl_load_face_dirs, (const char *, char *));
E unsigned char * FDECL(sdlgl_load_png_file, (const char*, int*, int*));
E int FDECL(sdlgl_save_ppm_file, (const char *, const unsigned char *,
      int, int));


/*
 * GL_FONT
 */

#define CHAR_2_TILE(ch)  ((tileidx_t)(unsigned char)(ch))
#define TILE_2_CHAR(t)   ((unsigned char)(t))

#define TXT_MAKE(r,g,b)  ((r)*25 + (g)*5 + (b))

#define TXT_GET_R(txt)   (((txt) / 25) % 5)
#define TXT_GET_G(txt)   (((txt) / 5)  % 5)
#define TXT_GET_B(txt)   (((txt)    )  % 5)

#define BLACK      TXT_MAKE(0,0,0)
#define D_GREY     TXT_MAKE(1,1,1)
#define GREY       TXT_MAKE(2,2,2)
#define L_GREY     TXT_MAKE(3,3,3)
#define WHITE      TXT_MAKE(4,4,4)

#define RED        TXT_MAKE(3,0,0)
#define BROWN      TXT_MAKE(3,2,0)
#define GREEN      TXT_MAKE(0,3,0)
#define CYAN       TXT_MAKE(0,3,3)
#define BLUE       TXT_MAKE(0,0,3)
#define MAGENTA    TXT_MAKE(3,0,3)

#define B_RED      TXT_MAKE(4,0,0)
#define B_YELLOW   TXT_MAKE(4,4,0)
#define B_ORANGE   TXT_MAKE(4,3,0)
#define B_GREEN    TXT_MAKE(0,4,0)
#define B_CYAN     TXT_MAKE(0,4,4)
#define B_BLUE     TXT_MAKE(0,0,4)
#define B_MAGENTA  TXT_MAKE(4,0,4)

#define TILECOL_UNUSED  0x7EEE

struct FontCache
{
  /* cache size (total number of characters on surface) */
  int char_num;

  SDL_Surface *char_surf;

  /* how the characters are packed in the surface */
  int pack_w, pack_h;

  /* what color each character in the cache currently is.  Initially
   * all values are TILECOL_UNUSED.
   */
  tilecol_t *what_cols;
};

#define FONTCACHE_SIZE    512
#define FONTCACHE_PACK_W  32

/* this hash function has been carefully chosen so that the two most
 * used colors (L_GREY and GREY) have no collisions.
 */
#define FONTCACHE_HASH(ch,col)  (int) \
    (TXT_GET_R(col) * 41 + TXT_GET_G(col) * 31 + \
     TXT_GET_B(col) * 23 + 2 * (unsigned int) ch)

extern tilecol_t termcolor_to_tilecol[CLR_MAX];

/* colors used for text.  Index 0 guaranteed to be black */
extern rgbcol_t sdlgl_text_colors[256];

extern struct TileSet *sdlgl_font_8;  /* 8x8 (rip) font */

extern struct TileSet *sdlgl_font_map;
extern struct TileSet *sdlgl_font_text;
extern struct TileSet *sdlgl_font_menu;
extern struct TileSet *sdlgl_font_message;
extern struct TileSet *sdlgl_font_status;

E void NDECL(sdlgl_font_startup);
E void NDECL(sdlgl_font_shutdown);
E int FDECL(sdlgl_quantize_font, (int));
E struct TileSet *FDECL(sdlgl_next_narrower_font, (int));


/*
 * GL_HARDW / GL_SOFTW
 *
 * (Rendering API)
 */
struct rendering_procs
{
  void NDECL((*rend_startup));
  void NDECL((*rend_shutdown));
  struct TileSet *FDECL((*load_tileset),
      (const char *, int, int, int, int, int *, int *));
  void FDECL((*free_tileset), (struct TileSet *));
  void NDECL((*create_extra_graphics));
  void NDECL((*free_extra_shapes));

  void FDECL((*enable_clipper), (int, int, int, int));
  void NDECL((*disable_clipper));
  void NDECL((*blit_frame));

  void FDECL((*draw_background), (int, int, int, int, rgbcol_t,
      int, int));
  void FDECL((*draw_extra_shape), (struct TileWindow *,
      struct ExtraShape *));
  void FDECL((*draw_cursor), (struct TileWindow *));
  void FDECL((*begin_tile_draw), (int, int));
  void FDECL((*draw_tile), (struct TileWindow *, int, int,
      int, int, tileidx_t, tilecol_t, tileflags_t, short));
  void NDECL((*finish_tile_draw));
  void FDECL((*draw_border), (struct TileWindow *, rgbcol_t));

  void FDECL((*start_fading), (int, int));
  void FDECL((*draw_fading), (float));
  void NDECL((*finish_fading));
  void FDECL((*make_screenshot), (const char *));

  void FDECL((*set_pan), (struct TileWindow *, int, int));
  void FDECL((*set_new_pos), (struct TileWindow *, int, int, int, int));
  void FDECL((*mark_dirty), (int, int, int, int, int));
};

extern struct rendering_procs sdlgl_rend_procs;

#ifdef GL_GRAPHICS
extern struct rendering_procs sdlgl_hardw_rend_procs;
#endif

#ifdef SDL_GRAPHICS
extern struct rendering_procs sdlgl_softw_rend_procs;
#endif

#define sdlgl_rend_startup  (*sdlgl_rend_procs.rend_startup)
#define sdlgl_rend_shutdown (*sdlgl_rend_procs.rend_shutdown)
#define sdlgl_load_tileset  (*sdlgl_rend_procs.load_tileset)
#define sdlgl_free_tileset  (*sdlgl_rend_procs.free_tileset)
#define sdlgl_create_extra_graphics (*sdlgl_rend_procs.create_extra_graphics)
#define sdlgl_free_extra_shapes (*sdlgl_rend_procs.free_extra_shapes)
#define sdlgl_enable_clipper   (*sdlgl_rend_procs.enable_clipper)
#define sdlgl_disable_clipper  (*sdlgl_rend_procs.disable_clipper)
#define sdlgl_blit_frame  (*sdlgl_rend_procs.blit_frame)
#define sdlgl_draw_background  (*sdlgl_rend_procs.draw_background)
#define sdlgl_draw_extra_shape (*sdlgl_rend_procs.draw_extra_shape)
#define sdlgl_draw_cursor      (*sdlgl_rend_procs.draw_cursor)
#define sdlgl_begin_tile_draw  (*sdlgl_rend_procs.begin_tile_draw)
#define sdlgl_draw_tile        (*sdlgl_rend_procs.draw_tile)
#define sdlgl_finish_tile_draw (*sdlgl_rend_procs.finish_tile_draw)
#define sdlgl_draw_border   (*sdlgl_rend_procs.draw_border)
#define sdlgl_start_fading  (*sdlgl_rend_procs.start_fading)
#define sdlgl_draw_fading   (*sdlgl_rend_procs.draw_fading)
#define sdlgl_finish_fading (*sdlgl_rend_procs.finish_fading)
#define sdlgl_make_screenshot (*sdlgl_rend_procs.make_screenshot)
#define sdlgl_set_pan       (*sdlgl_rend_procs.set_pan)
#define sdlgl_set_new_pos   (*sdlgl_rend_procs.set_new_pos)
#define sdlgl_mark_dirty    (*sdlgl_rend_procs.mark_dirty)

#ifdef GL_GRAPHICS
E GLuint FDECL(sdlgl_send_graphic_RGBA, (unsigned char *, int, int));
#endif


/*
 * GL_RENDU
 */

#define TRANS_PIX  255  /* 8 bit transparent pixel */

extern rgbcol_t sdlgl_palette[256];

#define DIRTY_SIZE   64
#define CLEAN_CELL   0x7F

struct DirtyMatrix
{
  /* pixel size that matrix covers (screen size) */
  int pw, ph;

  /* matrix size (in cells), enough to cover the screen */
  int cw, ch;

  /* cell matrix.  Each cell is either CLEAN_CELL, or the depth of the
   * window that became dirty somewhere inside the cell.  If multiple
   * windows get dirty in the same cell, the *lowest* depth is
   * remembered.  
   *
   * Note: using a high value of CLEAN_CELL is a nice trick,
   *       simplifying the marking and testing code.
   */
  unsigned char *cells;

  /* rectangles used to update screen.  Worst case scenario is
   * slightly over half the total number of cells.
   */
  int max_rects;

  SDL_Rect *updaters;
};

E void FDECL(sdlgl_set_surface_colors, (SDL_Surface *));
E SDL_Surface *FDECL(sdlgl_RGBA_to_truecolor, (unsigned char *,
        int, int));
E SDL_Surface *FDECL(sdlgl_RGBA_to_palettised, (unsigned char *,
        int, int));
E SDL_Surface *FDECL(sdlgl_shrink_surface, (SDL_Surface *));
E void FDECL(sdlgl_sw_create_has_alpha, (struct TileSet *));

E void FDECL(sdlgl_create_font_cache, (struct TileSet *));
E void FDECL(sdlgl_free_font_cache, (struct TileSet *));
E void FDECL(sdlgl_font_cache_lookup, (struct TileSet *, tileidx_t,
        tilecol_t, int *, int *));

E struct DirtyMatrix *FDECL(sdlgl_create_dirty_matrix, (int, int));
E void FDECL(sdlgl_free_dirty_matrix, (struct DirtyMatrix *));
E void FDECL(sdlgl_dirty_matrix_clear, (struct DirtyMatrix *));
E void FDECL(sdlgl_dirty_matrix_add, (struct DirtyMatrix *, int, int,
      int, int, int));
E int FDECL(sdlgl_dirty_matrix_test, (struct DirtyMatrix *, int, int,
      int, int, int));
E int FDECL(sdlgl_dirty_matrix_to_updaters, (struct DirtyMatrix *));
E void FDECL(sdlgl_dirty_matrix_blit, (struct DirtyMatrix *, 
      SDL_Surface *, SDL_Rect *, SDL_Rect *, Uint32, int));


/*
 * GL_WIN
 */

#define MAXWIN  20

/* active NHW_TEXT/NHW_MENU window, otherwise WIN_ERR */
extern int sdlgl_top_win;

/* active NHW_MAP window, otherwise WIN_ERR */
extern int sdlgl_map_win;

#define MIN_MESSAGE  2
#define MAX_MESSAGE  8

#define MIN_HISTORY  10
#define MAX_HISTORY  200   /* max history saved on message window */

typedef unsigned short glyphidx_t;

struct GlyphPair
{
  /* background and foreground glyphs.  One or both may be NO_GLYPH.
   * FG is only for objects/monsters/etc, and BG is only for dungeon
   * glyphs (CMAP).
   */
  glyphidx_t bg, fg;
};

#define MENU_NONE_COL  RGB_MAKE(48, 48, 48)
#define MENU_ONE_COL   RGB_MAKE(16, 32, 48)
#define MENU_ANY_COL   RGB_MAKE(0,  0,  48)
#define TEXT_BACK_COL  RGB_MAKE(0,  0,  48)
#define PREV_BACK_COL  RGB_MAKE(48, 24,  0)

#define BORDER_COL     RGB_MAKE(0,  0,  96)
#define CURSOR_COL     RGB_MAKE(192, 0, 0)
#define OUTLINE_COL    RGB_MAKE(255, 255, 255)


struct MouseLocation
{
  /* what the action should be at the current location.  This also
   * indicates what the mouse cursor should look like.  When the mouse
   * is outside the window (or when focus has been lost), then the
   * MACT_AWAY value is used.
   */
  enum MouseLocAction
  {
    MACT_NORMAL = 0,
    MACT_AWAY,
    MACT_UP, MACT_DOWN, MACT_LEFT, MACT_RIGHT,
    MACT_HAND, MACT_CROSS
  }
  action;

  /* pixel position of mouse cursor.  The coordinates are GL style
   * (i.e. y goes from the bottom up).
   */
  int x, y;
  
  /* the window that the mouse pointer is currently over.  Will be
   * WIN_ERR when not sitting over any window, e.g. a gap, or when the
   * focus has been lost.
   */
  int window;
};

extern struct MouseLocation sdlgl_mouseloc;


struct TextWindow
{
  /* type (one of NHW_XXX) */
  int type;

  /* for NHW_MENU types: whether this window is being used as an
   * actual menu or just for text.  Will be -1 until the
   * distinguishing call (start_menu | putstr) has been made.
   */
  int is_menu;

  /* is this window (NHW_TEXT type) used for the RIP message ? */
  int is_rip;
  int how_died;
  
  /* linked list of menu items.  Only valid when is_menu == 1.  Note
   * that this list goes upwards from the bottom of the window (not
   * top-down, as you might expect).
   */
  struct MenuItem *items;
  
  /* linked list of text items.  Only valid when type is NHW_TEXT, or
   * NHW_MENU and is_menu == 0.  This one also goes upwards.
   */
  struct TextItem *lines;
  
  /* computed width & height of the menu or text window.  For menus
   * (is_menu == 1), it's only valid after the end_menu() interface
   * routine has been called.  For text items, it is computed when
   * displayed.
   */
  int calc_w, calc_h;

  /* for menu/text windows, the number of tiles that completely fit on
   * the screen at any one time.  For small windows, this'll be the
   * same as calc_w/h.  Also valid for NHW_MESSAGE/STATUS windows.
   */
  int show_w, show_h;

  /* based on a tile window.  This is NULL while the window is
   * unmapped (i.e. display_nhwindow hasn't been called).
   */
  struct TileWindow *base;

  /* only valid for NHW_MAP windows, and then only when mapped.
   * Stores a record of all the glyphs on the map.  The size is
   * exactly the same as in the `base' field.
   */
  struct GlyphPair *glyphs;
 
  /* tile coordinates for the "focus", which (for NHW_MAP windows) is
   * the tile that controls where the jail is (usually the focus is
   * where the player tile is).
   *
   * For other window types, this is the tile to show at the bottom
   * left corner of the window.
   */
  int focus_x, focus_y;

  /* panning values for NHW_MAP windows.  Remembers the pan state of
   * graphical tiles when the text-mode view is active.
   */
  int map_px, map_py;
 
  /* the JAIL...  This is an rectangular area that is meant to be kept
   * at the middle of the screen.  The player can move freely within
   * the jail without the map window panning -- when the player moves
   * outside of the jail, the map window will usually pan.
   *
   * The jail is mostly implicit (size calc'd from sdlgl_jail_size).
   * Here is the offset (in pixels) from center of screen to jail's
   * center.  The jail is allowed to be uncentered on the screen,
   * though it should auto-correct itself, i.e. move closer to the
   * center instead of panning.  The jail (in pixel terms) must always
   * fit on the screen.  Hence when jail size is 100%, these offsets
   * are always zero.
   *
   * The jail is not used when in text-mode view (determined by
   * base->is_text), as we assume the whole map fits on the screen.
   */
  int jail_x, jail_y;
 
  /* current zoom factor.  Only used with NHW_MAP windows.
   * The value is the pixel height that the tiles are rendered at, so
   * higher values mean larger tiles.  The allowable sizes depends on
   * what tileset is being used and whether we are using GL or
   * Software rendering.  Not used when in text-mode view (i.e.
   * base->is_text is set) as we assume the whole map fits on the
   * screen.  Invariant: zoom_h != TEXT_ZOOM.
   */
  int zoom_h;

  /* For NHW_MAP windows, this field remembers which direction the
   * player was travelling in.  Used for the tile flipping option.
   */
  int player_dx;
  int player_dy;

  /* the current write position (in terms of tiles).  Must always be
   * inside the tile window area.  Order same as tiles: left->right,
   * bottom->top.
   */
  int write_x, write_y;

  /* whether we show a cursor at the current write position.  Also
   * used for NHW_MAP windows for cursor on player.
   */
  int write_cursor;

  /* current writing color */
  tilecol_t write_col;

  /* current filling color (for clear_screen, etc) */
  tilecol_t fill_col;

  /* for NHW_MESSAGE windows: number of "fresh" lines, i.e. lines that
   * have been written onto the message window since clear_nhwindow()
   * or the last do_more().  Initial state is 0, meaning "blank".
   * After a [More] it is also 0.  As soon as this value becomes the
   * same as the `show_h' field, that's when we display the more text
   * and wait for a key.
   *
   * Note: Other routines that use the MESSAGE window (e.g. getlin)
   * should also call sdlgl_more() when this value is non-zero.
   */
  int fresh_lines;

  /* for NHW_MESSAGE windows: true if the next putstr() should be
   * placed on a new line, even if it fits on the previous one.
   */
  int want_new_line;
 
  /* for NHW_MESSAGE windows: this field is for the scroll-back
   * capability (^P).  The tile window is normally unmapped, silently
   * recording lines that scroll off the screen (except empty lines).
   * Clearing the msg window is equivalent to fully scrolling it up.
   */
  struct TileWindow *scrollback;

  /* when true, scrolled-off lines are copied to the scrollback
   * buffer.  This is disabled when routines that take over the
   * message window (get_ext_cmd, etc) are active.
   */
  int scrollback_enable;
 
  /* this is 0 if the scrollback window is unmapped.  This value only
   * used in the `alt_prev' mode, and signifies how many lines back we
   * are showing.
   */
  int scrollback_pos;

  /* number of non-blank lines currently in the scrollback window.
   * Used to limit the scrollback to readable stuff.
   */
  int scrollback_size;

  /* for NHW_MESSAGE windows, these fields handle the message_menu()
   * hack.  When dismiss_more is non-zero, it is a character that can
   * be used to accept the single choice while [MORE] is displayed
   * (setting the more_ch).
   */
  int dismiss_more;
  int more_ch;

  /* for NHW_MESSAGE windows, this will be set when the user pressed
   * the Escape key to dismiss the more.  It should be cleared before
   * every turn, and when certain interface functions are called
   * (message_menu, etc).  It is used to prevent further [MORE]
   * prompts (the TTY version also stops further output to the message
   * window, but I'd prefer to see them).
   */
  int more_escaped;
};

extern struct TextWindow *text_wins[MAXWIN];

E winid FDECL(Sdlgl_create_nhwindow, (int));
E void FDECL(Sdlgl_clear_nhwindow, (winid));
E void FDECL(Sdlgl_display_nhwindow, (winid, BOOLEAN_P));
E void FDECL(Sdlgl_dismiss_nhwindow, (winid));
E void FDECL(Sdlgl_destroy_nhwindow, (winid));
E void FDECL(Sdlgl_curs, (winid,int,int));
E void FDECL(Sdlgl_putstr, (winid, int, const char *));
E void FDECL(Sdlgl_outrip, (winid,int));
E int NDECL(Sdlgl_doprev_message);

E void NDECL(sdlgl_win_startup);
E void NDECL(sdlgl_win_shutdown);
E void FDECL(sdlgl_more, (struct TextWindow *));
E void FDECL(sdlgl_adjust_scrollback, (struct TextWindow *, int));
E void NDECL(sdlgl_remove_scrollback);
E void FDECL(sdlgl_pan_window, (int, int, int));
E tilecol_t FDECL(sdlgl_attr_to_tilecol, (int attr));
E void FDECL(sdlgl_update_mouse_location, (int));

E int FDECL(sdlgl_internal_key_handler, (SDL_keysym *, int));
E int FDECL(sdlgl_internal_button_handler, (SDL_MouseButtonEvent *));
E void FDECL(sdlgl_internal_motion_handler, (SDL_MouseMotionEvent *));


/*
 * GL_MENU
 */

E void FDECL(Sdlgl_start_menu, (winid));
E void FDECL(Sdlgl_add_menu, (winid,int,const ANY_P *,
    CHAR_P,CHAR_P,int,const char *, BOOLEAN_P));
E void FDECL(Sdlgl_end_menu, (winid, const char *));
E int FDECL(Sdlgl_select_menu, (winid, int, MENU_ITEM_P **));
E char FDECL(Sdlgl_message_menu, (CHAR_P,int,const char *));
E char FDECL(Sdlgl_yn_function, (const char *, const char *, CHAR_P));

E void FDECL(sdlgl_clear_menu_items, (struct TextWindow *));
E int FDECL(sdlgl_process_menu_window, (int, struct TextWindow *, int));


/*
 * GL_TEXT
 */

E void FDECL(Sdlgl_display_file, (const char *, BOOLEAN_P));

E void FDECL(sdlgl_clear_text_items, (struct TextWindow *));
E void FDECL(sdlgl_insert_text_item, (struct TextWindow *,
    struct TextItem *, int, int, const char *));
E void FDECL(sdlgl_process_text_window, (int, struct TextWindow *));


/*
 * GL_MAP
 */

#if defined(VANILLA_GLHACK)
#define NUM_MON_TILES  393
#else
#define NUM_MON_TILES  603
#endif

/* extra floor tiles */
#if defined(VANILLA_GLHACK)
#define FLOOR_TILES  1057
#else
#define FLOOR_TILES  1367
#endif

#define Fl_Mine     (FLOOR_TILES + 0)
#define Fl_Sokoban  (FLOOR_TILES + 1)
#define Fl_Knox     Fl_Sokoban
#define Fl_Tower    (FLOOR_TILES + 2)
#define Fl_Hell     (FLOOR_TILES + 3)
#define Fl_Quest    (FLOOR_TILES + 4)
#define Fl_Astral   (FLOOR_TILES + 5)
#define Fl_Rogue    (FLOOR_TILES + 6)
#define Fl_Caves    Fl_Rogue
#define Fl_Beach    (FLOOR_TILES + 7)

E void FDECL(Sdlgl_cliparound, (int, int));
E void FDECL(Sdlgl_update_positionbar, (char *));
E void FDECL(Sdlgl_print_glyph, (winid,XCHAR_P,XCHAR_P,int));

E void FDECL(sdlgl_create_map, (struct TextWindow *, int, int));
E void FDECL(sdlgl_zoom_map, (int));
E int FDECL(sdlgl_quantize_zoom, (int));
E int NDECL(sdlgl_cursor_visible);
E void NDECL(sdlgl_center_screen_on_player);
E void NDECL(sdlgl_toggle_text_view);
E int FDECL(sdlgl_find_click, (int, int *, int *));
E void FDECL(sdlgl_pan_map_window, (int, int));


/*
 * GL_EMUL
 */

E void NDECL(sdlgl_emul_startup);
E void NDECL(sdlgl_emul_shutdown);
E void NDECL(Sdlgl_start_screen);
E void NDECL(Sdlgl_end_screen);
E void NDECL(Sdlgl_askname);
E void FDECL(Sdlgl_getlin, (const char *,char *));
E int NDECL(Sdlgl_get_ext_cmd);

E struct TextWindow *FDECL(sdlgl_new_textwin, (int));
E void FDECL(sdlgl_free_textwin, (struct TextWindow *));
E void FDECL(sdlgl_putc, (struct TextWindow *, int));
E void FDECL(sdlgl_puts, (struct TextWindow *, const char *));
E void FDECL(sdlgl_puts_nolf, (struct TextWindow *, const char *));
E void FDECL(sdlgl_gotoxy, (struct TextWindow *, int, int));
E void FDECL(sdlgl_home, (struct TextWindow *));
E void FDECL(sdlgl_backsp, (struct TextWindow *));
E void FDECL(sdlgl_clear, (struct TextWindow *));
E void FDECL(sdlgl_clear_end, (struct TextWindow *));
E void FDECL(sdlgl_clear_eos, (struct TextWindow *));
E void FDECL(sdlgl_enable_cursor, (struct TextWindow *, int));


/*
 * GL_STAT
 */
E void FDECL(sdlgl_write_status, (struct TextWindow *win));
E void FDECL(sdlgl_update_status, (int));


#undef E

#endif /* WINGL_INTERNAL */
#endif /* WINGL_H */
/*winGL.c*/
