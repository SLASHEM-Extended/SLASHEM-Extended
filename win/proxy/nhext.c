/* $Id: nhext.c,v 1.12 2002-11-23 22:41:59 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001-2002 */
/* NetHack may be freely redistributed.  See license for details. */

/* #define DEBUG */

#include <stdlib.h>
#ifdef DEBUG
#include <stdio.h>
#endif
#include <stdarg.h>
#include "nhxdr.h"
#include "proxycom.h"

/*
 * This module implements the low-level NhExt protocols.
 */

static int no_connections = 0;

/*
 * During development (when we have both the server and the client in one
 * executable) we need to be able to maintain two seperate connections.
 * This will become unnecessary in the final application and the facility
 * should probably be removed for efficiency.
 */

static struct nhext_connection {
    int length;
    NhExtXdr *in, *out;
    NhExtIO *rd, *wr;
    struct nhext_svc *callbacks;
} nhext_connections[2];

int nhext_subprotocol1_init(NhExtIO *rd, NhExtIO *wr, struct nhext_svc *cb)
{
    int cn;
    if (no_connections == 2)
	return -1;
    cn = no_connections++;
    nhext_io_setmode(rd, NHEXT_IO_NOAUTOFILL);
    nhext_connections[cn].rd = rd;
    nhext_connections[cn].wr = wr;
    nhext_connections[cn].in = (NhExtXdr *)alloc(sizeof(NhExtXdr));
    nhext_connections[cn].out = (NhExtXdr *)alloc(sizeof(NhExtXdr));
    nhext_xdrio_create(nhext_connections[cn].in, rd, NHEXT_XDR_DECODE);
    nhext_xdrio_create(nhext_connections[cn].out, wr, NHEXT_XDR_ENCODE);
    nhext_connections[cn].callbacks = cb;
    return cn;
}

void nhext_subprotocol1_end_c(int cn)
{
    nhext_xdr_destroy(nhext_connections[cn].in);
    nhext_xdr_destroy(nhext_connections[cn].out);
    free(nhext_connections[cn].in);
    free(nhext_connections[cn].out);
    nhext_connections[cn].in = NULL;
    nhext_connections[cn].out = NULL;
}

/*
 * This function is available for callers to use if sub-protocol 1 fails.
 * It returns a pointer to the received packet which was being processed
 * at the time.
 */

char *nhext_subprotocol1_get_failed_packet(int cn, int *nb)
{
/* FIXME: The facility to read a packet from the remote process on the
 * assumption that it is a legal NhExt packet and then back-out if this
 * proves not to be the case is currently broken. While it would be
 * possible to implement this in sub-protocol 1, the use of NhExt IO
 * streams makes this non-trivial. The correct solution is to implement
 * this facility as part of the (as yet unwritten) sub-protocol 0
 * implementation.
 */
#if 0
    *nb = nhext_connections[cn].reply_len;
    return nhext_connections[cn].reply;
#else
    *nb = 0;
    return NULL;
#endif
}

