/* $Id: nhext.c,v 1.16 2003-01-05 07:41:49 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001-2002 */
/* NetHack may be freely redistributed.  See license for details. */

/* #define DEBUG */

#include <stdlib.h>
#ifdef DEBUG
#include <stdio.h>
#endif
#include <string.h>
#include <stdarg.h>
#include "nhxdr.h"
#include "proxycom.h"

/*
 * This module implements the low-level NhExt protocols.
 */

static struct nhext_connection {
    int length;
    NhExtXdr *in, *out;
    NhExtIO *rd, *wr;
    struct nhext_svc *callbacks;
} nhext_connection;

int nhext_init(NhExtIO *rd, NhExtIO *wr, struct nhext_svc *cb)
{
    nhext_io_setmode(rd, NHEXT_IO_NOAUTOFILL);
    nhext_connection.rd = rd;
    nhext_connection.wr = wr;
    nhext_connection.in = (NhExtXdr *)alloc(sizeof(NhExtXdr));
    nhext_connection.out = (NhExtXdr *)alloc(sizeof(NhExtXdr));
    nhext_xdrio_create(nhext_connection.in, rd, NHEXT_XDR_DECODE);
    nhext_xdrio_create(nhext_connection.out, wr, NHEXT_XDR_ENCODE);
    nhext_connection.callbacks = cb;
    return 0;
}

void nhext_end()
{
    nhext_xdr_destroy(nhext_connection.in);
    nhext_xdr_destroy(nhext_connection.out);
    free(nhext_connection.in);
    free(nhext_connection.out);
    nhext_connection.in = NULL;
    nhext_connection.out = NULL;
}

static char *nhext_subprotocol0_encode_value(char *buf, char *value)
{
    *buf++ = '"';
    while (*value) {
	if (*value == '\\' || *value == '"')
	    *buf++ = '\\';
	*buf++ = *value++;
    }
    *buf++ = '"';
    return buf;
}

int nhext_subprotocol0_write_line(struct nhext_line *line)
{
    int i, len, retval;
    char *buf, *bp;
    len = strlen(line->type) + 1;
    for(i = 0; i < line->n; i++) {
	len += strlen(line->tags[i]) + 1;
	len += strlen(line->values[i]) * 2 + 2 + 1;
    }
    buf = (char *)alloc(len);
    (void)strcpy(buf, line->type);
    bp = buf + strlen(buf);
    for(i = 0; i < line->n; i++) {
	*bp++ = ' ';
	(void)strcpy(bp, line->tags[i]);
	bp += strlen(bp);
	*bp++ = ' ';
	bp = nhext_subprotocol0_encode_value(bp, line->values[i]);
    }
    *bp++ = '\n';
    retval = nhext_io_write(nhext_connection.wr, buf, bp - buf) == bp - buf &&
      !nhext_io_flush(nhext_connection.wr);
    free(buf);
    return retval;
}

#define NHEXT_SP0_NORMAL_SIZE	64
#define NHEXT_SP0_SPECIAL_SIZE	200

/*
 * Return the next token read or NULL on error. An empty string
 * will be returned if the line ends before a token is found.
 */

static char *nhext_subprotocol0_read_token(NhExtIO *io)
{
    int i, ch;
    static char token[NHEXT_SP0_NORMAL_SIZE+1];
    for(i = 0;; ) {
	ch = nhext_io_getc(io);
	if (ch < 0) {
#ifdef DEBUG
	    fprintf(stderr,
	      "[%d] EOF/ERROR while reading sub-protocol 0 token\n", getpid());
#endif
	    return NULL;
	}
	if (ch == '\r' || ch == '\n')
	    break;
	if (ch == ' ')
	    if (i)
		break;
	    else
		continue;
	if (ch != '_' && !isalnum(ch)) {
#ifdef DEBUG
	    fprintf(stderr,
	      "[%d] Illegal character (0x%02X) while reading sub-protocol 0 token\n",
	      getpid(), ch);
#endif
	    return NULL;
	}
	if (i == NHEXT_SP0_NORMAL_SIZE) {
#ifdef DEBUG
	    fprintf(stderr,
	      "[%d] Too many characters while reading sub-protocol 0 token\n",
	      getpid());
#endif
	    return NULL;
	}
	token[i++] = ch;
    }
    token[i] = '\0';
#ifdef DEBUG
    fprintf(stderr, "nhext_subprotocol0_read_token: %s\n", token);
#endif
    return token;
}

