/* $Id: proxycom.h,v 1.17 2004-04-19 06:56:41 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2002-2004 */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef PROXYCOM_H
#define PROXYCOM_H

/* The NhExt standard to which we conform */

#define EXT_STANDARD_MAJOR	1
#define EXT_STANDARD_MINOR	0
#define EXT_STANDARD_MICRO	2	/* Not used in the code */

#define EXT_IM_STATUS		0x0001
#define EXT_IM_DISPLAY_LAYERS	0x0002

#define EXT_PARAM_INT		0x4001
#define EXT_PARAM_LONG		0x4002
#define EXT_PARAM_STRING	0x4003
#define EXT_PARAM_BYTES		0x4004
#define EXT_PARAM_BOOLEAN	0x4005
#define EXT_PARAM_CHAR		0x4006
#define EXT_PARAM_XDRF		0x4007

#define EXT_PARAM_PTR		0x8000

#define EXT_INT(i)		EXT_PARAM_INT, i
#define EXT_LONG(l)		EXT_PARAM_LONG, l
#define EXT_STRING(s)		EXT_PARAM_STRING, (s) ? (s) : ""
#define EXT_BYTES(buf,nb)	EXT_PARAM_BYTES, buf, nb
#define EXT_BOOLEAN(b)		EXT_PARAM_BOOLEAN, b
#define EXT_CHAR(c)		EXT_PARAM_CHAR, c
#define EXT_XDRF(func,data)	EXT_PARAM_XDRF, func, data

#define EXT_INT_P(i)		EXT_PARAM_PTR | EXT_PARAM_INT, &(i)
#define EXT_LONG_P(l)		EXT_PARAM_PTR | EXT_PARAM_LONG, &(l)
#define EXT_STRING_P(s)		EXT_PARAM_PTR | EXT_PARAM_STRING, &(s)
#define EXT_BYTES_P(buf,nb)	EXT_PARAM_PTR | EXT_PARAM_BYTES, &(buf), &(nb)
#define EXT_BOOLEAN_P(b)	EXT_PARAM_PTR | EXT_PARAM_BOOLEAN, &(b)
#define EXT_CHAR_P(c)		EXT_PARAM_PTR | EXT_PARAM_CHAR, &(c)

#define EXT_SPECIAL_ERROR	0

#define EXT_ERROR_UNSUPPORTED		1
#define EXT_ERROR_UNAVAILABLE		2
#define EXT_ERROR_INVALIDENCODING	3
#define EXT_ERROR_INVALIDPARAMS		4
#define EXT_ERROR_RESOURCEFAILURE	5

#define EXT_ERROR_GENERIC		0x100

#define EXT_ERROR_INTERNAL		0x101
#define EXT_ERROR_COMMS			0x102
#define EXT_ERROR_NOTSUPPORTED		0x103
#define EXT_ERROR_PROTOCOL		0x104

#ifdef NHXDR_H
struct nhext_svc {
    unsigned short id;
    void (*handler)(unsigned short, NhExtXdr *, NhExtXdr *);
};

struct nhext_line {
    char *type;
    int n;
    char **tags, **values;
};

typedef void (*nhext_errhandler)(int, const char *);

E int nhext_init(NhExtIO *, NhExtIO *, struct nhext_svc *);
E int nhext_set_protocol(int);
E int nhext_async_mode(void);
E void nhext_end(void);
E int nhext_subprotocol0_write_line(struct nhext_line *);
E void nhext_subprotocol0_free_line(struct nhext_line *);
E struct nhext_line *nhext_subprotocol0_read_line(void);
E char *nhext_subprotocol0_get_failed_packet(int *);
E nhext_errhandler nhext_set_errhandler(nhext_errhandler);
E int nhext_rpc_params(NhExtXdr *xdrs, int, ...);
E int nhext_rpc(unsigned short, ...);
E int nhext_svc(struct nhext_svc *);
E void nhext_set_async_masks(int, unsigned long *);
E void nhext_set_unsupported(int);
E unsigned short nhext_rpc_get_next_serial(void);
E unsigned short nhext_svc_get_serial(void);
E void nhext_send_error(unsigned short, unsigned char);
#endif	/* NHXDR_H */

