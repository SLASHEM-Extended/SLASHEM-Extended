/* $Id: proxycb.c,v 1.16 2003-05-31 08:12:44 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001-2002 */
/* NetHack may be freely redistributed.  See license for details. */

#include <stdlib.h>
#include <stdio.h>
#include "nhxdr.h"
#include "proxycom.h"
#include "proxycb.h"

void
proxy_cb_display_inventory()
{
    (void)nhext_rpc(EXT_CID_DISPLAY_INVENTORY, 0, 0);
}

int
proxy_cb_dlbh_fopen(name, mode)
const char *name, *mode;
{
    int retval;
    if (!nhext_rpc(EXT_CID_DLBH_FOPEN, 2, EXT_STRING(name), EXT_STRING(mode),
      1, EXT_INT_P(retval)))
	retval = -1;
    return retval;
}

char *
proxy_cb_dlbh_fgets(buf, len, fh)
char *buf;
int len, fh;
{
    char *retval, *line = (char *)0;
    if (!nhext_rpc(EXT_CID_DLBH_FGETS, 2, EXT_INT(len), EXT_INT(fh),
      1, EXT_STRING_P(line))) {
	free(line);
	return (char *)0;
    }
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
    if (!nhext_rpc(EXT_CID_DLBH_FREAD, 2, EXT_INT(size * no), EXT_INT(fh),
      2, EXT_INT_P(retval), EXT_BYTES_P(buffer, nb))) {
	free(buffer);
	return -1;
    }
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
	if (!nhext_rpc(EXT_CID_DLBH_FREAD,
	  2, EXT_INT(size * no - offset), EXT_INT(fh),
	  2, EXT_INT_P(retval), EXT_BYTES_P(buffer, nb)))
	    nb = 0;
	if (nb)
	    memcpy(buf + offset, buffer, nb);
	free(buffer);
    }
    return retval ? -1 : (offset + nb) / size;
}

int
proxy_cb_dlbh_fwrite(buf, size, no, fh)
char *buf;
int size, no, fh;
{
    int retval;
    if (!nhext_rpc(EXT_CID_DLBH_FWRITE,
      2, EXT_INT(fh), EXT_BYTES(buf, size * no),
      1, EXT_INT_P(retval)))
	retval = -1;
    return retval ? -1 : no;
}

int
proxy_cb_dlbh_fclose(fh)
int fh;
{
    int retval;
    if (!nhext_rpc(EXT_CID_DLBH_FCLOSE, 1, EXT_INT(fh), 1, EXT_INT_P(retval)))
	retval = -1;
    return retval;
}

void
proxy_cb_flush_screen()
{
    (void)nhext_rpc(EXT_CID_FLUSH_SCREEN, 0, 0);
}

void
proxy_cb_doredraw()
{
    (void)nhext_rpc(EXT_CID_DOREDRAW, 0, 0);
}

void
proxy_cb_interface_mode(mode)
unsigned long mode;
{
    (void)nhext_rpc(EXT_CID_INTERFACE_MODE, 1, EXT_LONG(mode), 0);
}

int
proxy_cb_parse_options(opts)
char *opts;
{
    int retval;
    if (!nhext_rpc(EXT_CID_PARSE_OPTIONS,
      1, EXT_STRING(opts), 1, EXT_INT_P(retval)))
	retval = -1;
    return retval;
}

char *
proxy_cb_get_option(opt)
char *opt;
{
    char *retval = (char *)0;
    if (!nhext_rpc(EXT_CID_GET_OPTION,
      1, EXT_STRING(opt), 1, EXT_STRING_P(retval))) {
	free(retval);
	return (char *)0;
    }
    if (!retval) {
	fprintf(stderr, "proxy_cb_get_option: retval is NULL\n");
	abort();
    }
    return retval;
}

struct proxycb_get_player_choices_res *
proxy_cb_get_player_choices()
{
    struct proxycb_get_player_choices_res *retval;
    retval=(struct proxycb_get_player_choices_res *)alloc(sizeof(*retval));
    memset(retval, 0, sizeof(*retval));
    if (!nhext_rpc(EXT_CID_GET_PLAYER_CHOICES, 0, 1,
      EXT_XDRF(proxycb_xdr_get_player_choices_res, retval))) {
	free(retval);
	return (struct proxycb_get_player_choices_res *)0;
    }
    return retval;
}

void
proxy_cb_free_player_choices(choices)
struct proxycb_get_player_choices_res *choices;
{
    nhext_xdr_free(proxycb_xdr_get_player_choices_res, (char *)choices);
    free(choices);
}

struct proxycb_get_valid_selections_res *
proxy_cb_get_valid_selections()
{
    struct proxycb_get_valid_selections_res *retval;
    retval=(struct proxycb_get_valid_selections_res *)alloc(sizeof(*retval));
    memset(retval, 0, sizeof(*retval));
    if (!nhext_rpc(EXT_CID_GET_VALID_SELECTIONS, 0, 1,
      EXT_XDRF(proxycb_xdr_get_valid_selections_res, retval))) {
	free(retval);
	return (struct proxycb_get_valid_selections_res *)0;
    }
    return retval;
}

void
proxy_cb_free_valid_selections(vs)
struct proxycb_get_valid_selections_res *vs;
{
    nhext_xdr_free(proxycb_xdr_get_valid_selections_res, (char *)vs);
    free(vs);
}

