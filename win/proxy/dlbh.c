/* $Id: dlbh.c,v 1.1 2001-12-11 20:43:49 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001-2002 */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "dlb.h"

/*
 * A tiny front end to the dlb module which supports small integer handles.
 */

#define HANDLES_PER_NODE	32

struct dlb_node {
    struct dlb_node *next;
    int offset;
    dlb *handles[HANDLES_PER_NODE];
} *nodes = (struct dlb_node *)0;

int
#ifndef FILE_AREAS
dlbh_fopen(file, mode)
const char *file, *mode;
#else
dlbh_fopen_area(area, file, mode)
const char *area, *file, *mode;
#endif
{
    int i;
    struct dlb_node *n;
    for(n = nodes; n; n = n->next) {
	for(i = 0; i < HANDLES_PER_NODE; i++)
	    if (!n->handles[i])
		break;
	if (i < HANDLES_PER_NODE)
	    break;
    }
    if (!n) {
	n = (struct dlb_node *) alloc(sizeof(struct dlb_node));
	n->next = nodes;
	n->offset = nodes ? nodes->offset + HANDLES_PER_NODE : 0;
	for(i = 0; i < HANDLES_PER_NODE; i++)
	    n->handles[i] = NULL;
	nodes = n;
	i = 0;
    }
#ifndef FILE_AREAS
    n->handles[i] = dlb_fopen(file, mode);
#else
    n->handles[i] = dlb_fopen_area(area, file, mode);
#endif
    return n->handles[i] ? n->offset + i : -1;
}

static struct dlb_node *
dlbh_find_node(int fh)
{
    struct dlb_node *n;
    if (!nodes || fh >= nodes->offset + HANDLES_PER_NODE)
	return (struct dlb_node *)0;
    for(n = nodes; n; n = n->next)
	if (fh >= n->offset)
	    return n;
    return (struct dlb_node *)0;
}

int
dlbh_fclose(fh)
int fh;
{
    int i, retval;
    struct dlb_node *n = dlbh_find_node(fh);
    if (!n)
	return -1;
    retval = dlb_fclose(n->handles[fh - n->offset]);
    n->handles[fh - n->offset] = NULL;
    if (n == nodes) {
	while (nodes) {
	    for(i = 0; i < HANDLES_PER_NODE; i++)
		if (nodes->handles[i])
		    break;
	    if (i == HANDLES_PER_NODE) {
		n = nodes;
		nodes = nodes->next;
		free(n);
	    }
	    else
		break;
	}
    }
    return retval;
}

int
dlbh_fread(buf, size, quan, fh)
char *buf;
int size, quan, fh;
{
    struct dlb_node *n = dlbh_find_node(fh);
    return n ? dlb_fread(buf, size, quan, n->handles[fh - n->offset]) : 0;
}

int
dlbh_fseek(fh, pos, whence)
int fh;
long pos;
int whence;
{
    struct dlb_node *n = dlbh_find_node(fh);
    return n ? dlb_fseek(n->handles[fh - n->offset], pos, whence) : EOF;
}

char *
dlbh_fgets(buf, len, fh)
char *buf;
int len;
int fh;
{
    struct dlb_node *n = dlbh_find_node(fh);
    return n ? dlb_fgets(buf, len, n->handles[fh - n->offset]) : (char *)0;
}

int
dlbh_fgetc(fh)
int fh;
{
    struct dlb_node *n = dlbh_find_node(fh);
    return n ? dlb_fgetc(n->handles[fh - n->offset]) : EOF;
}

long
dlbh_ftell(fh)
int fh;
{
    struct dlb_node *n = dlbh_find_node(fh);
    return n ? dlb_ftell(n->handles[fh - n->offset]) : 0;
}
