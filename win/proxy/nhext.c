/* $ Id: $ */
/* Copyright (c) Slash'EM Development Team 2001 */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "winproxy.h"

/*
 * This module implements the low-level NhExt protocols.
 */

static char request[1024];
static char reply[1024];
static NhExtXdr *out;
FILE *fp_in, *fp_out;

int nhext_subprotocol1_init(FILE *inf, FILE *outf)
{
    out = alloc(sizeof(*out));
    nhext_xdrmem_create(out, request, sizeof(request), NHEXT_XDR_ENCODE);
    fp_in = inf;
    fp_out = outf;
    return 0;
}

int nhext_subprotocol1_end(void)
{
    nhext_xdr_destroy(out);
    free(out);
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
    unsigned int value;
    int i;
    int len;		/* Length of the request packet */
    int no;		/* Number of fields */
    int param;
    int param_i;
    const char *param_s;
    winid param_w;
    boolean param_b;
    char param_c;

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
		param_s = va_arg(ap, const char *);
		nhext_xdr_string(out, &param_s);
		break;
	    case EXT_PARAM_WINID:
	    case EXT_PARAM_BOOLEAN:
	    case EXT_PARAM_CHAR:
	    case EXT_PARAM_XDRF:
	    case EXT_PARAM_PTR | EXT_PARAM_INT:
	    case EXT_PARAM_PTR | EXT_PARAM_STRING:
	    case EXT_PARAM_PTR | EXT_PARAM_WINID:
	    case EXT_PARAM_PTR | EXT_PARAM_BOOLEAN:
	    case EXT_PARAM_PTR | EXT_PARAM_CHAR:
	    default:
		impossible("Bad key in proxy rpc (%d)", param);
		break;
	}
    }
    len = nhext_xdr_getpos(out) - 4;
    value = (id << 16) | (len >> 2);
    nhext_xdr_setpos(out, 0);
    nhext_xdr_long(out, &value);

    send request

    for(;;)
    {
	read reply
	if (reply.ID != 0)
	{
	    process call back
	    send reply
	}
	else
	{
	    decode reply
	    break;
	}
    }
}
