/* $Id: nhext.c,v 1.2 2001-09-06 18:06:38 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001 */
/* NetHack may be freely redistributed.  See license for details. */

#define NEED_VARARGS
#include "hack.h"
#include "winproxy.h"
#include "nhxdr.h"

/*
 * This module implements the low-level NhExt protocols.
 */

static char request[1024];
static char reply[1024];
static NhExtXdr *in, *out;
FILE *fp_in, *fp_out;

int nhext_subprotocol1_init(FILE *inf, FILE *outf)
{
    in = (NhExtXdr *)alloc(sizeof(*out));
    out = (NhExtXdr *)alloc(sizeof(*out));
    nhext_xdrmem_create(in, reply, sizeof(reply), NHEXT_XDR_DECODE);
    nhext_xdrmem_create(out, request, sizeof(request), NHEXT_XDR_ENCODE);
    fp_in = inf;
    fp_out = outf;
    return 0;
}

int nhext_subprotocol1_end(void)
{
    nhext_xdr_destroy(in);
    nhext_xdr_destroy(out);
    free(in);
    free(out);
    in = NULL;
    out = NULL;
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

void
nhext_rpc(unsigned short id, ...)
{
    va_list ap;
    unsigned long value;
    int i, retval;
    int len;		/* Length of the request packet */
    int no;		/* Number of fields */
    int param;
    long param_i;
    int *param_pi;
    char *param_s;
    char **param_ps, *param_pc;
    winid *param_pw;
    boolean param_b, *param_pb;
    int (*param_codec)();
    void *param_addr;

    nhext_xdr_setpos(out, 4);		/* Leave space for value */
    va_start(ap, id);
    no = va_arg(ap, int);
    while(no) {
	param = va_arg(ap, int);
	switch(param) {
	    case EXT_PARAM_INT:
		param_i = va_arg(ap, int);
		nhext_xdr_long(out, &param_i);
		break;
	    case EXT_PARAM_STRING:
		param_s = va_arg(ap, char *);
		nhext_xdr_string(out, &param_s, (unsigned int)-1);
		break;
	    case EXT_PARAM_WINID:
		param_i = va_arg(ap, winid);
		nhext_xdr_long(out, &param_i);
		break;
	    case EXT_PARAM_BOOLEAN:
		param_b = va_arg(ap, boolean);
		nhext_xdr_bool(out, &param_b);
		break;
	    case EXT_PARAM_CHAR:
		param_i = va_arg(ap, char);
		nhext_xdr_long(out, &param_i);
		break;
	    case EXT_PARAM_XDRF:
	    case EXT_PARAM_PTR | EXT_PARAM_INT:
	    case EXT_PARAM_PTR | EXT_PARAM_STRING:
	    case EXT_PARAM_PTR | EXT_PARAM_WINID:
	    case EXT_PARAM_PTR | EXT_PARAM_BOOLEAN:
	    case EXT_PARAM_PTR | EXT_PARAM_CHAR:
	    default:
		impossible("Bad key in proxy rpc (%d)", param);
		return;
		break;
	}
    }
    len = nhext_xdr_getpos(out) - 4;
    value = (id << 16) | (len >> 2);
    nhext_xdr_setpos(out, 0);
    nhext_xdr_u_long(out, &value);
    if (fwrite(request, 1, len + 4, fp_out) != len + 4) {
	impossible("Write to proxy interface failed");
	return;
    }
    do
    {
	len = fread(reply, 1, sizeof(reply), fp_in);
	nhext_xdr_setpos(in, 0);
	nhext_xdr_u_long(in, &value);
	id = value >> 16;
	if (len - 4 != (value & 0xffff) << 2) {
	    impossible("Bad reply packet (%d != %d)", len - 4,
	      (value & 0xffff) << 2);
	    return;
	}
	if (id)
	{
	    impossible("Unsupported proxy callback ID %d", id);
	    nhext_xdr_setpos(out, 0);
	    value = 0;	/* Reply with no results */
	    nhext_xdr_u_long(out, &value);
	    if (fwrite(request, 1, 4, fp_out) != 4) {
		impossible("Write to proxy interface failed");
		return;
	    }
	}
    } while(id);
    no = va_arg(ap, int);
    while(no) {
	param = va_arg(ap, int);
	switch(param) {
	    case EXT_PARAM_PTR | EXT_PARAM_INT:
		param_pi = va_arg(ap, int *);
		retval = nhext_xdr_int(in, param_pi);
		break;
	    case EXT_PARAM_PTR | EXT_PARAM_STRING:
		param_ps = va_arg(ap, char **);
		*param_ps = NULL;
		retval = nhext_xdr_string(in, param_ps, (unsigned int)-1);
		break;
	    case EXT_PARAM_PTR | EXT_PARAM_WINID:
		param_pw = va_arg(ap, winid *);
		retval = nhext_xdr_long(in, &param_i);
		*param_pw = (winid)param_i;
		break;
	    case EXT_PARAM_PTR | EXT_PARAM_BOOLEAN:
		param_pb = va_arg(ap, boolean *);
		retval = nhext_xdr_bool(in, param_pb);
		break;
	    case EXT_PARAM_PTR | EXT_PARAM_CHAR:
		param_pc = va_arg(ap, char *);
		retval = nhext_xdr_char(in, param_pc);
		break;
	    case EXT_PARAM_XDRF:
		param_codec = (int (*)())va_arg(ap, void *);
		param_addr = va_arg(ap, void *);
		retval = (*param_codec)(in, param_addr);
		break;
	    case EXT_PARAM_INT:
	    case EXT_PARAM_STRING:
	    case EXT_PARAM_WINID:
	    case EXT_PARAM_BOOLEAN:
	    case EXT_PARAM_CHAR:
	    default:
		impossible("Bad key in proxy rpc (%d)", param);
		return;
		break;
	}
	if (!retval) {
	    impossible("Decode failed in proxy rpc (%d, %d)",
	      id, nhext_xdr_getpos(in));
	    return;
	}
    }
    if (nhext_xdr_getpos(in) != len)
	impossible("Unexpected results in proxy rpc (%d, %d)", id, len);
}
