/* $ Id: $ */
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

extern void NDECL(proxy_cb_display_inventory);
extern int FDECL(proxy_cb_dlbh_fopen, (char *, char *));
extern char *FDECL(proxy_cb_dlbh_fgets, (char *,int, int));
extern int FDECL(proxy_cb_dlbh_fclose, (int));
extern void NDECL(proxy_cb_flush_screen);
extern void NDECL(proxy_cb_doredraw);
extern void FDECL(proxy_cb_status_mode, (int));

#endif /* PROXYCB_H */
