/* $Id: proxycb.c,v 1.18 2003-10-25 18:06:01 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001-2003 */
/* NetHack may be freely redistributed.  See license for details. */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "nhxdr.h"
#include "proxycom.h"
#include "proxycb.h"

#ifndef SIZE
#define SIZE(array)	(sizeof(array) / sizeof(*(array)))
#endif

extern NhExtIO *proxy_clnt_log;

void
proxy_cb_display_inventory()
{
    unsigned short serial;
    if (proxy_clnt_log) {
	serial = nhext_rpc_get_next_serial();
	nhext_io_printf(proxy_clnt_log,
	  "rpc [%u] display_inventory()\n", serial);
    }
    (void)nhext_rpc(EXT_CID_DISPLAY_INVENTORY, 0, 0);
}

int
proxy_cb_dlbh_fopen(name, mode)
const char *name, *mode;
{
    int retval;
    unsigned short serial;
    if (proxy_clnt_log) {
	serial = nhext_rpc_get_next_serial();
	nhext_io_printf(proxy_clnt_log,
	  "rpc [%u] dlbh_fopen(\"%s\", \"%s\")\n", serial, name, mode);
    }
    if (!nhext_rpc(EXT_CID_DLBH_FOPEN, 2, EXT_STRING(name), EXT_STRING(mode),
      1, EXT_INT_P(retval)))
	retval = -1;
    if (proxy_clnt_log)
	nhext_io_printf(proxy_clnt_log, "rpc [%u] dlbh_fopen = %d\n",
	  serial, retval);
    return retval;
}

char *
proxy_cb_dlbh_fgets(buf, len, fh)
char *buf;
int len, fh;
{
    char *retval, *line = (char *)0;
    unsigned short serial;
    if (proxy_clnt_log) {
	serial = nhext_rpc_get_next_serial();
	nhext_io_printf(proxy_clnt_log,
	  "rpc [%u] dlbh_fgets(%d, %d)\n", serial, len, fh);
    }
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
    if (proxy_clnt_log) {
	if (retval)
	    nhext_io_printf(proxy_clnt_log, "rpc [%u] dlbh_fgets = \"%s\"\n",
	      serial, retval);
	else
	    nhext_io_printf(proxy_clnt_log, "rpc [%u] dlbh_fgets = NULL\n",
	      serial);
    }
    return retval;
}

int
proxy_cb_dlbh_fread(buf, size, no, fh)
char *buf;
int size, no, fh;
{
    int retval, offset = 0, nb;
    char *buffer = (char *)0;
    unsigned short serial;
    if (proxy_clnt_log) {
	serial = nhext_rpc_get_next_serial();
	nhext_io_printf(proxy_clnt_log,
	  "rpc [%u] dlbh_fread(%p, %d, %d, %d)\n", serial, buf, size, no, fh);
    }
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
    retval = retval ? -1 : (offset + nb) / size;
    if (proxy_clnt_log)
	nhext_io_printf(proxy_clnt_log, "rpc [%u] dlbh_fread = %d\n",
	  serial, retval);
    return retval;
}

int
proxy_cb_dlbh_fwrite(buf, size, no, fh)
char *buf;
int size, no, fh;
{
    int retval;
    unsigned short serial;
    if (proxy_clnt_log) {
	serial = nhext_rpc_get_next_serial();
	nhext_io_printf(proxy_clnt_log,
	  "rpc [%u] dlbh_fwrite(%p, %d, %d, %d)\n", serial, buf, size, no, fh);
    }
    if (!nhext_rpc(EXT_CID_DLBH_FWRITE,
      2, EXT_INT(fh), EXT_BYTES(buf, size * no),
      1, EXT_INT_P(retval)))
	retval = -1;
    if (proxy_clnt_log)
	nhext_io_printf(proxy_clnt_log, "rpc [%u] dlbh_fwrite = %d\n",
	  serial, retval ? -1 : no);
    return retval ? -1 : no;
}

