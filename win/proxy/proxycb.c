/* $Id: proxycb.c,v 1.9 2002-09-01 21:58:19 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001-2002 */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "nhxdr.h"
#include "winproxy.h"
#include "proxycb.h"

extern int proxy_svc_connection;

/*
 * Warning: This uses a gcc extension. The assumption is that we're going to
 * remove the connection number before release anyway, so it's easier not to
 * add a new parameter to every call to next_rpc().
 */

#define nhext_rpc(id, args...) nhext_rpc_c(proxy_svc_connection, id, args)

void
proxy_cb_display_inventory()
{
    nhext_rpc(EXT_CID_DISPLAY_INVENTORY, 0, 0);
}

int
proxy_cb_dlbh_fopen(name, mode)
const char *name, *mode;
{
    int retval;
    nhext_rpc(EXT_CID_DLBH_FOPEN, 2, EXT_STRING(name), EXT_STRING(mode),
	1, EXT_INT_P(retval));
    return retval;
}

char *
proxy_cb_dlbh_fgets(buf, len, fh)
char *buf;
int len, fh;
{
    char *retval, *line = (char *)0;
    nhext_rpc(EXT_CID_DLBH_FGETS, 2, EXT_INT(len), EXT_INT(fh),
	1, EXT_STRING_P(line));
    if (*line) {
	strncpy(buf, line, len - 1);
	buf[len - 1] = '\0';
	retval = buf;
    }
    else
	retval = (char *)0;
    free(line);
    return retval;
}

int
proxy_cb_dlbh_fread(buf, size, no, fh)
char *buf;
int size, no, fh;
{
    int retval, offset = 0, nb;
    char *buffer = (char *)0;
    nhext_rpc(EXT_CID_DLBH_FREAD, 2, EXT_INT(size * no), EXT_INT(fh),
	2, EXT_INT_P(retval), EXT_BYTES_P(buffer, nb));
    if (nb)
	memcpy(buf, buffer, nb);
    free(buffer);
    while (!retval && nb % size) {
	/*
	 * The dblh_fread() callback has no concept of item sizes
	 * (it acts more like read() in this respect than fread()).
	 * If we get a sucessful read which is short then we shouldn't
	 * discard the remainder unless we have reached end of file.
	 */
	offset += nb;
	nhext_rpc(EXT_CID_DLBH_FREAD, 2, EXT_INT(size * no - offset), EXT_INT(fh),
	    2, EXT_INT_P(retval), EXT_BYTES_P(buffer, nb));
	if (nb)
	    memcpy(buf + offset, buffer, nb);
	free(buffer);
    }
    return retval ? -1 : (offset + nb) / size;
}

int
proxy_cb_dlbh_fclose(fh)
int fh;
{
    int retval;
    nhext_rpc(EXT_CID_DLBH_FCLOSE, 1, EXT_INT(fh), 1, EXT_INT_P(retval));
    return retval;
}

void
proxy_cb_flush_screen()
{
    nhext_rpc(EXT_CID_FLUSH_SCREEN, 0, 0);
}

void
proxy_cb_doredraw()
{
    nhext_rpc(EXT_CID_DOREDRAW, 0, 0);
}

void
proxy_cb_interface_mode(mode)
unsigned long mode;
{
    nhext_rpc(EXT_CID_INTERFACE_MODE, 1, EXT_LONG(mode), 0);
}

int
proxy_cb_parse_options(opts)
char *opts;
{
    int retval;
    nhext_rpc(EXT_CID_PARSE_OPTIONS, 1, EXT_STRING(opts), 1, EXT_INT_P(retval));
    return retval;
}

char *
proxy_cb_get_option(opt)
char *opt;
{
    char *retval = (char *)0;
    nhext_rpc(EXT_CID_GET_OPTION, 1, EXT_STRING(opt), 1, EXT_STRING_P(retval));
    return retval;
}