/*
 * Return the next value read or NULL on error. A NULL
 * will be returned if the line ends before a value can be found.
 */

static char *nhext_subprotocol0_read_value(NhExtIO *io, int isspecial)
{
    int i, ch, esc = FALSE, invalue = FALSE;
    const int maxlen =
      isspecial ? NHEXT_SP0_SPECIAL_SIZE : NHEXT_SP0_NORMAL_SIZE;
    static char value[NHEXT_SP0_SPECIAL_SIZE+1];
    for(i = 0;; ) {
	ch = nhext_io_getc(io);
	if (ch < 0) {
#ifdef DEBUG
	    fprintf(stderr,
	      "[%d] EOF/ERROR while reading sub-protocol 0 value\n", getpid());
#endif
	    return NULL;
	}
	if (!invalue) {
	    if (ch == '"')
		invalue = TRUE;
	    else if (ch == ' ')
		continue;
	    else {
#ifdef DEBUG
		fprintf(stderr,
		  "[%d] Read 0x%02X while expecting sub-protocol 0 value\n",
		  getpid(), ch);
#endif
		return NULL;
	    }
	} else if (ch < ' ' || ch > '~') {
#ifdef DEBUG
	    fprintf(stderr,
	      "[%d] Illegal character (0x%02X) while reading sub-protocol 0 value\n",
	      getpid(), ch);
#endif
	    return NULL;
	} else if (!esc && ch == '\\')
	    esc = TRUE;
	else if (!esc && ch == '"')
	    break;
	else if (i == maxlen) {
#ifdef DEBUG
	    fprintf(stderr,
	      "[%d] Too many characters while reading sub-protocol 0 value\n",
	      getpid());
#endif
	    return NULL;
	} else {
	    value[i++] = ch;
	    esc = FALSE;
	}
    }
    value[i] = '\0';
#ifdef DEBUG
    fprintf(stderr, "nhext_subprotocol0_read_value: %s\n", value);
#endif
    return value;
}

void nhext_subprotocol0_free_line(struct nhext_line *line)
{
    int i;
    for(i = 0; i < line->n; i++) {
	free(line->tags[i]);
	free(line->values[i]);
    }
    free(line->type);
    free(line->tags);
    free(line->values);
    free(line);
}

struct nhext_line *nhext_subprotocol0_read_line()
{
    int i, iserror;
    char *s;
    struct nhext_line *line;
#ifdef DEBUG
    if (nhext_io_getc(nhext_connection.rd) >= 0)
	fprintf(stderr,
	  "[%d] NhExt: Non-empty buffer in nhext_subprotocol0_read_line\n",
	  getpid());
#endif
    if (nhext_io_filbuf(nhext_connection.rd) <= 0) {
#ifdef DEBUG
	fprintf(stderr,
	  "[%d] EOF/ERROR while trying to read sub-protocol 0 packet\n",
	  getpid());
#endif
	return NULL;
    }
    line = (struct nhext_line *) alloc(sizeof(*line));
    s = nhext_subprotocol0_read_token(nhext_connection.rd);
    if (!s) {
	free(line);
	return NULL;
    }
    iserror = !strcmp(s, "Error");
    line->type = strdup(s);
    line->n = 0;
    line->tags = (char **)0;
    line->values = (char **)0;
    for(i = 0;; i++) {
	s = nhext_subprotocol0_read_token(nhext_connection.rd);
	if (!s || !*s)
	    break;
	if (line->n) {
	    line->tags =
	      (char **)realloc(line->tags, (line->n + 1)*sizeof(char **));
	    line->values =
	      (char **)realloc(line->values, (line->n + 1)*sizeof(char **));
	    if (!line->tags || !line->values) {
#ifdef DEBUG
		fprintf(stderr,
		  "[%d] NhExt: Memory allocation failure; cannot get %u tags",
		  getpid(), line->n + 1);
#endif
		s = NULL;
		break;
	    }
	} else {
	    line->tags = (char **)malloc(sizeof(char **));
	    line->values = (char **)malloc(sizeof(char **));
	    if (!line->tags || !line->values) {
#ifdef DEBUG
		fprintf(stderr,
		  "[%d] NhExt: Memory allocation failure; cannot get 1 tag",
		  getpid());
#endif
		s = NULL;
		break;
	    }
	}
	line->tags[line->n] = strdup(s);
	s = nhext_subprotocol0_read_value(nhext_connection.rd,
	  iserror && !strcmp(s, "mesg"));
	if (!s) {
	    free(line->tags[line->n]);
	    break;
	}
	line->values[line->n] = strdup(s);
	line->n++;
    }
    if (s && nhext_io_getc(nhext_connection.rd) >= 0) {
	for(i = 1; nhext_io_getc(nhext_connection.rd) >= 0; i++)
	    ;
#ifdef DEBUG
	fprintf(stderr,
	  "[%d] %d extra character%s after valid sub-protocol 0 packet\n",
	  getpid(), i, i == 1 ? "": "s");
#endif
	s = NULL;
    }
    if (!s) {
	nhext_subprotocol0_free_line(line);
	return NULL;
    } else
	return line;
}