static int nhext_rpc_vparams1(NhExtXdr *xdrs, int no, va_list *app)
{
    int retval = TRUE;
    int param;
    va_list ap;
    long param_l;
    long *param_pl;
    int param_i;
    int *param_pi;
    char *param_s;
    char **param_ps, *param_pc;
    nhext_xdr_bool_t param_b, *param_pb;
    int (*param_codec)();
    void *param_addr;
    ap = *app;
    while(retval && no--) {
	param = va_arg(ap, int);
	switch(param) {
	    case EXT_PARAM_INT:
		param_i = va_arg(ap, int);
		retval = nhext_xdr_int(xdrs, &param_i);
		break;
	    case EXT_PARAM_LONG:
		param_l = va_arg(ap, long);
		retval = nhext_xdr_long(xdrs, &param_l);
		break;
	    case EXT_PARAM_STRING:
		param_s = va_arg(ap, char *);
		retval = nhext_xdr_string(xdrs, &param_s, (unsigned int)-1);
		break;
	    case EXT_PARAM_BYTES:
		param_s = va_arg(ap, char *);
		param_i = va_arg(ap, int);
		retval = nhext_xdr_bytes(xdrs, &param_s, &param_i,
			(unsigned int)-1);
		break;
	    case EXT_PARAM_BOOLEAN:
		param_b = va_arg(ap, int);	/* boolean is promoted to int */
		retval = nhext_xdr_bool(xdrs, &param_b);
		break;
	    case EXT_PARAM_CHAR:
		param_i = va_arg(ap, int);	/* char is promoted to int */
		retval = nhext_xdr_int(xdrs, &param_i);
		break;
	    case EXT_PARAM_PTR | EXT_PARAM_INT:
		param_pi = va_arg(ap, int *);
		retval = nhext_xdr_int(xdrs, param_pi);
		break;
	    case EXT_PARAM_PTR | EXT_PARAM_LONG:
		param_pl = va_arg(ap, long *);
		retval = nhext_xdr_long(xdrs, param_pl);
		break;
	    case EXT_PARAM_PTR | EXT_PARAM_STRING:
		param_ps = va_arg(ap, char **);
		*param_ps = NULL;
		retval = nhext_xdr_string(xdrs, param_ps, (unsigned int)-1);
		break;
	    case EXT_PARAM_PTR | EXT_PARAM_BYTES:
		param_ps = va_arg(ap, char **);
		*param_ps = NULL;
		param_pi = va_arg(ap, int *);
		nhext_xdr_bytes(xdrs, param_ps, param_pi, (unsigned int)-1);
		break;
	    case EXT_PARAM_PTR | EXT_PARAM_BOOLEAN:
		param_pb = va_arg(ap, nhext_xdr_bool_t *);
		retval = nhext_xdr_bool(xdrs, param_pb);
		break;
	    case EXT_PARAM_PTR | EXT_PARAM_CHAR:
		param_pc = va_arg(ap, char *);
		retval = nhext_xdr_char(xdrs, param_pc);
		break;
	    case EXT_PARAM_XDRF:
		param_codec = (int (*)())va_arg(ap, void *);
		param_addr = va_arg(ap, void *);
		retval = (*param_codec)(xdrs, param_addr);
		break;
	    default:
		impossible("Bad key in proxy rpc (%d)", param);
		retval = FALSE;
		break;
	}
    }
    *app = ap;
    return retval;
}

/*
 * Note: nhext_rpc_params() does different things depending on whether
 * the XDR stream is set to encode or decode. In decode mode, the
 * header is assumed to have already been read whereas, in encode mode,
 * nhext_rpc_params() writes the header itself (using an ID of zero).
 */

int nhext_rpc_params(NhExtXdr *xdrs, int no, ...)
{
    int retval;
    va_list ap;
    unsigned long value;
    NhExtXdr sink;
    if (xdrs->x_op == NHEXT_XDR_ENCODE) {
	sink.x_op = NHEXT_XDR_COUNT;
	sink.x_pos = 0;
	va_start(ap, no);
	retval = nhext_rpc_vparams1(&sink, no, &ap);
	va_end(ap);
	if (!retval)
	    impossible("NhExt: Codec failed in sink");
	else {
	    value = sink.x_pos >> 2;
	    retval = nhext_xdr_u_long(xdrs, &value);
	    if (!retval)
		impossible("NhExt: Codec failed");
	}
	if (!retval)
	    return retval;
    }
    va_start(ap, no);
    retval = nhext_rpc_vparams1(xdrs, no, &ap);
    if (!retval)
	impossible("NhExt: Codec failed");
    va_end(ap);
    return retval;
}

/*
 * nhext_rpc() is a generic function to call a remote procedure and return
 * a result. It cannot deal with every possible type of request and result
 * packet, but it's very simple to use for those it can deal with.
 *
 * Arguments are:
 *	id		The ID of the procedure to call
 *	request		List of arguments describing the request packet
 *	response	List of arguments describing the response packet
 *
 * For both the request and response packets, the list of arguments consists
 * of:
 *
 *	no			The number of fields in the packet
 *	field[1] .. field[no]	The field to send/receive.
 */

