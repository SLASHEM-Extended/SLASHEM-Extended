/* $Id: proxycb.h,v 1.16 2003-10-25 18:06:00 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001-2003 */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef PROXYCB_H
#define PROXYCB_H

extern void proxy_cb_display_inventory(void);
extern int proxy_cb_dlbh_fopen(const char *, const char *);
extern char *proxy_cb_dlbh_fgets(char *, int, int);
extern int proxy_cb_dlbh_fread(char *, int, int, int);
extern int proxy_cb_dlbh_fwrite(char *, int, int, int);
extern int proxy_cb_dlbh_fclose(int);
extern char *proxy_cb_dlbh_fmd5sum(const char *);
extern void proxy_cb_flush_screen(void);
extern void proxy_cb_doredraw(void);
extern void proxy_cb_interface_mode(unsigned long);
extern int proxy_cb_parse_options(char *);
extern char *proxy_cb_get_option(char *);
extern struct proxycb_get_player_choices_res
		*proxy_cb_get_player_choices(void);
extern void proxy_cb_free_player_choices(struct proxycb_get_player_choices_res *choices);
extern struct proxycb_get_valid_selections_res
		*proxy_cb_get_valid_selections(void);
extern void proxy_cb_free_valid_selections(struct proxycb_get_valid_selections_res *vs);
extern void proxy_cb_valid_selection_open(void);
extern int proxy_cb_valid_selection_check(int, int, int, int);
extern void proxy_cb_valid_selection_close(void);
extern void proxy_cb_quit_game(void);
extern void proxy_cb_display_score(void);
extern void proxy_cb_doset(void);
extern struct proxycb_get_extended_commands_res
		*proxy_cb_get_extended_commands(void);
extern void proxy_cb_free_extended_commands(struct proxycb_get_extended_commands_res *commands);
extern int proxy_cb_map_menu_cmd(int);
extern int proxy_cb_get_standard_winid(char *);
extern struct proxycb_get_tilesets_res *proxy_cb_get_tilesets(void);
extern void proxy_cb_free_tilesets(struct proxycb_get_tilesets_res *tilesets);
extern struct proxycb_get_glyph_mapping_res *proxy_cb_get_glyph_mapping(void);
extern void proxy_cb_free_glyph_mapping(struct proxycb_get_glyph_mapping_res *mapping);
extern struct proxycb_get_extensions_res *proxy_cb_get_extensions(void);
extern void proxy_cb_free_extensions(struct proxycb_get_extensions_res *extensions);
extern unsigned long *proxy_cb_start_async_mode(int, unsigned long *async_callbacks, int *);

#endif /* PROXYCB_H */
