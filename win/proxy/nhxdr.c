/* $Id: nhxdr.c,v 1.6 2003-10-25 18:06:01 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001-2003 */
/* NetHack may be freely redistributed.  See license for details. */

/* #define DEBUG */

#include <stdlib.h>
#include <string.h>
#ifdef DEBUG
#include <stdio.h>
#endif
#include "nhxdr.h"

/*
 * This module implements the NhExt version of the XDR support routines.
 * It is functionally equivalent to the code provided with rpcgen, and
 * except for the xdrio stream can be replaced by that on systems which
 * support it. Rpcgen is included in the freely available RPCSRC 4.0
 * from Sun Microsystems, which can be downloaded in 17 shar files from
 * ftp://bcm.tmc.edu/nfs/ (this is mainly useful in testing).
 *
 * The xdrio stream has no direct equivalent in RPCSRC. Sun's xdrrec
 * stream has the capabilities but this requires both client and server
 * to use it which is not acceptable. Sun's xdrstdio stream is very
 * similar in function to xdrio but would either require changes to
 * the source of RPCSRC or the use of local pipes to connect xdrstdio
 * to nhext_io.
 */

#ifdef DEBUG
static void
debug_dump(buf, len, arrow)
void *buf;
unsigned int len;
char *arrow;
{
    int i, j, nc, ni;
    long l;
    char cbuf[17];
    unsigned char *bp = buf;
    for(i = 0; i < len; ) {
	if ((i & 15) == 0) {
	    if (!i)
	    {
		ni = fprintf(stderr, "[%d] ", getpid());
		ni += fputs(arrow, stderr);
	    }
	    else {
		cbuf[16] = '\0';
		while(nc++ < 40)
		    fputc(' ', stderr);
		fputs(cbuf, stderr);
		fputs("\n", stderr);
		for(j = 0; j <= ni; j++)
		    fputc(' ', stderr);
	    }
	    nc = 2;
	}
	if (len - i >= 4) {
	    l = (long)bp[i] << 24 | (long)bp[i + 1] << 16 |
	      (long)bp[i + 2] << 8 | bp[i + 3];
	    fprintf(stderr, " %08X", l);
	    nc += 9;
	    for(j = 0; j < 4; j++, i++)
		cbuf[i & 15] = isgraph(bp[i]) || bp[i] == ' ' ?  bp[i] : '.';
	}
	else {
	    fprintf(stderr, " %02X", bp[i]);
	    nc += 3;
	    cbuf[i & 15] = isgraph(bp[i]) || bp[i] == ' ' ? bp[i] : '.';
	    i++;
	}
    }
    if (len) {
	cbuf[i & 15 ? i & 15 : 16] = '\0';
	while(nc++ < 40)
	    fputc(' ', stderr);
	fputs(cbuf, stderr);
    }
    fputc('\n', stderr);
}
#endif	/* DEBUG */

unsigned int
nhext_xdr_getpos(xdrs)
NhExtXdr *xdrs;
{
    return xdrs->x_pos;
}

nhext_xdr_bool_t
nhext_xdr_setpos(xdrs, pos)
NhExtXdr *xdrs;
unsigned int pos;
{
    if (!xdrs->x_size || pos > xdrs->x_size)
	return FALSE;
    xdrs->x_pos = pos;
    return TRUE;
}

void
nhext_xdr_free(codec, addr)
char *addr;
nhext_xdr_bool_t (*codec)(NhExtXdr *, void *);
{
    NhExtXdr xdrs;
    xdrs.x_op = NHEXT_XDR_FREE;
    (void)(*codec)(&xdrs, addr);
}

static nhext_xdr_bool_t
nhext_xdrmem_read(xdrs, addr, size)
NhExtXdr *xdrs;
char *addr;
int size;
{
    if (size < 0)
	return FALSE;
    if (size > xdrs->x_size - xdrs->x_pos)
	size = xdrs->x_size - xdrs->x_pos;
    memcpy(addr, (unsigned char *)xdrs->x_data + xdrs->x_pos, size);
#ifdef DEBUG
    debug_dump(addr, size, "<<");
#endif
    xdrs->x_pos += size;
    return TRUE;
}

static nhext_xdr_bool_t
nhext_xdrmem_write(xdrs, addr, size)
NhExtXdr *xdrs;
const char *addr;
int size;
{
    if (size < 0 || size > xdrs->x_size - xdrs->x_pos)
	return FALSE;
#ifdef DEBUG
    debug_dump(addr, size, ">>");
#endif
    memcpy((unsigned char *)xdrs->x_data + xdrs->x_pos, addr, size);
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
    xdrs->x_data = addr;
    xdrs->x_op = op;
    xdrs->x_size = size;
    xdrs->x_pos = 0;
    xdrs->x_read = nhext_xdrmem_read;
    xdrs->x_write = nhext_xdrmem_write;
    xdrs->x_destroy = nhext_xdrmem_destroy;
}

static nhext_xdr_bool_t
nhext_xdrio_read(xdrs, addr, size)
NhExtXdr *xdrs;
char *addr;
int size;
{
    int retval;
    if (size <= 0)
	return !size;
    retval = nhext_io_fread(addr, size, 1, (NhExtIO *)xdrs->x_data);
#ifdef DEBUG
    if (retval < 0)
	/*
	 * Only nhext_xdr_bytestring() calls us with anything larger
	 * than 4 bytes. If this is triggered then either NhExtIO
	 * needs to be modified to increase the buffer size or the
	 * NhExt protocol needs to be modified to keep all strings
	 * below NhExtIO's limit.
	 */
	fprintf(stderr, "nhext_xdrio: datum too large to read (%d)\n", size);
#endif
    if (retval == 1) {
#ifdef DEBUG
	debug_dump(addr, size, "<<");
#endif
	xdrs->x_pos += size;
    }
#ifdef DEBUG
    else
	fprintf(stderr, "[%d] << EOF\n", getpid());
#endif
    return retval == 1;
}

static nhext_xdr_bool_t
nhext_xdrio_write(xdrs, addr, size)
NhExtXdr *xdrs;
const char *addr;
int size;
{
    int retval;
    if (size < 0)
	return FALSE;
#ifdef DEBUG
    debug_dump(addr, size, ">>");
#endif
    retval = nhext_io_write((NhExtIO *)xdrs->x_data, (void *)addr, size);
    xdrs->x_pos += retval;
    return retval == size;
}

static void
nhext_xdrio_destroy(xdrs)
NhExtXdr *xdrs;
{
    (void)nhext_io_flush((NhExtIO *)xdrs->x_data);
}

void
nhext_xdrio_create(xdrs, io, op)
NhExtXdr *xdrs;
NhExtIO *io;
enum nhext_xdr_op op;
{
    xdrs->x_data = io;
    xdrs->x_op = op;
    xdrs->x_size = 0;		/* xdr_setpos() is not supported */
    xdrs->x_pos = 0;
    xdrs->x_read = nhext_xdrio_read;
    xdrs->x_write = nhext_xdrio_write;
    xdrs->x_destroy = nhext_xdrio_destroy;
}
