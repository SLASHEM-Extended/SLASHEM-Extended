/* $ Id: $ */
/* Copyright (c) Slash'EM Development Team 2001 */
/* NetHack may be freely redistributed.  See license for details. */

#include "nhxdr.h"

/*
 * This module implements the NhExt version of the XDR support routines.
 * It is functionally equivalent to the code provided with rpcgen, and can
 * be replaced by that on systems which support it. Rpcgen is included in
 * the freely available RPCSRC 4.0 from Sun Microsystems, which can be
 * downloaded in 17 shar files from ....
 */

unsigned int
nhext_xdr_getpos(xdrs)
NhExtXdr *xdrs;
{
}

unsigned int
nhext_xdr_setpos(xdrs, pos)
NhExtXdr *xdrs;
unsigned int pos;
{
}

void
nhext_xdrmem_create(xdrs, addr, size, op)
NhExtXdr *xdrs;
char *addr;
unsigned int size;
enum nhext_xdr_op op;
{
}