#define EXT_FID_INIT			0x01
#define EXT_FID_INIT_NHWINDOWS		0x02
#define EXT_FID_PLAYER_SELECTION	0x03
#define EXT_FID_ASKNAME			0x04
#define EXT_FID_GET_NH_EVENT		0x05
#define EXT_FID_EXIT_NHWINDOWS		0x06
#define EXT_FID_SUSPEND_NHWINDOWS	0x07
#define EXT_FID_RESUME_NHWINDOWS	0x08
#define EXT_FID_CREATE_NHWINDOW		0x09
#define EXT_FID_CLEAR_NHWINDOW		0x0A
#define EXT_FID_DISPLAY_NHWINDOW	0x0B
#define EXT_FID_DESTROY_NHWINDOW	0x0C
#define EXT_FID_CURS			0x0D
#define EXT_FID_PUTSTR			0x0E
#define EXT_FID_DISPLAY_FILE		0x0F
#define EXT_FID_START_MENU		0x10
#define EXT_FID_ADD_MENU		0x11
#define EXT_FID_END_MENU		0x12
#define EXT_FID_SELECT_MENU		0x13
#define EXT_FID_MESSAGE_MENU		0x14
#define EXT_FID_UPDATE_INVENTORY	0x15
#define EXT_FID_MARK_SYNC		0x16
#define EXT_FID_WAIT_SYNC		0x17
#define EXT_FID_CLIPAROUND		0x18
#define EXT_FID_UPDATE_POSITIONBAR	0x19
#define EXT_FID_PRINT_GLYPH		0x1A
#define EXT_FID_RAW_PRINT		0x1B
#define EXT_FID_RAW_PRINT_BOLD		0x1C
#define EXT_FID_NHGETCH			0x1D
#define EXT_FID_NH_POSKEY		0x1E
#define EXT_FID_NHBELL			0x1F
#define EXT_FID_DOPREV_MESSAGE		0x20
#define EXT_FID_YN_FUNCTION		0x21
#define EXT_FID_GETLIN			0x22
#define EXT_FID_GET_EXT_CMD		0x23
#define EXT_FID_NUMBER_PAD		0x24
#define EXT_FID_DELAY_OUTPUT		0x25
#define EXT_FID_CHANGE_COLOR		0x26
#define EXT_FID_CHANGE_BACKGROUND	0x27
#define EXT_FID_SET_FONT_NAME		0x28
#define EXT_FID_GET_COLOR_STRING	0x29
#define EXT_FID_START_SCREEN		0x2A
#define EXT_FID_END_SCREEN		0x2B
#define EXT_FID_OUTRIP			0x2C
#define EXT_FID_PREFERENCE_UPDATE	0x2D
#define EXT_FID_STATUS			0x2E
#define EXT_FID_PRINT_GLYPH_LAYERED	0x2F
#define EXT_FID_SEND_CONFIG_FILE	0x30

#ifdef NHXDR_H
struct proxy_init_nhwindow_req {
    int argc;
    char **argv;
};

struct proxy_init_nhwindow_res {
    nhext_xdr_bool_t inited;
    int argc;
    char **argv;
    int capc;
    char **capv;
};

struct proxy_mi {
    int item;			/* identifier */
    long count;			/* count */
};

struct proxy_select_menu_res {
    int retval;
    int n;
    struct proxy_mi *selected;
};

struct proxy_status_req {
    int reconfig;
    int nv;
    const char **values;
};

struct proxy_glyph_row {
    int start;
    int ng;
    int *glyphs;
};

struct proxy_glyph_layer {
    int start;
    int nr;
    struct proxy_glyph_row *rows;
};

struct proxy_print_glyph_layered_req {
    int window;
    int nl;
    struct proxy_glyph_layer *layers;
};

E nhext_xdr_bool_t proxy_xdr_init_nhwindow_req(NhExtXdr *, struct proxy_init_nhwindow_req *);
E nhext_xdr_bool_t proxy_xdr_init_nhwindow_res(NhExtXdr *, struct proxy_init_nhwindow_res *);
E nhext_xdr_bool_t proxy_xdr_proxy_mi(NhExtXdr *, struct proxy_mi *);
E nhext_xdr_bool_t proxy_xdr_select_menu_res(NhExtXdr *, struct proxy_select_menu_res *);
E nhext_xdr_bool_t proxy_xdr_status_req(NhExtXdr *, struct proxy_status_req *);
E nhext_xdr_bool_t proxy_xdr_print_glyph_layered_req(NhExtXdr *, struct proxy_print_glyph_layered_req *);
#endif	/* NHXDR_H */

#define EXT_CID_DISPLAY_INVENTORY	0x01
#define EXT_CID_DLBH_FOPEN		0x02
#define EXT_CID_DLBH_FGETS		0x03
#define EXT_CID_DLBH_FREAD		0x04
#define EXT_CID_DLBH_FWRITE		0x05
#define EXT_CID_DLBH_FCLOSE		0x06
#define EXT_CID_DLBH_FMD5SUM		0x07
#define EXT_CID_FLUSH_SCREEN		0x08
#define EXT_CID_DOREDRAW		0x09
#define EXT_CID_INTERFACE_MODE		0x0A
#define EXT_CID_PARSE_OPTIONS		0x0B
#define EXT_CID_GET_OPTION		0x0C
#define EXT_CID_GET_PLAYER_CHOICES	0x0D
#define EXT_CID_GET_VALID_SELECTIONS	0x0E
#define EXT_CID_QUIT_GAME		0x0F
#define EXT_CID_DISPLAY_SCORE		0x10
#define EXT_CID_DOSET			0x11
#define EXT_CID_GET_EXTENDED_COMMANDS	0x12
#define EXT_CID_MAP_MENU_CMD		0x13
#define EXT_CID_GET_STANDARD_WINID	0x14
#define EXT_CID_GET_TILESETS		0x15
#define EXT_CID_GET_GLYPH_MAPPING	0x16
#define EXT_CID_GET_EXTENSIONS		0x17
#define EXT_CID_SET_OPTION_MOD_STATUS	0x18

