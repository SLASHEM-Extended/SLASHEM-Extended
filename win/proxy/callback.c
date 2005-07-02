/* $Id: callback.c,v 1.28 2005-02-02 09:42:13 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001-2004 */
/* NetHack may be freely redistributed.  See license for details. */

/* #define DEBUG */

#include <nhproxy/nhproxy.h>
#include "hack.h"
#include "func_tab.h"
#include "winproxy.h"

static void
cb_display_inventory()
{
    display_inventory((char *)0, FALSE);
}

#ifndef FILE_AREAS
#define SET_FILE(f, a) if (1) { file = f; } else
#else
#define SET_FILE(f, a) if (1) { file = f; area = a; } else
#endif

static int
cb_dlbh_fopen(path, mode)
const char *path, *mode;
{
    const char *file, *subname;
#ifdef FILE_AREAS
    const char *area;
#endif
    char *s;
    char *buf = NULL;
    char *name = NULL;
    int i, retval = 0;
    if (path[0] != '$' || path[1] != '(')
	retval = -1;
    else {
	for(i = 0; path[i] && path[i] != ')'; i++)
	    ;
	if (!path[i])
	    retval = -1;
	else {
	    name = (char *)alloc(i + 1);
	    memcpy(name, path, i);
	    name[i] = '\0';
	    subname = path + i + 1;
	    if (*subname=='/')
		subname++;
	    else if (*subname)
		retval = -1;
	}
    }
    if (strcmp(mode, "r") && strcmp(mode, "rb"))
	retval = -1;
    if (!retval && *subname) {
	if (!strcmp(name + 2, "TILEDIR") || !strcmp(name + 2, "TILEMAPDIR")) {
#ifdef MAXNOTILESETS
	    for(i = 0; i < no_tilesets; i++)
		if (!strcmp(tilesets[i].file, subname))
		    break;
	    if (i == no_tilesets)
		retval = -1;
	    else
#else
	    if (strcmp(tile_file, subname))
		retval = -1;
	    else
#endif
	    if (!strcmp(name + 2, "TILEDIR"))
		SET_FILE(subname, FILE_AREA_SHARE);
	    else {
		s = strrchr(subname, '.');
		if (s) {
		    buf = (char *)alloc(s - subname + 5);
		    (void)strncpy(buf, subname, s - subname);
		    buf[s - subname] = '\0';
		} else {
		    buf = (char *)alloc(strlen(subname) + 5);
		    Strcpy(buf, subname);
		}
		Strcat(buf, ".map");
		SET_FILE(buf, FILE_AREA_SHARE);
	    }
	} else
	    retval = -1;
    } else if (!retval) {
	if (!strcmp(name + 2, "RECORD"))
	    SET_FILE(NH_RECORD, NH_RECORD_AREA);
	else if (!strcmp(name + 2, "HELP"))
	    SET_FILE(NH_HELP, NH_HELP_AREA);
	else if (!strcmp(name + 2, "SHELP"))
	    SET_FILE(NH_SHELP, NH_SHELP_AREA);
	else if (!strcmp(name + 2, "DEBUGHELP"))
	    SET_FILE(NH_DEBUGHELP, NH_DEBUGHELP_AREA);
#if 0
	else if (!strcmp(name + 2, "RUMORFILE"))
	    SET_FILE(NH_RUMORFILE, NH_RUMORAREA);
	else if (!strcmp(name + 2, "ORACLEFILE"))
	    SET_FILE(NH_ORACLEFILE, NH_ORACLEAREA);
#endif
	else if (!strcmp(name + 2, "DATAFILE"))
	    SET_FILE(NH_DATAFILE, NH_DATAAREA);
	else if (!strcmp(name + 2, "CMDHELPFILE"))
	    SET_FILE(NH_CMDHELPFILE, NH_CMDHELPAREA);
	else if (!strcmp(name + 2, "HISTORY"))
	    SET_FILE(NH_HISTORY, NH_HISTORY_AREA);
	else if (!strcmp(name + 2, "LICENSE"))
	    SET_FILE(NH_LICENSE, NH_LICENSE_AREA);
	else if (!strcmp(name + 2, "OPTIONFILE"))
	    SET_FILE(NH_OPTIONFILE, NH_OPTIONAREA);
	else if (!strcmp(name + 2, "OPTIONS_USED"))
	    SET_FILE(NH_OPTIONS_USED, NH_OPTIONS_USED_AREA);
	else if (!strcmp(name + 2, "GUIDEBOOK"))
	    SET_FILE(NH_GUIDEBOOK, NH_GUIDEBOOK_AREA);
	else
	    retval = -1;
    }
    if (!retval) {
	dlb_init();
#ifndef FILE_AREAS
	retval = dlbh_fopen(file, mode);
#else
	retval = dlbh_fopen_area(area, file, mode);
#endif
    }
    if (buf)
	free(buf);
    free(name);
    return retval;
}

