/* $Id: nhxdr.c,v 1.3 2001-12-24 07:56:33 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001 */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "nhxdr.h"

/*
 * This module implements the NhExt version of the XDR support routines.
 * It is functionally equivalent to the code provided with rpcgen, and can
 * be replaced by that on systems which support it. Rpcgen is included in
 * the freely available RPCSRC 4.0 from Sun Microsystems, which can be
 * downloaded in 17 shar files from ftp://bcm.tmc.edu/nfs/
 */

unsigned int
nhext_xdr_getpos(xdrs)
NhExtXdr *xdrs;
{
    return xdrs->x_pos;
}

boolean
nhext_xdr_setpos(xdrs, pos)
NhExtXdr *xdrs;
unsigned int pos;
{
    if (pos > xdrs->x_size)
	return FALSE;
    xdrs->x_pos = pos;
    return TRUE;
}

void
nhext_xdr_free(codec, addr)
char *addr;
boolean (*codec)(NhExtXdr *, void *);
{
    NhExtXdr xdrs;
    xdrs.x_op = NHEXT_XDR_FREE;
    (void)(*codec)(&xdrs, addr);
}

static boolean
nhext_xdrmem_read(xdrs, addr, size)
NhExtXdr *xdrs;
char *addr;
int size;
{
    if (size < 0)
	return FALSE;
    if (size > xdrs->x_size - xdrs->x_pos)
	size = xdrs->x_size - xdrs->x_pos;
    memcpy(addr, xdrs->x_buf + xdrs->x_pos, size);
    xdrs->x_pos += size;
    return TRUE;
}

static boolean
nhext_xdrmem_write(xdrs, addr, size)
NhExtXdr *xdrs;
const char *addr;
int size;
{
    if (size < 0 || size > xdrs->x_size - xdrs->x_pos)
	return FALSE;
    memcpy(xdrs->x_buf + xdrs->x_pos, addr, size);
    xdrs->x_pos += size;
    return TRUE;
}

static void
nhext_xdrmem_destroy(xdrs)
NhExtXdr *xdrs;
{
}

void
nhext_xdrmem_create(xdrs, addr, size, op)
NhExtXdr *xdrs;
char *addr;
unsigned int size;
enum nhext_xdr_op op;
{
    xdrs->x_buf = addr;
    xdrs->x_op = op;
    xdrs->x_size = size;
    xdrs->x_pos = 0;
    xdrs->x_read = nhext_xdrmem_read;
    xdrs->x_write = nhext_xdrmem_write;
    xdrs->x_destroy = nhext_xdrmem_destroy;
}
