/* $Id: mapid.c,v 1.2 2001-12-24 07:56:33 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001-2002 */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "nhxdr.h"
#include "winproxy.h"

/*
 * [ALI] It would be far too much work to pass generic identifiers to
 * plug-in window ports (and also pointless). Instead we substitute
 * integers. We keep a record of the mapping so that we can reverse
 * this in the reply.
 * Note: zero is a special value and must be preserved. Window ports
 * should treat all other values as opaque.
 */

struct mapid__window {
    winid id;
    int no_identifiers;
    anything *identifiers;
};
static int mapid__no_windows;
static struct mapid__window *mapid__windows;

/*
 * Get the index into mapid__windows for window id, adding an
 * entry if necessary.
 */

static int mapid__add_winid(id)
winid id;
{
    int i;
    if (id == WIN_ERR)
	panic("mapid__add_winid: Bad window ID");
    for(i = 0; i < mapid__no_windows; i++)
	if (mapid__windows[i].id == id)
	    return i;
    for(i = 0; i < mapid__no_windows; i++)
	if (mapid__windows[i].id == WIN_ERR)
	    break;
    if (i == mapid__no_windows) {
	if (mapid__no_windows++)
	    mapid__windows = (struct mapid__window *)realloc(mapid__windows, 
	      mapid__no_windows * sizeof(*mapid__windows));
	else
	    mapid__windows = (struct mapid__window *)malloc(sizeof(*mapid__windows));
	if (!mapid__windows)
	    panic("proxy: can't get %d bytes",
	      mapid__no_windows * sizeof(*mapid__windows));
    }
    mapid__windows[i].id = id;
    mapid__windows[i].no_identifiers = 0;
    mapid__windows[i].identifiers = (anything *)0;
    return i;
}

/*
 * Scrap any entry in mapid__windows for window id.
 */

void mapid_del_winid(id)
winid id;
{
    int i;
    if (id == WIN_ERR)
	panic("mapid_del_winid: Bad window ID");
    for(i = 0; i < mapid__no_windows; i++)
	if (mapid__windows[i].id == id) {
	    if (mapid__windows[i].no_identifiers)
		free(mapid__windows[i].identifiers);
	    mapid__windows[i].id = WIN_ERR;
	    break;
	}
}

/*
 * Map identifier into an integer which can be used to unmap back to
 * the same identifier later. Zero is treated specially and will always
 * map to zero (and non-zero identifiers will never map to zero).
 */

int mapid_map_identifier(id, identifier)
winid id;
const anything *identifier;
{
    int i, j;
    if (identifier->a_void == 0)
	return 0;
    i = mapid__add_winid(id);
    if (j = mapid__windows[i].no_identifiers++)
	mapid__windows[i].identifiers =
	  (anything *)realloc(mapid__windows[i].identifiers, 
	  mapid__windows[i].no_identifiers * sizeof(*mapid__windows->identifiers));
    else
	mapid__windows[i].identifiers =
	  (anything *)malloc(sizeof(*mapid__windows->identifiers));
    if (!mapid__windows[i].identifiers)
	panic("proxy: can't get %d bytes",
	  mapid__windows[i].no_identifiers * sizeof(*mapid__windows->identifiers));
    mapid__windows[i].identifiers[j] = *identifier;
    return j + 1;
}

/*
 * Retrieve the identifier from the mapping.
 */

void mapid_unmap_identifier(id, mapping, identifier)
winid id;
int mapping;
anything *identifier;
{
    int i;
    if (mapping == 0) {
	identifier->a_void = 0;
	return;
    }
    if (id == WIN_ERR)
	panic("mapid_unmap_identifier: Bad window ID");
    for(i = 0; i < mapid__no_windows; i++)
	if (mapid__windows[i].id == id)
	    break;
    if (i == mapid__no_windows)
    {
	impossible("Ext: Trying to unmap on an unopened window?");
	identifier->a_void = 0;
	return;
    }
    if (mapping < 1 || mapping > mapid__windows[i].no_identifiers)
    {
	impossible("Bad identifier returned from plug-in.");
	identifier->a_void = 0;
	return;
    }
    *identifier = mapid__windows[i].identifiers[mapping - 1];
}

/*
 * Discard all previously mapped identifiers for window.
 */

void mapid_del_identifiers(id)
winid id;
{
    int i;
    if (id == WIN_ERR)
	panic("mapid_del_indentifiers: Bad window ID");
    for(i = 0; i < mapid__no_windows; i++)
	if (mapid__windows[i].id == id) {
	    if (mapid__windows[i].no_identifiers)
		free(mapid__windows[i].identifiers);
	    mapid__windows[i].identifiers = (anything *)0;
	    mapid__windows[i].no_identifiers = 0;
	    break;
	}
}