#ifdef NHXDR_H
struct proxycb_dlbh_fmd5sum_res {
	int retval;
	const char *digest;
};

struct proxycb_get_player_choices_res_role {
	const char *male;
	const char *female;
};

struct proxycb_get_player_choices_res {
	int n_aligns;
	const char **aligns;
	int n_genders;
	const char **genders;
	int n_races;
	const char **races;
	int n_roles;
	struct proxycb_get_player_choices_res_role *roles;
};

struct proxycb_get_valid_selections_res {
	int no_roles;
	int no_races;
	int no_aligns;
	int no_genders;
	int n_masks;
	unsigned long *masks;
};

struct proxycb_get_extended_commands_res {
	int n_commands;
	const char **commands;
};

struct proxycb_get_tilesets_res_tileset {
	const char *name;
	const char *file;
	const char *mapfile;
	unsigned long flags;
};

struct proxycb_get_tilesets_res {
	int n_tilesets;
	struct proxycb_get_tilesets_res_tileset *tilesets;
};

#define RGB_SYM(rgb, sym)	((rgb) << 8 | (sym))
#define RGBSYM_RGB(rgbsym)	((rgbsym) >> 8 & 0xFFFFFF)
#define RGBSYM_SYM(rgbsym)	((rgbsym) & 0xFF)

struct proxycb_get_glyph_mapping_res_symdef {
	long rgbsym;
	const char *description;
};

struct proxycb_get_glyph_mapping_res_submapping {
	struct proxycb_get_glyph_mapping_res_symdef symdef;
	int n_glyphs;
	struct proxycb_get_glyph_mapping_res_symdef *glyphs;
};

struct proxycb_get_glyph_mapping_res_mapping {
	const char *flags;
	int base_mapping;
	int alt_glyph;
	struct proxycb_get_glyph_mapping_res_symdef symdef;
	int n_submappings;
	struct proxycb_get_glyph_mapping_res_submapping *submappings;
};

struct proxycb_get_glyph_mapping_res {
	int no_glyph;
	long transparent;
	int n_mappings;
	struct proxycb_get_glyph_mapping_res_mapping *mappings;
};

struct proxycb_get_extensions_res_extension {
	const char *name;
	const char *version;
	int no_procedures;
};

struct proxycb_get_extensions_res {
	int n_extensions;
	struct proxycb_get_extensions_res_extension *extensions;
};

/* This structure is used for both the sub-protocol 2 init request and reply */
struct proxycb_subprot2_init {
	int n_masks;
	unsigned long *masks;
};

extern nhext_xdr_bool_t proxycb_xdr_get_player_choices_res_role(NhExtXdr *, struct proxycb_get_player_choices_res_role *);
extern nhext_xdr_bool_t proxycb_xdr_get_player_choices_res(NhExtXdr *, struct proxycb_get_player_choices_res *);
extern nhext_xdr_bool_t proxycb_xdr_get_valid_selections_res(NhExtXdr *, struct proxycb_get_valid_selections_res *);
extern nhext_xdr_bool_t proxycb_xdr_get_extended_commands_res(NhExtXdr *, struct proxycb_get_extended_commands_res *);
extern nhext_xdr_bool_t proxycb_xdr_get_tilesets_res_tileset(NhExtXdr *, struct proxycb_get_tilesets_res_tileset *);
extern nhext_xdr_bool_t proxycb_xdr_get_tilesets_res(NhExtXdr *, struct proxycb_get_tilesets_res *);
extern nhext_xdr_bool_t proxycb_xdr_get_glyph_mapping_res_symdef(NhExtXdr *, struct proxycb_get_glyph_mapping_res_symdef *);
extern nhext_xdr_bool_t proxycb_xdr_get_glyph_mapping_res_submapping(NhExtXdr *, struct proxycb_get_glyph_mapping_res_submapping *);
extern nhext_xdr_bool_t proxycb_xdr_get_glyph_mapping_res_mapping(NhExtXdr *, struct proxycb_get_glyph_mapping_res_mapping *);
extern nhext_xdr_bool_t proxycb_xdr_get_glyph_mapping_res(NhExtXdr *, struct proxycb_get_glyph_mapping_res *);
extern nhext_xdr_bool_t proxycb_xdr_get_extensions_res(NhExtXdr *, struct proxycb_get_extensions_res *);
extern nhext_xdr_bool_t proxycb_xdr_subprot2_init(NhExtXdr *, struct proxycb_subprot2_init *);
#endif  /* NHXDR_H */

/* riputil.c */
extern char * get_killer_string(int);
/* getopt.c */
extern char * get_option(const char *);
/* glyphmap.c */
extern void set_glyph_mapping(void);
extern struct proxycb_get_glyph_mapping_res * get_glyph_mapping(void);
extern void free_glyph_mapping(struct proxycb_get_glyph_mapping_res *);

#endif /* PROXYCOM_H */
