/* $Id: nhextxdr.c,v 1.7 2003-10-25 18:06:01 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001-2003 */
/* NetHack may be freely redistributed.  See license for details. */

#include <stdlib.h>
#include <string.h>
#include "nhxdr.h"

/*
 * This module implements the NhExt version of XDR according to RFC 1014.
 * It is backwards compatible with the code produced by rpcgen, and can
 * be replaced by that for testing purposes on systems which support it.
 * NetHack uses the extra facilities implemented here and is not therefore
 * itself compatible with rpcgen.
 * Rpcgen is included in the freely available RPCSRC 4.0 from Sun
 * Microsystems, which can be downloaded in 17 shar files from
 * ftp://bcm.tmc.edu/nfs/
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

nhext_xdr_bool_t
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
    } else if (xdrs->x_op == NHEXT_XDR_COUNT)
	xdrs->x_pos += 4;
    return retval;
}

nhext_xdr_bool_t
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

nhext_xdr_bool_t
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

nhext_xdr_bool_t
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

nhext_xdr_bool_t
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

nhext_xdr_bool_t
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

nhext_xdr_bool_t
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

nhext_xdr_bool_t
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

nhext_xdr_bool_t
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

nhext_xdr_bool_t
nhext_xdr_bool(xdrs, datum)
NhExtXdr *xdrs;
nhext_xdr_bool_t *datum;
{
    int retval;
    long l;
    l = !!*datum;
    retval = nhext_xdr_long(xdrs, &l);
    *datum = (nhext_xdr_bool_t)l;
    return retval;
}

/*
 * A common routine for nhext_xdr_string() and nhext_xdr_bytes()
 */

static nhext_xdr_bool_t
nhext_xdr_bytestring(xdrs, datum, len, maxlen, ext)
NhExtXdr *xdrs;
char **datum;
unsigned int *len;
unsigned int maxlen, ext;
{
    int retval;
    long zero = 0;
    if (xdrs->x_op == NHEXT_XDR_ENCODE) {
	if (*len > maxlen)
	    return FALSE;
	retval = nhext_xdr_u_int(xdrs, len);
	retval &= xdrs->x_write(xdrs, *datum, *len);
	if (*len & 3)
	    retval &= xdrs->x_write(xdrs, &zero, 4 - (*len & 3));
    } else if (xdrs->x_op == NHEXT_XDR_DECODE) {
	retval = nhext_xdr_u_int(xdrs, len);
	if (retval) {
	    if (*len > maxlen)
		return FALSE;
	    if (!*datum) {
		*datum = malloc(MAX(NHEXT_XDR_RNDUP(*len), *len + ext));
		if (!*datum)
		    return FALSE;
	    }
	    retval &= xdrs->x_read(xdrs, *datum, NHEXT_XDR_RNDUP(*len));
	}
    } else if (xdrs->x_op == NHEXT_XDR_FREE) {
	free(*datum);
	*datum = NULL;
	retval = TRUE;
    } else if (xdrs->x_op == NHEXT_XDR_COUNT) {
	xdrs->x_pos += 4 + NHEXT_XDR_RNDUP(*len);
	retval = TRUE;
    }
    return retval;
}

nhext_xdr_bool_t
nhext_xdr_string(xdrs, datum, maxsize)
NhExtXdr *xdrs;
char **datum;
unsigned int maxsize;
{
    int retval;
    unsigned int slen;
    if (xdrs->x_op == NHEXT_XDR_ENCODE || xdrs->x_op == NHEXT_XDR_COUNT)
	slen = strlen(*datum);
    retval = nhext_xdr_bytestring(xdrs, datum, &slen, maxsize, 1);
    if (retval && xdrs->x_op == NHEXT_XDR_DECODE)
	(*datum)[slen] = '\0';
    return retval;
}

nhext_xdr_bool_t
nhext_xdr_bytes(xdrs, datum, len, maxlen)
NhExtXdr *xdrs;
char **datum;
unsigned int *len;
unsigned int maxlen;
{
    return nhext_xdr_bytestring(xdrs, datum, len, maxlen, 0);
}

nhext_xdr_bool_t
nhext_xdr_wrapstring(xdrs, datum)
NhExtXdr *xdrs;
char **datum;
{
    return nhext_xdr_string(xdrs, datum, (unsigned int)-1);
}

nhext_xdr_bool_t
nhext_xdr_vector(xdrs, addr, len, size, codec)
NhExtXdr *xdrs;
char *addr;
unsigned int len;
unsigned int size;
nhext_xdr_bool_t (*codec)(NhExtXdr *, void *);
{
    for(; len > 0; len--) {
	if (!(*codec)(xdrs, addr))
	    return FALSE;
	addr += size;
    }
    return TRUE;
}

nhext_xdr_bool_t
nhext_xdr_array(xdrs, addr, len, maxlen, size, codec)
NhExtXdr *xdrs;
char **addr;
unsigned int *len;
unsigned int maxlen;
unsigned int size;
nhext_xdr_bool_t (*codec)(NhExtXdr *, void *);
{
    int retval;
    long slen;
    if (!nhext_xdr_u_int(xdrs, len))
	return FALSE;
    slen = *len;
    if (xdrs->x_op != NHEXT_XDR_FREE && slen > maxlen)
	return FALSE;
    if (xdrs->x_op == NHEXT_XDR_DECODE && !*addr && slen) {
	*addr = calloc(slen, size);
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
