/* $ Id: $ */
/* Copyright (c) Slash'EM Development Team 2001 */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "winproxy.h"

/*
 * This module implements the NhExt version of XDR according to RFC 1014.
 * It is functionally equivalent to the code produced by rpcgen, and can
 * be replaced by that on systems which support it. Rpcgen is included in
 * the freely available RPCSRC 4.0 from Sun Microsystems, which can be
 * downloaded in 17 shar files from ....
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

int
nhext_xdr_long(xdrs, datum)
NhExtXdr *xdrs;
long *datum;
{
    int retval = 1;
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

int
nhext_xdr_string(xdrs, datum)
NhExtXdr *xdrs;
char **datum;
{
    int retval = 1;
    long slen;
    long zero = 0;
    if (xdrs->x_op == NHEXT_XDR_ENCODE) {
	slen = strlen(*datum);
	retval = nhext_xdr_long(xdrs, &slen);
	retval &= xdrs->x_write(xdrs, *datum, slen);
	if (slen & 3)
	    retval &= xdrs->x_write(xdrs, &zero, 4 - (slen & 3));
    }
    else if (xdrs->x_op == NHEXT_XDR_DECODE) {
	retval = nhext_xdr_long(xdrs, &slen);
	if (retval) {
	    *datum = malloc(MAX(NHEXT_XDR_RNDUP(slen), slen + 1));
	    if (!*datum)
		return 0;
	    retval &= xdrs->x_read(xdrs, *datum, NHEXT_XDR_RNDUP(slen));
	    *datum = realloc(*datum, slen + 1);
	}
	
    }
    else if (xdrs->x_op == NHEXT_XDR_FREE)
	free(*datum);
    return retval;
}