/*
 * This function is available for callers to use if sub-protocol 0 fails.
 * It returns a pointer to the received packet which was being processed
 * at the time.
 */

char *nhext_subprotocol0_get_failed_packet(int *nb)
{
    if (nhext_connection.in)
	return nhext_io_getpacket(nhext_connection.rd, nb);
    else
	return (char *)0;
}

static void nhext_default_handler(int class, const char *error)
{
    impossible(error);
}

static nhext_errhandler nhext_error_handler = nhext_default_handler;

nhext_errhandler nhext_set_errhandler(nhext_errhandler new)
{
    nhext_errhandler old = nhext_error_handler;
    nhext_error_handler = new;
    return old;
}

static void nhext_error(int class, const char *fmt, ...)
{
    va_list ap;
    char buf[128];
    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    nhext_error_handler(class, buf);
    va_end(ap);
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
		nhext_error(EXT_ERROR_INTERNAL,
		  "Bad key in proxy rpc (%d)", param);
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
	    nhext_error(EXT_ERROR_INTERNAL, "Codec failed in sink");
	else {
	    value = sink.x_pos >> 2;
	    retval = nhext_xdr_u_long(xdrs, &value);
	    if (!retval)
		nhext_error(EXT_ERROR_COMMS, "Failed to write header");
	}
	if (!retval)
	    return retval;
    }
    va_start(ap, no);
    retval = nhext_rpc_vparams1(xdrs, no, &ap);
    if (!retval)
	nhext_error(EXT_ERROR_COMMS, "Codec failed");
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
nhext_rpc(unsigned short id, ...)
{
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
	nhext_error(EXT_ERROR_INTERNAL, "Codec failed in sink");
	va_end(ap);
	return FALSE;
    }
    va_end(ap);
    va_start(ap, id);
    (void) va_arg(ap, int);
    value = (id << 16) | (sink.x_pos >> 2);
    nhext_xdr_u_long(nhext_connection.out, &value);
    pos = nhext_xdr_getpos(nhext_connection.out);
    if (!nhext_rpc_vparams1(nhext_connection.out, no, &ap) ||
      nhext_io_flush(nhext_connection.wr)) {
	nhext_error(EXT_ERROR_COMMS, "Write to proxy interface failed");
	va_end(ap);
	return FALSE;
    }
    if (nhext_xdr_getpos(nhext_connection.out) - pos != sink.x_pos) {
	nhext_error(EXT_ERROR_INTERNAL,
	  "Miscounted length in proxy rpc ID %d (counted %lu, wrote %lu)",
	  id, sink.x_pos, nhext_xdr_getpos(nhext_connection.out) - pos);
	va_end(ap);
	return FALSE;
    }
    do
    {
	retval = nhext_svc(nhext_connection.callbacks);
	if (retval < 0) {
	    va_end(ap);
	    return FALSE;
	}
    } while(retval);
    no = va_arg(ap, int);
    /* External window ports are always allowed to return an empty reply to
     * a request. This indicates that the request is not supported.
     */
    if (no && !nhext_connection.length) {
	nhext_error(EXT_ERROR_NOTSUPPORTED,
	  "Procedure %d not supported by remote end", id);
	va_end(ap);
	return FALSE;
    }
    nhext_io_setautofill_limit(nhext_connection.rd, nhext_connection.length);
    if (!nhext_rpc_vparams1(nhext_connection.in, no, &ap)) {
	/*
	 * There are two important causes of nhext_rpc_vparams1() failing.
	 * Either the packet sent by the remote end is shorter than we
	 * are expecting or a comms error caused us to fail to read the
	 * whole packet. We distinguish these by checking if NOAUTOFILL
	 * is set (in which case we have read the advertised length).
	 */
	if (nhext_io_getmode(nhext_connection.rd) & NHEXT_IO_NOAUTOFILL)
	    nhext_error(EXT_ERROR_PROTOCOL,
	      "Short reply received for protocol %d (received %d)", id,
	      nhext_connection.length);
	else
	    nhext_error(EXT_ERROR_COMMS, "Read from proxy interface failed");
	va_end(ap);
	return FALSE;
    }
    if (nhext_io_getc(nhext_connection.rd) >= 0) {
	/* One or more bytes are still available. This means that the
	 * whole packet was not used. We output an error and read and
	 * throw away the excess data.
	 */
	for(i = 1; nhext_io_getc(nhext_connection.rd) >= 0; i++)
	    ;
	nhext_error(EXT_ERROR_PROTOCOL,
	  "Mismatch in RPC ID %d reply length (%d of %d unused)",
	  id, i, nhext_connection.length);
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
nhext_svc(struct nhext_svc *services)
{
    int i, j;
    unsigned short id;
    unsigned long value;
    if (nhext_io_getc(nhext_connection.rd) >= 0) {
	/* One or more bytes are already available. This means that a
	 * previous packet was not wholly used. We output an error and
	 * read and throw away the excess data.
	 */
	for(j = 1; nhext_io_getc(nhext_connection.rd) >= 0; j++)
	    ;
	nhext_error(EXT_ERROR_PROTOCOL,
	  "Mismatch in packet length (%d of %d unused)",
	  j, nhext_connection.length);
    }
    nhext_io_setautofill_limit(nhext_connection.rd, 4);
    if (!nhext_xdr_u_long(nhext_connection.in, &value)) {
	nhext_error(EXT_ERROR_COMMS, "Read from proxy interface failed");
	return -1;
    }
    nhext_connection.length = (value & 0xffff) << 2;
    id = value >> 16;
    if (id) {
	nhext_io_setautofill_limit(nhext_connection.rd,
	  nhext_connection.length);
	for(i = 0; services[i].id; i++) {
	    if (id == services[i].id) {
		(*services[i].handler)(id, nhext_connection.in,
		  nhext_connection.out);
		break;
	    }
	}
	if (nhext_io_getc(nhext_connection.rd) >= 0) {
	    /* One or more bytes are still available. This means that the
	     * whole packet was not used. We output an error if we called
	     * a handler (otherwise the error is not helpful) and, in any
	     * case, read and throw away the excess data.
	     */
	    for(j = 1; nhext_io_getc(nhext_connection.rd) >= 0; j++)
		;
	    if (services[i].id)
		/* Note: This can only occur if there was not a recursive
		 * call (otherwise any error would have been detected
		 * either just before the next packet was read if this
		 * callback request was partially unused or just after
		 * the packet was processed if a reply packet was partially
		 * unused). The conclusion of all this is that
		 * nhext_connection.length is always valid. -- ALI
		 */
		nhext_error(EXT_ERROR_PROTOCOL,
		  "Mismatch in callback ID %d request length (%d of %d unused)",
		  id, j, nhext_connection.length);
	}
	if (!services[i].id) {
#ifdef DEBUG
	    fprintf(stderr,"[%d] Unsupported proxy callback ID %d (%d known)\n",
	      getpid(), id, i);
#endif
	    nhext_error(EXT_ERROR_NOTSUPPORTED, "Procedure %d not supported",
	      id);
	    nhext_rpc_params(nhext_connection.out, 0);
	}
	if (nhext_io_flush(nhext_connection.wr)) {
	    nhext_error(EXT_ERROR_COMMS, "Write to proxy interface failed");
	    return -1;
	}
    }
    return id;
}