int
proxy_cb_dlbh_fclose(fh)
int fh;
{
    int retval;
    unsigned short serial;
    if (proxy_clnt_log) {
	serial = nhext_rpc_get_next_serial();
	nhext_io_printf(proxy_clnt_log,
	  "rpc [%u] dlbh_fclose(%d)\n", serial, fh);
    }
    if (!nhext_rpc(EXT_CID_DLBH_FCLOSE, 1, EXT_INT(fh), 1, EXT_INT_P(retval)))
	retval = -1;
    if (proxy_clnt_log)
	nhext_io_printf(proxy_clnt_log, "rpc [%u] dlbh_fclose = %d\n",
	  serial, retval);
    return retval;
}

char *
proxy_cb_dlbh_fmd5sum(name)
const char *name;
{
    int retval;
    char *digest = (char *)0;
    unsigned short serial;
    if (proxy_clnt_log) {
	serial = nhext_rpc_get_next_serial();
	nhext_io_printf(proxy_clnt_log,
	  "rpc [%u] dlbh_fmd5sum(\"%s\")\n", serial, name);
    }
    if (!nhext_rpc(EXT_CID_DLBH_FMD5SUM, 1, EXT_STRING(name),
      2, EXT_INT_P(retval), EXT_STRING_P(digest)))
	retval = -1;
    if (!retval) {
	if (proxy_clnt_log)
	    nhext_io_printf(proxy_clnt_log, "rpc [%u] dlbh_fmd5sum = \"%s\"\n",
	      serial, digest);
	return digest;
    } else {
	free(digest);
	if (proxy_clnt_log)
	    nhext_io_printf(proxy_clnt_log, "rpc [%u] dlbh_fmd5sum = NULL\n",
	      serial);
	return (char *)0;
    }
}

void
proxy_cb_flush_screen()
{
    unsigned short serial;
    if (proxy_clnt_log) {
	serial = nhext_rpc_get_next_serial();
	nhext_io_printf(proxy_clnt_log,
	  "rpc [%u] flush_screen()\n", serial);
    }
    (void)nhext_rpc(EXT_CID_FLUSH_SCREEN, 0, 0);
}

void
proxy_cb_doredraw()
{
    unsigned short serial;
    if (proxy_clnt_log) {
	serial = nhext_rpc_get_next_serial();
	nhext_io_printf(proxy_clnt_log,
	  "rpc [%u] doredraw()\n", serial);
    }
    (void)nhext_rpc(EXT_CID_DOREDRAW, 0, 0);
}

void
proxy_cb_interface_mode(mode)
unsigned long mode;
{
    unsigned short serial;
    if (proxy_clnt_log) {
	unsigned long m = mode;
	serial = nhext_rpc_get_next_serial();
	nhext_io_printf(proxy_clnt_log,
	  "rpc [%u] interface_mode(", serial);
	if (m & EXT_IM_STATUS) {
	    nhext_io_printf(proxy_clnt_log, "EXT_IM_STATUS");
	    m &= ~EXT_IM_STATUS;
	}
	if (m & EXT_IM_DISPLAY_LAYERS) {
	    if (m != mode)
		nhext_io_printf(proxy_clnt_log, " | ");
	    nhext_io_printf(proxy_clnt_log, "EXT_IM_DISPLAY_LAYERS");
	    m &= ~EXT_IM_DISPLAY_LAYERS;
	}
	if (m || m == mode) {
	    if (m != mode)
		nhext_io_printf(proxy_clnt_log, " | ");
	    nhext_io_printf(proxy_clnt_log, "%lu", m);
	}
	nhext_io_printf(proxy_clnt_log, ")\n");
    }
    (void)nhext_rpc(EXT_CID_INTERFACE_MODE, 1, EXT_LONG(mode), 0);
}