#undef SET_FILE

static void
cb_flush_screen()
{
    extern int proxy_curs_on_u;
    flush_screen(proxy_curs_on_u);
}

static void
cb_interface_mode(mode)
long mode;
{
    proxy_interface_mode = mode;
    bot_set_handler(mode & NHPROXY_EXT_IM_STATUS ?
      nhproxy_proc_status : (void (*)())0L);
}

static void
cb_parse_options(char *opts)
{
    parseoptions(opts, FALSE, FALSE);
}

static struct nhproxy_cb_get_player_choices_res *
cb_get_player_choices()
{
    int i;
    static const char *aligns_adj[ROLE_ALIGNS], *genders_adj[ROLE_GENDERS];
    struct nhproxy_cb_get_player_choices_res *choices;
    choices =
      (struct nhproxy_cb_get_player_choices_res *)alloc(sizeof(*choices));
    choices->n_aligns = ROLE_ALIGNS;
    choices->aligns = aligns_adj;
    for(i = 0; i < ROLE_ALIGNS; i++)
	aligns_adj[i] = aligns[i].adj;
    choices->n_genders = ROLE_GENDERS;
    choices->genders = genders_adj;
    for(i = 0; i < ROLE_GENDERS; i++)
	genders_adj[i] = genders[i].adj;
    for(i = 0; races[i].noun; i++)
	;
    choices->n_races = i;
    choices->races = (const char **)alloc(i * sizeof(const char *));
    for(i = 0; i < choices->n_races; i++)
	choices->races[i] = races[i].noun;
    for(i = 0; roles[i].name.m; i++)
	;
    choices->n_roles = i;
    choices->roles = (struct nhproxy_cb_get_player_choices_res_role *)
      alloc(i * sizeof(struct nhproxy_cb_get_player_choices_res_role));
    for(i = 0; i < choices->n_roles; i++) {
	choices->roles[i].male = roles[i].name.m;
	choices->roles[i].female = roles[i].name.f ? roles[i].name.f : "";
    }
    return choices;
}

static void
cb_free_player_choices(choices)
struct nhproxy_cb_get_player_choices_res *choices;
{
    free(choices->races);
    free(choices->roles);
    free(choices);
}

#define SKIP(n)	do { \
	    i += (n) / pack; \
	    k += ((n) % pack) * ROLE_GENDERS; \
	    if (k >= pack * ROLE_GENDERS) { \
		i++; \
		k -= pack * ROLE_GENDERS; \
	    } \
	} while(0)

static struct nhproxy_cb_get_valid_selections_res *
cb_get_valid_selections()
{
    int role, race, gend, align;
    const int pack = 32 / ROLE_GENDERS;	/* No. masks packed in each element */
    int i, k;
    struct nhproxy_cb_get_valid_selections_res *vs;
    vs = (struct nhproxy_cb_get_valid_selections_res *)alloc(sizeof(*vs));
    for(vs->no_roles = 0; roles[vs->no_roles].name.m; vs->no_roles++)
	;
    for(vs->no_races = 0; races[vs->no_races].noun; vs->no_races++)
	;
    vs->no_aligns = ROLE_ALIGNS;
    vs->no_genders = ROLE_GENDERS;
    vs->n_masks = (vs->no_roles * vs->no_races * ROLE_ALIGNS + pack - 1) / pack;
    vs->masks = (unsigned long *)alloc(vs->n_masks * sizeof(unsigned long));
    memset((genericptr_t)vs->masks, 0, vs->n_masks * sizeof(unsigned long));
    for(role = 0, i = 0, k = 0; role < vs->no_roles; role++)
	if (!validrole(role))
	    SKIP(vs->no_races * ROLE_ALIGNS);
	else
	    for(race = 0; race < vs->no_races; race++)
		if (!validrace(role, race))
		    SKIP(ROLE_ALIGNS);
		else
		    for(align = 0; align < ROLE_ALIGNS; align++) {
			if (!validalign(role, race, align))
			    k += ROLE_GENDERS;
			else
			    for(gend = 0; gend < ROLE_GENDERS; gend++, k++)
				if (validgend(role, race, gend))
				    vs->masks[i] |= 1L << k;
			if (k >= pack * ROLE_GENDERS) {
			    i++;
			    k = 0;
			}
		    }
    if (i != (k ? vs->n_masks - 1 : vs->n_masks))
	panic("cb_get_valid_selections: Bad packing (%d, %d, %d)",
	  i, k, vs->n_masks);
    return vs;
}

static void
cb_free_valid_selections(vs)
struct nhproxy_cb_get_valid_selections_res *vs;
{
    free(vs->masks);
    free(vs);
}

#undef SKIP