static struct proxycb_get_valid_selections_res *valid_selections;

void
proxy_cb_valid_selection_open()
{
    if (!valid_selections)
	valid_selections = proxy_cb_get_valid_selections();
}

/*
 * If s is negative, then iterate i over 0 <= i < n, otherwise do just one
 * iteration at i == s. Early exit as soon as we find a valid combination.
 */

#define ITERATE(i, s, n) for((i) = (s) >= 0 ? (s) : 0; \
			    !valid && ((s) >= 0 ? (i) == (s) : (i) < (n)); \
			    (i)++)

int
proxy_cb_valid_selection_check(role, race, gend, align)
int role, race, gend, align;
{
    int valid = 0;
    int pack;			/* No. masks packed in each element */
    int i, k, n;
    int irole, irace, igend, ialign;
    pack = 32 / valid_selections->no_genders;
    ITERATE(irole, role, valid_selections->no_roles)
	ITERATE(irace, race, valid_selections->no_races)
	    ITERATE(ialign, align, valid_selections->no_aligns) {
		n = (irole * valid_selections->no_races + irace) *
		  valid_selections->no_aligns + ialign;
		i = n / pack;
		k = (n % pack) * valid_selections->no_genders;
		ITERATE(igend, gend, valid_selections->no_genders)
		    if (valid_selections->masks[i] & 1L << k + igend)
			valid = 1;
	    }
    return valid;
}

#undef ITERATE

void
proxy_cb_valid_selection_close()
{
    if (valid_selections) {
	proxy_cb_free_valid_selections(valid_selections);
	valid_selections = (struct proxycb_get_valid_selections_res *)0;
    }
}

void
proxy_cb_quit_game()
{
    (void)nhext_rpc(EXT_CID_QUIT_GAME, 0, 0);
}

void
proxy_cb_display_score()
{
    (void)nhext_rpc(EXT_CID_DISPLAY_SCORE, 0, 0);
}

void
proxy_cb_doset()
{
    (void)nhext_rpc(EXT_CID_DOSET, 0, 0);
}

struct proxycb_get_extended_commands_res *
proxy_cb_get_extended_commands()
{
    struct proxycb_get_extended_commands_res *retval;
    retval=(struct proxycb_get_extended_commands_res *)alloc(sizeof(*retval));
    memset(retval, 0, sizeof(*retval));
    if (!nhext_rpc(EXT_CID_GET_EXTENDED_COMMANDS, 0, 1,
      EXT_XDRF(proxycb_xdr_get_extended_commands_res, retval))) {
	free(retval);
	return (struct proxycb_get_extended_commands_res *)0;
    }
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
    if (!nhext_rpc(EXT_CID_MAP_MENU_CMD, 1, EXT_INT(ch), 1, EXT_INT_P(retval)))
	retval = ch;
    return retval;
}

int
proxy_cb_get_standard_winid(window)
char *window;
{
    int retval;
    if (!nhext_rpc(EXT_CID_GET_STANDARD_WINID, 1, EXT_STRING(window),
      1, EXT_INT_P(retval)))
	retval = -1;
    return retval;
}

struct proxycb_get_tilesets_res *
proxy_cb_get_tilesets()
{
    struct proxycb_get_tilesets_res *retval;
    retval=(struct proxycb_get_tilesets_res *)alloc(sizeof(*retval));
    memset(retval, 0, sizeof(*retval));
    if (!nhext_rpc(EXT_CID_GET_TILESETS, 0, 1,
      EXT_XDRF(proxycb_xdr_get_tilesets_res, retval))) {
	free(retval);
	return (struct proxycb_get_tilesets_res *)0;
    }
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
    if (!nhext_rpc(EXT_CID_GET_GLYPH_MAPPING, 0, 1,
      EXT_XDRF(proxycb_xdr_get_glyph_mapping_res, retval))) {
	free(retval);
	retval = (struct proxycb_get_glyph_mapping_res *)0;
    }
    return retval;
}

void
proxy_cb_free_glyph_mapping(mapping)
struct proxycb_get_glyph_mapping_res *mapping;
{
    nhext_xdr_free(proxycb_xdr_get_glyph_mapping_res, (char *)mapping);
    free(mapping);
}

struct proxycb_get_extensions_res *
proxy_cb_get_extensions()
{
    struct proxycb_get_extensions_res *retval;
    retval=(struct proxycb_get_extensions_res *)alloc(sizeof(*retval));
    memset(retval, 0, sizeof(*retval));
    if (!nhext_rpc(EXT_CID_GET_EXTENSIONS, 0, 1,
      EXT_XDRF(proxycb_xdr_get_extensions_res, retval))) {
	free(retval);
	return (struct proxycb_get_extensions_res *)0;
    }
    return retval;
}

void
proxy_cb_free_extensions(extensions)
struct proxycb_get_extensions_res *extensions;
{
    nhext_xdr_free(proxycb_xdr_get_extensions_res, (char *)extensions);
    free(extensions);
}

void
proxy_cb_set_option_mod_status(optnam, status)
const char *optnam;
{
    (void)nhext_rpc(EXT_CID_SET_OPTION_MOD_STATUS, 2, EXT_STRING(optnam),
      EXT_INT(status), 0);
}