int
proxy_cb_parse_options(opts)
char *opts;
{
    int retval;
    unsigned short serial;
    if (proxy_clnt_log) {
	serial = nhext_rpc_get_next_serial();
	nhext_io_printf(proxy_clnt_log,
	  "rpc [%u] parse_options(\"%s\")\n", serial, opts);
    }
    if (!nhext_rpc(EXT_CID_PARSE_OPTIONS,
      1, EXT_STRING(opts), 1, EXT_INT_P(retval)))
	retval = -1;
    if (proxy_clnt_log)
	nhext_io_printf(proxy_clnt_log, "rpc [%u] parse_options = %d\n",
	  serial, retval);
    return retval;
}

char *
proxy_cb_get_option(opt)
char *opt;
{
    char *retval = (char *)0;
    unsigned short serial;
    if (proxy_clnt_log) {
	serial = nhext_rpc_get_next_serial();
	nhext_io_printf(proxy_clnt_log,
	  "rpc [%u] get_option(\"%s\")\n", serial, opt);
    }
    if (!nhext_rpc(EXT_CID_GET_OPTION,
      1, EXT_STRING(opt), 1, EXT_STRING_P(retval))) {
	free(retval);
	if (proxy_clnt_log)
	    nhext_io_printf(proxy_clnt_log, "rpc [%u] get_option = NULL\n",
	      serial);
	return (char *)0;
    }
    if (proxy_clnt_log)
	nhext_io_printf(proxy_clnt_log, "rpc [%u] get_option = \"%s\"\n",
	  serial, retval);
    return retval;
}

