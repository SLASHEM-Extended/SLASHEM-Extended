/* $Id: nhextxdr.c,v 1.2 2001-09-06 18:06:38 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001 */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "nhxdr.h"

/*
 * This module implements the NhExt version of XDR according to RFC 1014.
 * It is functionally equivalent to the code produced by rpcgen, and can
 * be replaced by that on systems which support it. Rpcgen is included in
 * the freely available RPCSRC 4.0 from Sun Microsystems, which can be
 * downloaded in 17 shar files from ftp://bcm.tmc.edu/nfs/
 */

/*
 * Note: Although this routine is called _long and takes a pointer to a
 * long datum, it always reads and writes 32-bit values, regardless of
 * the size of the long type. This might cause problems on a platform
 * where long is smaller than 32 bits (but then NetHack doesn't support
 * such platforms anyway). On platforms with longs larger than 32 bits,
 * NetHack only needs the least significant 32 bits, and this function
 * will correctly encode and decode this quantity.
 */

boolean
nhext_xdr_long(xdrs, datum)
NhExtXdr *xdrs;
long *datum;
{
    int retval = TRUE;
    unsigned char buf[4];
    if (xdrs->x_op == NHEXT_XDR_ENCODE) {
	buf[0] = *datum >> 24 & 0xff;
	buf[1] = *datum >> 16 & 0xff;
	buf[2] = *datum >> 8 & 0xff;
	buf[3] = *datum & 0xff;
	retval = xdrs->x_write(xdrs, buf, 4);
    } else if (xdrs->x_op == NHEXT_XDR_DECODE) {
	retval = xdrs->x_read(xdrs, buf, 4);
	*datum =
	  (long)buf[0] << 24 | (long)buf[1] << 16 | (long)buf[2] << 8 | buf[3];
    }
    return retval;
}

boolean
nhext_xdr_u_long(xdrs, datum)
NhExtXdr *xdrs;
unsigned long *datum;
{
    int retval;
    long l;
    l = *datum;
    retval = nhext_xdr_long(xdrs, &l);
    *datum = (unsigned long)l;
    return retval;
}

boolean
nhext_xdr_short(xdrs, datum)
NhExtXdr *xdrs;
short *datum;
{
    int retval;
    long l;
    l = *datum;
    retval = nhext_xdr_long(xdrs, &l);
    *datum = (short)l;
    return retval;
}

boolean
nhext_xdr_u_short(xdrs, datum)
NhExtXdr *xdrs;
unsigned short *datum;
{
    int retval;
    long l;
    l = *datum;
    retval = nhext_xdr_long(xdrs, &l);
    *datum = (unsigned short)l;
    return retval;
}

boolean
nhext_xdr_int(xdrs, datum)
NhExtXdr *xdrs;
int *datum;
{
    int retval;
    long l;
    l = *datum;
    retval = nhext_xdr_long(xdrs, &l);
    *datum = (int)l;
    return retval;
}

boolean
nhext_xdr_u_int(xdrs, datum)
NhExtXdr *xdrs;
unsigned int *datum;
{
    int retval;
    long l;
    l = *datum;
    retval = nhext_xdr_long(xdrs, &l);
    *datum = (unsigned int)l;
    return retval;
}

boolean
nhext_xdr_char(xdrs, datum)
NhExtXdr *xdrs;
char *datum;
{
    int retval;
    long l;
    l = *datum;
    retval = nhext_xdr_long(xdrs, &l);
    *datum = (char)l;
    return retval;
}

boolean
nhext_xdr_u_char(xdrs, datum)
NhExtXdr *xdrs;
unsigned char *datum;
{
    int retval;
    long l;
    l = *datum;
    retval = nhext_xdr_long(xdrs, &l);
    *datum = (unsigned char)l;
    return retval;
}

boolean
nhext_xdr_enum(xdrs, datum)
NhExtXdr *xdrs;
int *datum;
{
    int retval;
    long l;
    l = *datum;
    retval = nhext_xdr_long(xdrs, &l);
    *datum = (int)l;
    return retval;
}

boolean
nhext_xdr_bool(xdrs, datum)
NhExtXdr *xdrs;
boolean *datum;
{
    int retval;
    long l;
    l = !!*datum;
    retval = nhext_xdr_long(xdrs, &l);
    *datum = (boolean)l;
    return retval;
}

boolean
nhext_xdr_string(xdrs, datum, maxsize)
NhExtXdr *xdrs;
char **datum;
unsigned int maxsize;
{
    int retval;
    long slen;
    long zero = 0;
    if (xdrs->x_op == NHEXT_XDR_ENCODE) {
	slen = strlen(*datum);
	if (slen > maxsize)
	    return FALSE;
	retval = nhext_xdr_long(xdrs, &slen);
	retval &= xdrs->x_write(xdrs, *datum, slen);
	if (slen & 3)
	    retval &= xdrs->x_write(xdrs, &zero, 4 - (slen & 3));
    }
    else if (xdrs->x_op == NHEXT_XDR_DECODE) {
	retval = nhext_xdr_long(xdrs, &slen);
	if (retval) {
	    if (slen > maxsize)
		return FALSE;
	    if (!*datum) {
		*datum = malloc(MAX(NHEXT_XDR_RNDUP(slen), slen + 1));
		if (!*datum)
		    return FALSE;
	    }
	    retval &= xdrs->x_read(xdrs, *datum, NHEXT_XDR_RNDUP(slen));
	    (*datum)[slen] = '\0';
	}
	
    }
    else if (xdrs->x_op == NHEXT_XDR_FREE) {
	free(*datum);
	*datum = NULL;
	retval = TRUE;
    }
    return retval;
}

boolean
nhext_xdr_vector(xdrs, addr, len, size, codec)
NhExtXdr *xdrs;
char *addr;
unsigned int len;
unsigned int size;
boolean (*codec)(NhExtXdr *, void *);
{
    for(; len > 0; len--) {
	if (!(*codec)(xdrs, addr))
	    return FALSE;
	addr += size;
    }
    return TRUE;
}

boolean
nhext_xdr_array(xdrs, addr, len, maxlen, size, codec)
NhExtXdr *xdrs;
char **addr;
unsigned int *len;
unsigned int maxlen;
unsigned int size;
boolean (*codec)(NhExtXdr *, void *);
{
    int retval;
    long slen;
    if (!nhext_xdr_u_int(xdrs, len))
	return FALSE;
    slen = *len;
    if (xdrs->x_op != NHEXT_XDR_FREE && slen > maxlen)
	return FALSE;
    if (xdrs->x_op == NHEXT_XDR_DECODE && !*addr && slen) {
	*addr = malloc(slen * size);
	if (!*addr)
	    return FALSE;
    }
    retval = nhext_xdr_vector(xdrs, *addr, slen, size, codec);
    if (xdrs->x_op == NHEXT_XDR_FREE) {
	free(*addr);
	*addr = NULL;
    }
    return retval;
}
