/* $Id: proxycb.h,v 1.4 2002-06-23 15:32:20 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001-2002 */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef PROXYCB_H
#define PROXYCB_H

#define EXT_CID_DISPLAY_INVENTORY	0x01
#define EXT_CID_DLBH_FOPEN		0x02
#define EXT_CID_DLBH_FGETS		0x03
#define EXT_CID_DLBH_FCLOSE		0x04
#define EXT_CID_FLUSH_SCREEN		0x05
#define EXT_CID_DOREDRAW		0x06
#define EXT_CID_STATUS_MODE		0x07
#define EXT_CID_PARSE_OPTIONS		0x08
#define EXT_CID_GET_OPTION		0x09
#define EXT_CID_GET_PLAYER_CHOICES	0x10
#define EXT_CID_IS_VALID_SELECTION	0x11

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

extern void NDECL(proxy_cb_display_inventory);
extern int FDECL(proxy_cb_dlbh_fopen, (char *, char *));
extern char *FDECL(proxy_cb_dlbh_fgets, (char *, int, int));
extern int FDECL(proxy_cb_dlbh_fclose, (int));
extern void NDECL(proxy_cb_flush_screen);
extern void NDECL(proxy_cb_doredraw);
extern void FDECL(proxy_cb_status_mode, (int));
extern int FDECL(proxy_cb_parse_options, (char *));
extern char *FDECL(proxy_cb_get_option, (char *));
extern int FDECL(proxy_cb_is_valid_selection, (int, int, int, int));
extern struct proxycb_get_player_choices_res
		*NDECL(proxy_cb_get_player_choices);
extern void FDECL(proxy_cb_free_player_choices,
		(struct proxycb_get_player_choices_res *choices));

#ifdef NHXDR_H
extern boolean FDECL(proxycb_xdr_get_player_choices_res_role,
		(NhExtXdr *, struct proxycb_get_player_choices_res_role *));
extern boolean FDECL(proxycb_xdr_get_player_choices_res,
		(NhExtXdr *, struct proxycb_get_player_choices_res *));
#endif  /* NHXDR_H */

#endif /* PROXYCB_H */