struct proxycb_get_player_choices_res *
proxy_cb_get_player_choices()
{
    struct proxycb_get_player_choices_res *retval;
    unsigned short serial;
    if (proxy_clnt_log) {
	serial = nhext_rpc_get_next_serial();
	nhext_io_printf(proxy_clnt_log,
	  "rpc [%u] get_player_choices()\n", serial);
    }
    retval=(struct proxycb_get_player_choices_res *)alloc(sizeof(*retval));
    memset(retval, 0, sizeof(*retval));
    if (!nhext_rpc(EXT_CID_GET_PLAYER_CHOICES, 0, 1,
      EXT_XDRF(proxycb_xdr_get_player_choices_res, retval))) {
	free(retval);
	if (proxy_clnt_log)
	    nhext_io_printf(proxy_clnt_log,
	      "rpc [%u] get_player_choices = NULL\n", serial);
	return (struct proxycb_get_player_choices_res *)0;
    }
    if (proxy_clnt_log) {
	int i;
	nhext_io_printf(proxy_clnt_log,
	  "rpc [%u] get_player_choices = <", serial);
	for(i = 0; i < retval->n_aligns; i++) {
	    if (i)
		nhext_io_printf(proxy_clnt_log, ", ");
	    nhext_io_printf(proxy_clnt_log, "\"%s\"", retval->aligns[i]);
	}
	nhext_io_printf(proxy_clnt_log, ">, <");
	for(i = 0; i < retval->n_genders; i++) {
	    if (i)
		nhext_io_printf(proxy_clnt_log, ", ");
	    nhext_io_printf(proxy_clnt_log, "\"%s\"", retval->genders[i]);
	}
	nhext_io_printf(proxy_clnt_log, ">, <");
	for(i = 0; i < retval->n_races; i++) {
	    if (i)
		nhext_io_printf(proxy_clnt_log, ", ");
	    nhext_io_printf(proxy_clnt_log, "\"%s\"", retval->races[i]);
	}
	nhext_io_printf(proxy_clnt_log, ">, <");
	for(i = 0; i < retval->n_roles; i++) {
	    if (i)
		nhext_io_printf(proxy_clnt_log, ", ");
	    nhext_io_printf(proxy_clnt_log, "{\"%s\", \"%s\"}",
	      retval->roles[i].male, retval->roles[i].female);
	}
	nhext_io_printf(proxy_clnt_log, ">\n");
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
    unsigned short serial;
    if (proxy_clnt_log) {
	serial = nhext_rpc_get_next_serial();
	nhext_io_printf(proxy_clnt_log,
	  "rpc [%u] get_valid_selections()\n", serial);
    }
    retval=(struct proxycb_get_valid_selections_res *)alloc(sizeof(*retval));
    memset(retval, 0, sizeof(*retval));
    if (!nhext_rpc(EXT_CID_GET_VALID_SELECTIONS, 0, 1,
      EXT_XDRF(proxycb_xdr_get_valid_selections_res, retval))) {
	free(retval);
	if (proxy_clnt_log)
	    nhext_io_printf(proxy_clnt_log,
	      "rpc [%u] get_valid_selections = NULL\n", serial);
	return (struct proxycb_get_valid_selections_res *)0;
    }
    if (proxy_clnt_log) {
	int i;
	nhext_io_printf(proxy_clnt_log,
	  "rpc [%u] get_valid_selections = %d, %d, %d, %d, <", serial,
	  retval->no_roles, retval->no_races, retval->no_aligns,
	  retval->no_genders);
	for(i = 0; i < retval->n_masks; i++) {
	    if (i)
		nhext_io_printf(proxy_clnt_log, ", ");
	    nhext_io_printf(proxy_clnt_log, "0x%08lX", retval->masks[i]);
	}
	nhext_io_printf(proxy_clnt_log, ">\n");
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
    unsigned short serial;
    if (proxy_clnt_log) {
	serial = nhext_rpc_get_next_serial();
	nhext_io_printf(proxy_clnt_log,
	  "rpc [%u] quit_game()\n", serial);
    }
    (void)nhext_rpc(EXT_CID_QUIT_GAME, 0, 0);
}

void
proxy_cb_display_score()
{
    unsigned short serial;
    if (proxy_clnt_log) {
	serial = nhext_rpc_get_next_serial();
	nhext_io_printf(proxy_clnt_log,
	  "rpc [%u] display_score()\n", serial);
    }
    (void)nhext_rpc(EXT_CID_DISPLAY_SCORE, 0, 0);
}

void
proxy_cb_doset()
{
    unsigned short serial;
    if (proxy_clnt_log) {
	serial = nhext_rpc_get_next_serial();
	nhext_io_printf(proxy_clnt_log,
	  "rpc [%u] doset()\n", serial);
    }
    (void)nhext_rpc(EXT_CID_DOSET, 0, 0);
}

struct proxycb_get_extended_commands_res *
proxy_cb_get_extended_commands()
{
    struct proxycb_get_extended_commands_res *retval;
    unsigned short serial;
    if (proxy_clnt_log) {
	serial = nhext_rpc_get_next_serial();
	nhext_io_printf(proxy_clnt_log,
	  "rpc [%u] get_extended_commands()\n", serial);
    }
    retval=(struct proxycb_get_extended_commands_res *)alloc(sizeof(*retval));
    memset(retval, 0, sizeof(*retval));
    if (!nhext_rpc(EXT_CID_GET_EXTENDED_COMMANDS, 0, 1,
      EXT_XDRF(proxycb_xdr_get_extended_commands_res, retval))) {
	free(retval);
	if (proxy_clnt_log)
	    nhext_io_printf(proxy_clnt_log,
	      "rpc [%u] get_extended_commands = NULL\n", serial);
	return (struct proxycb_get_extended_commands_res *)0;
    }
    if (proxy_clnt_log) {
	int i;
	nhext_io_printf(proxy_clnt_log,
	  "rpc [%u] get_extended_commands = <", serial);
	for(i = 0; i < retval->n_commands; i++) {
	    if (i)
		nhext_io_printf(proxy_clnt_log, ", ");
	    nhext_io_printf(proxy_clnt_log, "\"%s\"", retval->commands[i]);
	}
	nhext_io_printf(proxy_clnt_log, ">\n");
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
    unsigned short serial;
    if (proxy_clnt_log) {
	serial = nhext_rpc_get_next_serial();
	nhext_io_printf(proxy_clnt_log,
	  "rpc [%u] map_menu_cmd(%d)\n", serial, ch);
    }
    if (!nhext_rpc(EXT_CID_MAP_MENU_CMD, 1, EXT_INT(ch), 1, EXT_INT_P(retval)))
	retval = ch;
    if (proxy_clnt_log)
	nhext_io_printf(proxy_clnt_log, "rpc [%u] map_menu_cmd = %d\n",
	  serial, retval);
    return retval;
}

int
proxy_cb_get_standard_winid(window)
char *window;
{
    int retval;
    unsigned short serial;
    if (proxy_clnt_log) {
	serial = nhext_rpc_get_next_serial();
	nhext_io_printf(proxy_clnt_log,
	  "rpc [%u] get_standard_winid(\"%s\")\n", serial, window);
    }
    if (!nhext_rpc(EXT_CID_GET_STANDARD_WINID, 1, EXT_STRING(window),
      1, EXT_INT_P(retval)))
	retval = -1;
    if (proxy_clnt_log)
	nhext_io_printf(proxy_clnt_log, "rpc [%u] get_standard_winid = %d\n",
	  serial, retval);
    return retval;
}

struct proxycb_get_tilesets_res *
proxy_cb_get_tilesets()
{
    struct proxycb_get_tilesets_res *retval;
    unsigned short serial;
    if (proxy_clnt_log) {
	serial = nhext_rpc_get_next_serial();
	nhext_io_printf(proxy_clnt_log,
	  "rpc [%u] get_tilesets()\n", serial);
    }
    retval=(struct proxycb_get_tilesets_res *)alloc(sizeof(*retval));
    memset(retval, 0, sizeof(*retval));
    if (!nhext_rpc(EXT_CID_GET_TILESETS, 0, 1,
      EXT_XDRF(proxycb_xdr_get_tilesets_res, retval))) {
	free(retval);
	if (proxy_clnt_log)
	    nhext_io_printf(proxy_clnt_log, "rpc [%u] get_tilesets = NULL\n",
	      serial);
	return (struct proxycb_get_tilesets_res *)0;
    }
    if (proxy_clnt_log) {
	int i;
	nhext_io_printf(proxy_clnt_log, "rpc [%u] get_tilesets = <", serial);
	for(i = 0; i < retval->n_tilesets; i++) {
	    if (i)
		nhext_io_printf(proxy_clnt_log, ", ");
	    nhext_io_printf(proxy_clnt_log, "{\"%s\",\"%s\",\"%s\",%ld}",
	      retval->tilesets[i].name, retval->tilesets[i].file,
	      retval->tilesets[i].mapfile, retval->tilesets[i].flags);
	}
	nhext_io_printf(proxy_clnt_log, ">\n");
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
    unsigned short serial;
    if (proxy_clnt_log) {
	serial = nhext_rpc_get_next_serial();
	nhext_io_printf(proxy_clnt_log,
	  "rpc [%u] get_glyph_mapping()\n", serial);
    }
    retval=(struct proxycb_get_glyph_mapping_res *)alloc(sizeof(*retval));
    memset(retval, 0, sizeof(*retval));
    if (!nhext_rpc(EXT_CID_GET_GLYPH_MAPPING, 0, 1,
      EXT_XDRF(proxycb_xdr_get_glyph_mapping_res, retval))) {
	free(retval);
	if (proxy_clnt_log)
	    nhext_io_printf(proxy_clnt_log,
	      "rpc [%u] get_glpyh_mapping = NULL\n", serial);
	retval = (struct proxycb_get_glyph_mapping_res *)0;
    }
    if (proxy_clnt_log) {
	int i, j, k;
	struct proxycb_get_glyph_mapping_res_submapping *sm;
	nhext_io_printf(proxy_clnt_log,
	  "rpc [%u] get_glpyh_mapping = %d, 0x%08lX, <\n  ",
	  serial, retval->no_glyph, retval->transparent);
	for(i = 0; i < retval->n_mappings; i++) {
	    if (i)
		nhext_io_printf(proxy_clnt_log, ",\n  ");
	    nhext_io_printf(proxy_clnt_log,
	      "{\"%s\", %d, %d, {0x%08lX, \"%s\"}, <",
	      retval->mappings[i].flags, retval->mappings[i].base_mapping,
	      retval->mappings[i].alt_glyph, retval->mappings[i].symdef.rgbsym,
	      retval->mappings[i].symdef.description);
	    for(j = 0; j < retval->mappings[i].n_submappings; j++) {
		if (j)
		    nhext_io_fputc(',', proxy_clnt_log);
		nhext_io_printf(proxy_clnt_log, "\n    ");
		sm = retval->mappings[i].submappings + j;
		nhext_io_printf(proxy_clnt_log, "{{0x%08lX, \"%s\"}, <",
		  sm->symdef.rgbsym, sm->symdef.description);
		for(k = 0; k < sm->n_glyphs; k++) {
		    if (k)
			nhext_io_printf(proxy_clnt_log, ", ");
		    nhext_io_printf(proxy_clnt_log, "{0x%08lX, \"%s\"}",
		      sm->glyphs[k].rgbsym, sm->glyphs[k].description);
		}
		nhext_io_printf(proxy_clnt_log, ">}");
	    }
	    nhext_io_printf(proxy_clnt_log, "\n    >}");
	}
	nhext_io_printf(proxy_clnt_log, "\n  >\n");
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
    unsigned short serial;
    if (proxy_clnt_log) {
	serial = nhext_rpc_get_next_serial();
	nhext_io_printf(proxy_clnt_log,
	  "rpc [%u] get_extentions()\n", serial);
    }
    retval=(struct proxycb_get_extensions_res *)alloc(sizeof(*retval));
    memset(retval, 0, sizeof(*retval));
    if (!nhext_rpc(EXT_CID_GET_EXTENSIONS, 0, 1,
      EXT_XDRF(proxycb_xdr_get_extensions_res, retval))) {
	free(retval);
	if (proxy_clnt_log)
	    nhext_io_printf(proxy_clnt_log,
	      "rpc [%u] get_extentions = NULL\n", serial);
	return (struct proxycb_get_extensions_res *)0;
    }
    if (proxy_clnt_log) {
	int i;
	nhext_io_printf(proxy_clnt_log,
	  "rpc [%u] get_extentions = <\n", serial);
	for(i = 0; i < retval->n_extensions; i++) {
	    if (i)
		nhext_io_printf(proxy_clnt_log, ", ");
	    nhext_io_printf(proxy_clnt_log, "{\"%s\", \"%s\", %d}",
	      retval->extensions[i].name, retval->extensions[i].version,
	      retval->extensions[i].no_procedures);
	}
	nhext_io_printf(proxy_clnt_log, ">\n");
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
    unsigned short serial;
    static const char *option_mod_flags[] = { "SET_IN_FILE", "SET_VIA_PROG",
      "DISP_IN_GAME", "SET_IN_GAME" };
    if (proxy_clnt_log) {
	serial = nhext_rpc_get_next_serial();
	if (status >= 0 && status < SIZE(option_mod_flags))
	    nhext_io_printf(proxy_clnt_log,
	      "rpc [%u] set_option_mod_status(\"%s\", %s)\n",
	      serial, optnam, option_mod_flags[status]);
	else
	    nhext_io_printf(proxy_clnt_log,
	      "rpc [%u] set_option_mod_status(\"%s\", %d)\n",
	      serial, optnam, status);
    }
    (void)nhext_rpc(EXT_CID_SET_OPTION_MOD_STATUS, 2, EXT_STRING(optnam),
      EXT_INT(status), 0);
}
