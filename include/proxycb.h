/* $Id: proxycb.h,v 1.10 2002-09-12 18:21:47 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001-2002 */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef PROXYCB_H
#define PROXYCB_H

#define EXT_CID_DISPLAY_INVENTORY	0x01
#define EXT_CID_DLBH_FOPEN		0x02
#define EXT_CID_DLBH_FGETS		0x03
#define EXT_CID_DLBH_FREAD		0x04
#define EXT_CID_DLBH_FCLOSE		0x05
#define EXT_CID_FLUSH_SCREEN		0x06
#define EXT_CID_DOREDRAW		0x07
#define EXT_CID_INTERFACE_MODE		0x08
#define EXT_CID_PARSE_OPTIONS		0x09
#define EXT_CID_GET_OPTION		0x0A
#define EXT_CID_GET_PLAYER_CHOICES	0x0B
#define EXT_CID_IS_VALID_SELECTION	0x0C
#define EXT_CID_QUIT_GAME		0x0D
#define EXT_CID_DISPLAY_SCORE		0x0E
#define EXT_CID_DOSET			0x0F
#define EXT_CID_GET_EXTENDED_COMMANDS	0x10
#define EXT_CID_MAP_MENU_CMD		0x11
#define EXT_CID_GET_STANDARD_WINID	0x12
#define EXT_CID_GET_TILESETS		0x13
#define EXT_CID_GET_GLYPH_MAPPING	0x14

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

extern void NDECL(proxy_cb_display_inventory);
extern int FDECL(proxy_cb_dlbh_fopen, (const char *, const char *));
extern char *FDECL(proxy_cb_dlbh_fgets, (char *, int, int));
extern int FDECL(proxy_cb_dlbh_fread, (char *, int, int, int));
extern int FDECL(proxy_cb_dlbh_fclose, (int));
extern void NDECL(proxy_cb_flush_screen);
extern void NDECL(proxy_cb_doredraw);
extern void FDECL(proxy_cb_interface_mode, (unsigned long));
extern int FDECL(proxy_cb_parse_options, (char *));
extern char *FDECL(proxy_cb_get_option, (char *));
extern struct proxycb_get_player_choices_res
		*NDECL(proxy_cb_get_player_choices);
extern void FDECL(proxy_cb_free_player_choices,
		(struct proxycb_get_player_choices_res *choices));
extern int FDECL(proxy_cb_is_valid_selection, (int, int, int, int));
extern void NDECL(proxy_cb_quit_game);
extern void NDECL(proxy_cb_display_score);
extern void NDECL(proxy_cb_doset);
extern struct proxycb_get_extended_commands_res
		*NDECL(proxy_cb_get_extended_commands);
extern void FDECL(proxy_cb_free_extended_commands,
		(struct proxycb_get_extended_commands_res *commands));
extern int FDECL(proxy_cb_map_menu_cmd, (int));
extern winid FDECL(proxy_cb_get_standard_winid, (char *));
extern struct proxycb_get_tilesets_res *NDECL(proxy_cb_get_tilesets);
extern void FDECL(proxy_cb_free_tilesets,
		(struct proxycb_get_tilesets_res *tilesets));
extern struct proxycb_get_glyph_mapping_res *NDECL(proxy_cb_get_glyph_mapping);
extern void FDECL(proxy_cb_free_glyph_mapping,
		(struct proxycb_get_glyph_mapping_res *mapping));

#ifdef NHXDR_H
extern boolean FDECL(proxycb_xdr_get_player_choices_res_role,
		(NhExtXdr *, struct proxycb_get_player_choices_res_role *));
extern boolean FDECL(proxycb_xdr_get_player_choices_res,
		(NhExtXdr *, struct proxycb_get_player_choices_res *));
extern boolean FDECL(proxycb_xdr_get_extended_commands_res,
		(NhExtXdr *, struct proxycb_get_extended_commands_res *));
extern boolean FDECL(proxycb_xdr_get_tilesets_res_tileset,
		(NhExtXdr *, struct proxycb_get_tilesets_res_tileset *));
extern boolean FDECL(proxycb_xdr_get_tilesets_res,
		(NhExtXdr *, struct proxycb_get_tilesets_res *));
extern boolean FDECL(proxycb_xdr_get_glyph_mapping_res_symdef,
		(NhExtXdr *, struct proxycb_get_glyph_mapping_res_symdef *));
extern boolean FDECL(proxycb_xdr_get_glyph_mapping_res_submapping,
		(NhExtXdr *, struct proxycb_get_glyph_mapping_res_submapping *));
extern boolean FDECL(proxycb_xdr_get_glyph_mapping_res_mapping,
		(NhExtXdr *, struct proxycb_get_glyph_mapping_res_mapping *));
extern boolean FDECL(proxycb_xdr_get_glyph_mapping_res,
		(NhExtXdr *, struct proxycb_get_glyph_mapping_res *));
#endif  /* NHXDR_H */

#endif /* PROXYCB_H */