struct proxycb_get_player_choices_res *
proxy_cb_get_player_choices()
{
    struct proxycb_get_player_choices_res *retval;
    retval=(struct proxycb_get_player_choices_res *)alloc(sizeof(*retval));
    memset(retval, 0, sizeof(*retval));
    nhext_rpc(EXT_CID_GET_PLAYER_CHOICES, 0, 1,
      EXT_XDRF(proxycb_xdr_get_player_choices_res, retval));
    return retval;
}

void
proxy_cb_free_player_choices(choices)
struct proxycb_get_player_choices_res *choices;
{
    nhext_xdr_free(proxycb_xdr_get_player_choices_res, (char *)choices);
    free(choices);
}

int
proxy_cb_is_valid_selection(role, race, gender, alignment)
int role, race, gender, alignment;
{
    int retval;
    nhext_rpc(EXT_CID_IS_VALID_SELECTION,
      4, EXT_INT(role), EXT_INT(race), EXT_INT(gender), EXT_INT(alignment),
      1, EXT_INT_P(retval));
    return retval;
}

void
proxy_cb_quit_game()
{
    nhext_rpc(EXT_CID_QUIT_GAME, 0, 0);
}

void
proxy_cb_display_score()
{
    nhext_rpc(EXT_CID_DISPLAY_SCORE, 0, 0);
}

void
proxy_cb_doset()
{
    nhext_rpc(EXT_CID_DOSET, 0, 0);
}

struct proxycb_get_extended_commands_res *
proxy_cb_get_extended_commands()
{
    struct proxycb_get_extended_commands_res *retval;
    retval=(struct proxycb_get_extended_commands_res *)alloc(sizeof(*retval));
    memset(retval, 0, sizeof(*retval));
    nhext_rpc(EXT_CID_GET_EXTENDED_COMMANDS, 0, 1,
      EXT_XDRF(proxycb_xdr_get_extended_commands_res, retval));
    return retval;
}

void
proxy_cb_free_extended_commands(commands)
struct proxycb_get_extended_commands_res *commands;
{
    nhext_xdr_free(proxycb_xdr_get_extended_commands_res, (char *)commands);
    free(commands);
}

int
proxy_cb_map_menu_cmd(ch)
int ch;
{
    int retval;
    nhext_rpc(EXT_CID_MAP_MENU_CMD, 1, EXT_INT(ch), 1, EXT_INT_P(retval));
    return retval;
}

winid
proxy_cb_get_standard_winid(window)
char *window;
{
    winid retval;
    nhext_rpc(EXT_CID_GET_STANDARD_WINID, 1, EXT_STRING(window),
      1, EXT_WINID_P(retval));
    return retval;
}

struct proxycb_get_tilesets_res *
proxy_cb_get_tilesets()
{
    struct proxycb_get_tilesets_res *retval;
    retval=(struct proxycb_get_tilesets_res *)alloc(sizeof(*retval));
    memset(retval, 0, sizeof(*retval));
    nhext_rpc(EXT_CID_GET_TILESETS, 0, 1,
      EXT_XDRF(proxycb_xdr_get_tilesets_res, retval));
    return retval;
}

void
proxy_cb_free_tilesets(tilesets)
struct proxycb_get_tilesets_res *tilesets;
{
    nhext_xdr_free(proxycb_xdr_get_tilesets_res, (char *)tilesets);
    free(tilesets);
}

struct proxycb_get_glyph_mapping_res *
proxy_cb_get_glyph_mapping()
{
    struct proxycb_get_glyph_mapping_res *retval;
    retval=(struct proxycb_get_glyph_mapping_res *)alloc(sizeof(*retval));
    memset(retval, 0, sizeof(*retval));
    nhext_rpc(EXT_CID_GET_GLYPH_MAPPING, 0, 1,
      EXT_XDRF(proxycb_xdr_get_glyph_mapping_res, retval));
    return retval;
}

void
proxy_cb_free_glyph_mapping(mapping)
struct proxycb_get_glyph_mapping_res *mapping;
{
    nhext_xdr_free(proxycb_xdr_get_glyph_mapping_res, (char *)mapping);
    free(mapping);
}
