/* $Id: proxycb.h,v 1.13 2003-01-18 17:52:08 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001-2002 */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef PROXYCB_H
#define PROXYCB_H

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
extern struct proxycb_get_valid_selections_res
		*NDECL(proxy_cb_get_valid_selections);
extern void FDECL(proxy_cb_free_valid_selections,
		(struct proxycb_get_valid_selections_res *vs));
extern void NDECL(proxy_cb_valid_selection_open);
extern int FDECL(proxy_cb_valid_selection_check, (int, int, int, int));
extern void NDECL(proxy_cb_valid_selection_close);
extern void NDECL(proxy_cb_quit_game);
extern void NDECL(proxy_cb_display_score);
extern void NDECL(proxy_cb_doset);
extern struct proxycb_get_extended_commands_res
		*NDECL(proxy_cb_get_extended_commands);
extern void FDECL(proxy_cb_free_extended_commands,
		(struct proxycb_get_extended_commands_res *commands));
extern int FDECL(proxy_cb_map_menu_cmd, (int));
extern int FDECL(proxy_cb_get_standard_winid, (char *));
extern struct proxycb_get_tilesets_res *NDECL(proxy_cb_get_tilesets);
extern void FDECL(proxy_cb_free_tilesets,
		(struct proxycb_get_tilesets_res *tilesets));
extern struct proxycb_get_glyph_mapping_res *NDECL(proxy_cb_get_glyph_mapping);
extern void FDECL(proxy_cb_free_glyph_mapping,
		(struct proxycb_get_glyph_mapping_res *mapping));
extern struct proxycb_get_extensions_res *NDECL(proxy_cb_get_extensions);
extern void FDECL(proxy_cb_free_extensions,
		(struct proxycb_get_extensions_res *extensions));

#endif /* PROXYCB_H */