static void
cb_quit_game()
{
    if (program_state.something_worth_saving)
	done2();
    else {
	clearlocks();
	exit_nhwindows(NULL);
	terminate(EXIT_SUCCESS);
    }
}

static void
cb_display_score()
{
    extern int proxy_rawprint_win;
    char *argv[] = {
	"nethack",
	"-sall",
    };

    proxy_rawprint_win = create_toptenwin();
    prscore(2, argv);
    display_nhwindow(proxy_rawprint_win, TRUE);
    destroy_toptenwin();
    dlb_init();                         /* Re-initialise DLB */
    proxy_rawprint_win = WIN_ERR;
}

static struct nhproxy_cb_get_extended_commands_res *
cb_get_extended_commands()
{
    int i;
    extern struct ext_func_tab extcmdlist[];
    struct nhproxy_cb_get_extended_commands_res *list;
    list = (struct nhproxy_cb_get_extended_commands_res *)alloc(sizeof(*list));
    for(i = 0; extcmdlist[i].ef_txt; i++)
	;
    list->n_commands = i;
    list->commands = (const char **)alloc(i * sizeof(const char *));
    for(i = 0; i < list->n_commands; i++)
	list->commands[i] = extcmdlist[i].ef_txt;
    return list;
}

static void
cb_free_extended_commands(struct nhproxy_cb_get_extended_commands_res *list)
{
    free(list->commands);
    free(list);
}

static int
cb_get_standard_winid(const char *window)
{
    int retval;
    if (!strcmp(window, "MESSAGE"))
	retval = WIN_MESSAGE;
    else if (!strcmp(window, "STATUS"))
	retval = WIN_STATUS;
    else if (!strcmp(window, "MAP"))
	retval = WIN_MAP;
    else if (!strcmp(window, "INVEN"))
	retval = WIN_INVEN;
    else
	retval = -1;
    return retval;
}

static struct nhproxy_cb_get_tilesets_res *
cb_get_tilesets()
{
    int i;
    char *file;
    struct nhproxy_cb_get_tilesets_res *list;
    list = (struct nhproxy_cb_get_tilesets_res *)alloc(sizeof(*list));
#ifdef MAXNOTILESETS
    list->n_tilesets = no_tilesets;
    list->tilesets = (struct nhproxy_cb_get_tilesets_res_tileset *)
      alloc(no_tilesets * sizeof(*list->tilesets));
    for(i = 0; i < list->n_tilesets; i++) {
	list->tilesets[i].name = tilesets[i].name;
	file = (char *)alloc(strlen(tilesets[i].file) + 12);
	sprintf(file, "$(TILEDIR)/%s", tilesets[i].file);
	list->tilesets[i].file = file;
	file = (char *)alloc(strlen(tilesets[i].file) + 16);
	sprintf(file, "$(TILEMAPDIR)/%s", tilesets[i].file);
	list->tilesets[i].mapfile = file;
	list->tilesets[i].flags = tilesets[i].flags;
    }
#else
    list->n_tilesets = 1;
    list->tilesets = (struct nhproxy_cb_get_tilesets_res_tileset *)
      alloc(1 * sizeof(*list->tilesets));
    list->tilesets[0].name = "Default";
    file = (char *)alloc(strlen(tile_file) + 12);
    sprintf(file, "$(TILEDIR)/%s", tile_file);
    list->tilesets[0].file = file;
    file = (char *)alloc(strlen(tile_file) + 16);
    sprintf(file, "$(TILEMAPDIR)/%s", tile_file);
    list->tilesets[0].mapfile = file;
    list->tilesets[0].flags = 0;
#endif
    return list;
}

static void
cb_free_tilesets(struct nhproxy_cb_get_tilesets_res *list)
{
    int i;
    for(i = 0; i < list->n_tilesets; i++)
	free((char *)list->tilesets[i].file);
    free(list->tilesets);
    free(list);
}

struct nhproxy_serv_callbacks win_proxy_callbacks = {
    cb_display_inventory,
    cb_dlbh_fopen,
    dlbh_fgets,
    dlbh_fread,
    0,			/* placeholder for dlbh_fwrite */
    dlbh_fclose,
    cb_flush_screen,
    (void (*)())doredraw,
    cb_interface_mode,
    cb_parse_options,
    get_option,
    cb_get_player_choices,
    cb_free_player_choices,
    cb_get_valid_selections,
    cb_free_valid_selections,
    cb_quit_game,
    cb_display_score,
    (void (*)())doset,
    cb_get_extended_commands,
    cb_free_extended_commands,
    (char NHPROXY_FDECL(*, (NHPROXY_CHAR_P)))map_menu_cmd,
    cb_get_standard_winid,
    cb_get_tilesets,
    cb_free_tilesets,
    get_glyph_mapping,
    free_glyph_mapping,
    set_option_mod_status,
#if NHPROXY_CHECK_VERSION(0,9,1)
    get_no_glyph,
#endif
};