int
nhext_rpc_c(int cn, unsigned short id, ...)
{
    struct nhext_connection *nc = nhext_connections + cn;
    NhExtXdr sink;
    va_list ap;
    unsigned long value, pos;
    int i, retval;
    int no;		/* Number of fields */

    sink.x_op = NHEXT_XDR_COUNT;
    sink.x_pos = 0;
    va_start(ap, id);
    no = va_arg(ap, int);
    if (!nhext_rpc_vparams1(&sink, no, &ap)) {
	va_end(ap);
	return FALSE;
    }
    va_end(ap);
    va_start(ap, id);
    (void) va_arg(ap, int);
    value = (id << 16) | (sink.x_pos >> 2);
    nhext_xdr_u_long(nc->out, &value);
    pos = nhext_xdr_getpos(nc->out);
    if (!nhext_rpc_vparams1(nc->out, no, &ap) || nhext_io_flush(nc->wr)) {
	impossible("Write to proxy interface failed");
	va_end(ap);
	return FALSE;
    }
    if (nhext_xdr_getpos(nc->out) - pos != sink.x_pos) {
	impossible(
	  "Miscounted length in proxy rpc ID %d (counted %lu, wrote %lu)",
	  id, sink.x_pos, nhext_xdr_getpos(nc->out) - pos);
	va_end(ap);
	return FALSE;
    }
    do
    {
	retval = nhext_svc_c(cn, nc->callbacks);
	if (retval < 0) {
	    impossible("Proxy server failed");
	    va_end(ap);
	    return FALSE;
	}
    } while(retval);
    no = va_arg(ap, int);
    /* External window ports are always allowed to return an empty reply to
     * a request. This indicates that the request is not supported.
     */
    if (no && !nc->length) {
	va_end(ap);
	return FALSE;
    }
    nhext_io_setautofill_limit(nc->rd, nc->length);
    if (!nhext_rpc_vparams1(nc->in, no, &ap)) {
	va_end(ap);
	return FALSE;
    }
    if (nhext_io_getc(nc->rd) >= 0) {
	/* One or more bytes are still available. This means that the
	 * whole packet was not used. We output an error and read and
	 * throw away the excess data.
	 */
	for(i = 1; nhext_io_getc(nc->rd) >= 0; i++)
	    ;
	impossible(
	  "Mismatch in RPC ID %d request length (%d of %d unused)",
	  id, i, nc->length);
	va_end(ap);
	return FALSE;
    }
    va_end(ap);
    return TRUE;
}

/*
 * nhext_svc() is a function to service incoming packets. It reads a packet
 * header from the remote process (either a child or a parent). Service
 * packets (those with non-zero IDs) are dispatched and replied to, reply
 * packets are left in the NhExt buffer (except for the header). The length
 * of the packet (as advised in the header) will be stored in the length
 * variable. In both cases, the ID is returned.
 *
 * nhext_svc() is thus suitable for use in two occasions. In the first, it
 * can be used to dispatch callbacks while waiting for a reply. In this mode
 * nhext_svc() should be called repeatedly until it returns 0. In the second
 * case, nhext_svc() can be used as the main loop of a server in which case
 * it should be called repeatedly to service requests and a zero ID should
 * be treated as an error.
 */

int
nhext_svc_c(int cn, struct nhext_svc *services)
{
    struct nhext_connection *nc = nhext_connections + cn;
    int i, j;
    unsigned short id;
    unsigned long value;
    if (nhext_io_getc(nc->rd) >= 0) {
	/* One or more bytes are already available. This means that a
	 * previous packet was not wholly used. We output an error and
	 * read and throw away the excess data.
	 */
	for(j = 1; nhext_io_getc(nc->rd) >= 0; j++)
	    ;
	impossible("Mismatch in packet length (%d of %d unused)",
	  j, nc->length);
    }
    nhext_io_setautofill_limit(nc->rd, 4);
    if (!nhext_xdr_u_long(nc->in, &value)) {
	impossible("Bad reply packet (no header)");
	return -1;
    }
    nc->length = (value & 0xffff) << 2;
    id = value >> 16;
    if (id)
    {
	nhext_io_setautofill_limit(nc->rd, nc->length);
	for(i = 0; services[i].id; i++) {
	    if (id == services[i].id) {
		(*services[i].handler)(id, nc->in, nc->out);
		break;
	    }
	}
	if (nhext_io_getc(nc->rd) >= 0) {
	    /* One or more bytes are still available. This means that the
	     * whole packet was not used. We output an error if we called
	     * a handler (otherwise the error is not helpful) and, in any
	     * case, read and throw away the excess data.
	     */
	    for(j = 1; nhext_io_getc(nc->rd) >= 0; j++)
		;
	    if (services[i].id)
		/* Note: This can only occur if there was not a recursive
		 * call (otherwise any error would have been detected
		 * either just before the next packet was read if this
		 * callback request was partially unused or just after
		 * the packet was processed if a reply packet was partially
		 * unused). The conclusion of all this is that nc->length
		 * is always valid. -- ALI
		 */
		impossible(
		  "Mismatch in callback ID %d request length (%d of %d unused)",
		  id, j, nc->length);
	}
	if (!services[i].id) {
#ifdef DEBUG
	    fprintf(stderr,"[%d] Unsupported proxy callback ID %d (%d known)\n",
	      getpid(), id, i);
#endif
	    nhext_rpc_params(nc->out, 0);
	}
	if (nhext_io_flush(nc->wr)) {
	    impossible("Write to proxy interface failed");
	    return -1;
	}
    }
    return id;
}
