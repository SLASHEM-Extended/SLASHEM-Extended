/* $Id: nhextio.c,v 1.5 2003-10-25 18:06:01 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2003 */
/* NetHack may be freely redistributed.  See license for details. */

/* NhExt: buffering support */

/* #define PRINTF_FP_SUPPORT */		/* Support for %e, %f & %g */

#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#include <string.h>
#ifdef PRINTF_FP_SUPPORT
#include <float.h>
#endif
#include "nhxdr.h"

struct NhExtIO_ {
    unsigned int flags;
    unsigned int autofill_limit;	/* Zero for no limit */
    nhext_io_func func, nb_func;
    void *handle;
    unsigned char buffer[1025];		/* Capable of storing n-1 chars */
    unsigned char *rp, *wp;
    NhExtNB *nb;
};

#define ADVANCE_PTR(io, ptr, inc)				\
	do {							\
	    (ptr) += (inc);					\
	    if ((ptr) >= (io)->buffer + sizeof((io)->buffer))	\
		(ptr) -= sizeof((io)->buffer);			\
	} while(0)

/* Flags changeable by nhext_io_setmode */

#define NHEXT_IO__USERFLAGS	(NHEXT_IO_NOAUTOFILL | NHEXT_IO_LINEBUF)

/* Flags readable by nhext_io_getmode */

#define NHEXT_IO__READFLAGS	(NHEXT_IO__USERFLAGS | \
				NHEXT_IO_NBLOCK | \
				NHEXT_IO_RDONLY | \
				NHEXT_IO_WRONLY)

NhExtIO *nhext_io_open(nhext_io_func func, void *handle, unsigned int flags)
{
    NhExtIO *io;
    io = malloc(sizeof(*io));
    if (!io)
	return NULL;
    io->flags = flags;
    io->autofill_limit = 0;
    io->func = func;
    io->nb_func = NULL;
    io->handle = handle;
    io->rp = io->wp = io->buffer;
    io->nb = NULL;
    return io;
}

int nhext_io_close(NhExtIO *io)
{
    int retval;
    retval = nhext_io_flush(io);
    if (io->nb)
	(void)nhext_nb_close(io->nb);
    free(io);
    return retval;
}

unsigned int nhext_io_getmode(NhExtIO *io)
{
    return io->flags & NHEXT_IO__READFLAGS;
}

void nhext_io_setmode(NhExtIO *io, unsigned int flags)
{
    io->flags &= ~NHEXT_IO__USERFLAGS;
    io->flags |= flags & NHEXT_IO__USERFLAGS;
}

/* Warning: The numbering of free blocks is a little odd */ 

static void nhext_io__getfree(NhExtIO *io, int *nf1, int *nf2)
{
    /*  R
     *  W
     * +--------------------+
     * |222222222222222222|G|    1 - free1, 2 - free2, D - data, G - guard
     * +--------------------+
     *
     *        R        W
     * +--------------------+
     * |111|G|DDDDDDDD|22222|
     * +--------------------+
     *
     *  R            W
     * +--------------------+
     * |DDDDDDDDDDDD|22222|G|
     * +--------------------+
     *
     *  W     R
     * +--------------------+
     * |111|G|DDDDDDDDDDDDDD|
     * +--------------------+
     *
     *      W R
     * +--------------------+
     * |DDD|G|DDDDDDDDDDDDDD|  (and variations with guard byte at start or end)
     * +--------------------+
     */
    *nf1 = io->wp >= io->rp ? io->rp - io->buffer: io->rp - io->wp;
    *nf2 = io->wp >= io->rp ? io->buffer + sizeof(io->buffer) - io->wp : 0;
    /* Guard byte goes in free1 if non-empty else in free2 */
    if (*nf1)
	(*nf1)--;
    else
	(*nf2)--;
}

/* If an autofill limit is set, then autofill will be enabled until
 * that many bytes have been read and then disabled. Set a zero limit
 * to allow an unlimited number of bytes to be read (the default).
 * Notes:
 * 1. When a limit is in place, all calls to filbuf will reduce
 *    the limit by the number of bytes read regardless of whether the
 *    call was made internally or by the application.
 * 2. If there are unread bytes in the buffer at the time this function
 *    is called then the limit will be adjusted as if they had been read
 *    after the given limit was set.
 */

void nhext_io_setautofill_limit(NhExtIO *io, unsigned int limit)
{
    int nf1, nf2, nb;
    nhext_io__getfree(io, &nf1, &nf2);
    nb = sizeof(io->buffer) - nf1 - nf2 - 1;
    if (limit > nb) {
	io->flags &= ~NHEXT_IO_NOAUTOFILL;
	io->autofill_limit = limit - nb;
    } else {
	io->flags |= NHEXT_IO_NOAUTOFILL;
	io->autofill_limit = 0;
    }
}

void nhext_io_setnbfunc(NhExtIO *io, nhext_io_func func)
{
    /* If there is a read pending then delay the closure until it completes */
    if (io->nb && !(io->flags & NHEXT_IO_PENDING)) {
	nhext_nb_close(io->nb);
	io->nb = NULL;
    }
    io->nb_func = func;
    io->flags |= NHEXT_IO_NBLOCK;
}

/*
 * Returns: >0 if buffer now non-empty,
 *          else 0 on EOF, -1 on error, or -2 on would block
 */

int nhext_io_filbuf(NhExtIO *io, int blocking)
{
    int retval, nf1, nf2;
    if (!blocking && !(io->flags & NHEXT_IO_NBLOCK))
	return -2;
    /*
     * NHEXT_IO_NBLOCK is advisory only, so no error on failure
     */
    if ((io->flags & NHEXT_IO_NBLOCK) && !io->nb && !io->nb_func) {
	io->nb = nhext_nb_open(io->func, io->handle);
	if (!io->nb)
	    io->flags &= ~NHEXT_IO_NBLOCK;
    }
    if (io->rp == io->wp && !(io->flags & NHEXT_IO_PENDING))
	io->rp = io->wp = io->buffer;
    nhext_io__getfree(io, &nf1, &nf2);
    if (nf2) {
	if (nf1 && !(io->flags & NHEXT_IO_PENDING)) {
	    /* Combine free blocks so that we don't make unneccesary
	     * calls to I/O function (which may be expensive).
	     */
	    io->flags &= ~NHEXT_IO_SIMPLEBUFFER;
	    memmove(io->buffer, io->rp, io->wp - io->rp);
	    io->wp += io->buffer - io->rp;
	    io->rp = io->buffer;
	}
	nf1 += nf2;
	/* nf2 = 0 */
    }
    if (!nf1)
	return 1;	/* Buffer is full */
    /*
     * A simple buffer is one that has the whole of the latest packet read
     * stored starting at io->buffer and ending at io->wp - 1. Such buffers
     * may be used to retrieve said packet by calling nhext_io_getpacket().
     */
    if (io->wp == io->buffer)
	io->flags |= NHEXT_IO_SIMPLEBUFFER;
    else
	io->flags &= ~NHEXT_IO_SIMPLEBUFFER;
    if (io->nb) {
	retval = nhext_nb_read(io->nb, io->wp, nf1, blocking);
	/*
	 * If we have a read call pending then we must remeber this so that
	 * we don't change io->wp or allow a direct read or else NhExtNB's
	 * requirements (of keeping read call parameters constant until a
	 * read is complete) would be broken.
	 */
	if (retval == -2)
	    io->flags |= NHEXT_IO_PENDING;
	else {
	    io->flags &= ~NHEXT_IO_PENDING;
	    if (io->nb_func) {
		/* Closure delayed until completion of pending read */
		nhext_nb_close(io->nb);
		io->nb = NULL;
	    }
	}
    } else if (!blocking)
	retval = (*io->nb_func)(io->handle, io->wp, nf1);
    else {
	retval = (*io->func)(io->handle, io->wp, nf1);
	if (retval < 0)
	    retval = -1;
    }
    if (retval > 0) {
	ADVANCE_PTR(io, io->wp, retval);
	if (!(io->flags & NHEXT_IO_NOAUTOFILL) && io->autofill_limit) {
	    if (io->autofill_limit > retval)
		io->autofill_limit -= retval;
	    else {
		io->autofill_limit = 0;
		io->flags |= NHEXT_IO_NOAUTOFILL;
	    }
	}
    }
    return retval;
}

int nhext_io_getc(NhExtIO *io)
{
    unsigned char c;
    if (io->rp == io->wp &&
      (io->flags & NHEXT_IO_NOAUTOFILL || nhext_io_filbuf(io, TRUE) <= 0))
	return -1;		/* getc doesn't distinguish EOF and ERROR */
    c = *io->rp;
    ADVANCE_PTR(io, io->rp, 1);
    if (io->rp == io->wp && io->nb)
	nhext_io_filbuf(io, FALSE);
    return c;
}

/*
 * nhext_io_read() reads between 1 and nb bytes from the stream. A read will
 * only be requested on the underlying I/O system if there would otherwise
 * be no bytes to return to the caller (and not even then if NOAUTOFILL is
 * set). This prevents blocking but means that using nhext_io_read() to
 * read fixed sized datums is not useful. For example,
 *	nhext_io_read(io, &datum, sizeof(datum))
 * may easily only partially read datum even though the rest of datum is
 * available for reading. Where the caller knows in advance that an infinite
 * block will not occur when reading, nhext_io_fread() should be used instead:
 *	nhext_io_fread(&datum, sizeof(datum), 1, io);
 */

int nhext_io_read(NhExtIO *io, char *buf, int nb)
{
    int i;
    int retval = 0;
    /* Satisfy from read buffer (or 1st half if split) */
    if (nb && io->rp != io->wp) {
	if (io->wp > io->rp)
	    i = io->wp - io->rp;
	else
	    i = io->buffer + sizeof(io->buffer) - io->rp;
	if (i > nb)
	    i = nb;
	memcpy(buf, io->rp, i);
	ADVANCE_PTR(io, io->rp, i);
	buf += i;
	nb -= i;
	retval += i;
    }
    /* Satisfy from 2nd half of read buffer */
    if (nb && io->rp != io->wp) {
	i = io->wp - io->rp;	/* rp must point at buffer at this point */
	if (i > nb)
	    i = nb;
	memcpy(buf, io->rp, i);
	ADVANCE_PTR(io, io->rp, i);
	buf += i;
	nb -= i;
	retval += i;
    }
    /* The buffer is empty or the request is satisfied.
     * If we've read some data or we shouldn't auto-fill then we're done
     */
    if (retval || io->flags & NHEXT_IO_NOAUTOFILL) {
	if (io->nb)
	    nhext_io_filbuf(io, FALSE);
	return retval;
    }
    if (nb >= sizeof(io->buffer) && !(io->flags & NHEXT_IO_PENDING)) {
	/* If caller still wants more than we can buffer and we don't have
	 * a read call pending, read direct
	 */
	io->flags &= ~NHEXT_IO_SIMPLEBUFFER;
	if (io->nb)
	    i = nhext_nb_read(io->nb, buf, nb, TRUE);
	else
	    i = (*io->func)(io->handle, buf, nb);
	if (i <= 0) {
	    /* If we have previously read some data correctly, then return
	     * this number and leave EOF/ERROR reporting until later.
	     * Otherwise report EOF as 0 and ERROR as -1
	     */
	    return retval ? retval : i < 0 ? -1 : 0;
	}
	if (io->autofill_limit) {
	    if (io->autofill_limit > i)
		io->autofill_limit -= i;
	    else {
		io->autofill_limit = 0;
		io->flags |= NHEXT_IO_NOAUTOFILL;
	    }
	}
	buf += i;
	nb -= i;
	retval += i;
    } else if (nb) {
	/* Caller wants a fragment or we have a read call pending;
	 * fill our buffer and then satisfy from there.
	 */
	i = nhext_io_filbuf(io, TRUE);
	if (i <= 0)
	    return retval ? retval : i < 0 ? -1 : 0;	/* As above */
	i = io->wp - io->rp;	/* rp must point at buffer at this point */
	if (i > nb)
	    i = nb;
	memcpy(buf, io->rp, i);
	ADVANCE_PTR(io, io->rp, i);
	buf += i;
	nb -= i;
	retval += i;
    }
    return retval;
}

/* nhext_io_fread() reads up to nmemb members of the given size and returns
 * the number read. Where fewer members were read than requested then an EOF
 * or ERROR has occured. Any partially read member will still be available
 * for reading after nhext_io_fread() returns.
 *
 * A negative return indicates that the member size is too large.
 */

int nhext_io_fread(void *buffer, int size, int nmemb, NhExtIO *io)
{
    int nb, bf;
    int nbp = 0;		/* Number of bytes read in partial read */
    int nm = nmemb;		/* Members still to read */
    if (size > sizeof(io->buffer)) {
	/*
	 * We need enough room to store a partial member so that we don't
	 * get an indeterminate result. This means that the buffer must
	 * be capable of storing (size - 1) bytes, which is equivalent to
	 * saying that the buffer size (including the guard byte) must be
	 * no smaller than the size of a member.
	 */
	return -1;
    }
    if (size <= 0)
	return -1;
    bf = INT_MAX / size;	/* Blocking factor */
    while(nm) {
	if (bf > nm)
	    bf = nm;
	nb = nhext_io_read(io, buffer, bf * size);
	if (nb <= 0)
	    return nmemb - nm;
	buffer += nb;
	nm -= nb / size;
	nbp = nb % size;
	while(nbp) {
	    nb = nhext_io_read(io, buffer, size - nbp);
	    if (nb <= 0) {
		/* EOF or ERROR after a partial read.
		 *
		 * The buffer must be empty so we can place the partially
		 * read member back into it.
		 */
		io->flags &= ~NHEXT_IO_SIMPLEBUFFER;
		memcpy(io->buffer, buffer - nbp, nbp);
		io->rp = io->buffer;
		io->wp = io->buffer + nbp;
		return nmemb - nm;
	    }
	    nbp += nb;
	    buffer += nb;
	    if (nbp == size) {
		nm--;
		nbp = 0;
	    }
	}
    }
    return nmemb;
}

/* nhext_io_getpacket() gets the last packet read under certain circumstances.
 * This will always work if the caller sets no-autofill mode and calls
 * nhext_io_fillbuf() on an empty buffer. Calls to nhext_io_fread() must
 * also be avoided since this can cause simple buffer mode to be cancelled
 * (nhext_io_fread() could be re-written to avoid this if it became important).
 */

char *nhext_io_getpacket(NhExtIO *io, int *nb)
{
    if (io->flags & NHEXT_IO_SIMPLEBUFFER) {
	*nb = io->wp - io->buffer;
	return io->buffer;
    } else
	return NULL;
}

/* Return TRUE if a read call would block */

int nhext_io_willblock(NhExtIO *io)
{
    if (io->flags & NHEXT_IO_WRONLY)
	return -1;
    if (io->rp != io->wp)
	return FALSE;				/* Buffer non-empty */
    if (io->flags & NHEXT_IO_NBLOCK)
	nhext_io_filbuf(io, FALSE);
    return io->rp == io->wp;
}

int nhext_io_flush(NhExtIO *io)
{
    unsigned char *s;
    int retval, nb1, nb2;
    if (io->rp == io->wp)
	return 0;		/* Nothing to do */
    if (!(io->flags & NHEXT_IO_WRONLY))
	return 0;		/* Read streams don't need flushing */
    /*
     *        W       R
     * +--------------------+
     * |11111|FFFFF|G|222222|    1 - block1, 2 - block2, F - free, G - guard
     * +--------------------+
     *
     *        R        W
     * +--------------------+
     * |FFF|G|11111111|FFFFF|
     * +--------------------+
     *
     *  R            W
     * +--------------------+
     * |111111111111|FFFFF|G|
     * +--------------------+
     *
     *  W     R
     * +--------------------+
     * |FFF|G|22222222222222|
     * +--------------------+
     *
     *      W R
     * +--------------------+
     * |111|G|22222222222222|  (and variations with guard byte at start or end)
     * +--------------------+
     */
    nb1 = io->wp >= io->rp ? io->wp - io->rp : io->wp - io->buffer;
    nb2 = io->wp >= io->rp ? 0 : io->buffer + sizeof(io->buffer) - io->rp;
    if (nb1 && nb2) {
	/* Try and combine split buffer so that we don't make unneccesary
	 * calls to I/O function (which may be expensive).
	 */
	s = malloc(nb2);	/* For copy of 2nd half */
	if (s) {
	    memcpy(s, io->buffer, nb2);
	    memmove(io->buffer, io->rp, nb1);
	    memcpy(io->buffer + nb1, s, nb2);
	    io->rp = io->buffer;
	    io->wp = io->buffer + nb1 + nb2;
	    nb1 += nb2;
	    nb2 = 0;
	    free(s);
	}
    }
    /* Write out buffer (or 1st half if split) */
    while (nb1) {
	retval = (*io->func)(io->handle, io->rp, nb1);
	if (retval > 0) {
	    ADVANCE_PTR(io, io->rp, retval);
	    nb1 -= retval;
	} else
	    return -1;
    }
    /* Write out 2nd half */
    while (nb2) {
	retval = (*io->func)(io->handle, io->rp, nb2);
	if (retval > 0) {
	    io->rp += retval;	/* rp can't wrap while writing 2nd half */
	    nb2 -= retval;
	} else
	    return -1;
    }
    io->rp = io->wp = io->buffer;	/* Buffer is now empty */
    return 0;
}

int nhext_io_fputc(int c, NhExtIO *io)
{
    unsigned char ch, *wp = io->wp;
    ch = c;
    *wp = ch;
    ADVANCE_PTR(io, wp, 1);
    if (wp == io->rp) {
	/* Buffer was full; flush first */
	if (nhext_io_flush(io))
	    return -1;
	*io->wp = ch;
	ADVANCE_PTR(io, io->wp, 1);
    }
    else
	io->wp = wp;
    if (io->flags & NHEXT_IO_LINEBUF && ch == '\n' && nhext_io_flush(io))
	return -1;
    return (int)ch;
}

int nhext_io_write(NhExtIO *io, char *buf, int nb)
{
    int i;
    int retval = 0, nf1, nf2;
    /* If the buffer is not empty, then top it up first */
    if (nb && io->wp != io->rp) {
	nhext_io__getfree(io, &nf1, &nf2);
	/* If free2 is non-empty, then wp must point at it so store there first
	 */
	if (nb && nf2) {
	    i = nf2;
	    if (i > nb)
		i = nb;
	    memcpy(io->wp, buf, i);
	    ADVANCE_PTR(io, io->wp, i);
	    buf += i;
	    nb -= i;
	    retval += i;
	    /* nf2 -= i; */
	}
	/* If there is any excess then store in free1
	 * (which wp must now point to after filling free2)
	 */
	if (nb && nf1) {
	    i = nf1;
	    if (i > nb)
		i = nb;
	    memcpy(io->wp, buf, i);
	    ADVANCE_PTR(io, io->wp, i);
	    buf += i;
	    nb -= i;
	    retval += i;
	    /* nf1 -= i; */
	}
    }
    if (nb) {
	/* Then flush the buffer (which must be either empty (NOP) or full) */
	if (nhext_io_flush(io))
	    return retval ? retval : -1;
	/* If we can't buffer the data then write it directly until we can.
	 */
	while (nb >= sizeof(io->buffer)) {
	    i = (*io->func)(io->handle, buf, nb);
	    if (i <= 0)
		return retval ? retval : i;
	    buf += i;
	    nb -= i;
	    retval += i;
	}
    }
    if (nb) {
	/* Finally, place the last fragment in the by now empty buffer */
	memcpy(io->buffer, buf, nb);
	io->wp += nb;
	retval += nb;
    }
    return retval;
}

/* A version of write that honours LINEBUF */

int nhext_io_writet(NhExtIO *io, char *buf, int nb)
{
    int i, retval = 0;
    if (!(io->flags & NHEXT_IO_LINEBUF))
	return nhext_io_write(io, buf, nb);
    for(i = nb - 1; i >= 0; i--)
	if (buf[i] == '\n') {
	    retval = nhext_io_write(io, buf, i + 1);
	    if (retval <= i || nhext_io_flush(io) || retval == nb)
		return retval;
	    break;
	}
    i = nhext_io_write(io, buf + retval, nb - retval);
    if (i > 0)
	retval += i;
    else if (!retval)
	retval = i;
    return retval;
}

#define NHEXT_IO_FMT_LJUST	0x00001		/* neg width */
#define NHEXT_IO_FMT_SIGN	0x00002		/* + prefix */
#define NHEXT_IO_FMT_ALT	0x00004		/* # prefix */
#define NHEXT_IO_FMT_ZEROPAD	0x00008		/* 0 prefix */
#define NHEXT_IO_FMT_WIDTH	0x00010		/* width present */
#define NHEXT_IO_FMT_PRECISION	0x00020		/* precision present */
#define NHEXT_IO_FMT_SHORT	0x00040		/* h prefix */
#define NHEXT_IO_FMT_LONG	0x00080		/* l prefix */
#define NHEXT_IO_FMT_UNSIGNED	0x00100		/* u, x, X, p or P */
#define NHEXT_IO_FMT_HEX	0x00200		/* x, X, p or P */
#define NHEXT_IO_FMT_PTR	0x00400		/* p or P */
#define NHEXT_IO_FMT_CAPS	0x00800		/* Upper case variant */
#define NHEXT_IO_FMT_NEG	0x01000		/* Value is negative */
#define NHEXT_IO_FMT_EXP	0x02000		/* Output exponent suffix */
#define NHEXT_IO_FMT_DONE	0x04000		/* %fmt finished */

static int nhext_io_pad(NhExtIO *io, int flags, int padding, int pre)
{
    int i, nb = 0;
    int do_pad = (flags & (NHEXT_IO_FMT_WIDTH | NHEXT_IO_FMT_LJUST)) ==
      (pre ? NHEXT_IO_FMT_WIDTH : NHEXT_IO_FMT_WIDTH | NHEXT_IO_FMT_LJUST);
    int zero_pad = pre && flags & NHEXT_IO_FMT_ZEROPAD;
    if (flags & (NHEXT_IO_FMT_SIGN | NHEXT_IO_FMT_NEG))
	padding--;
    if ((flags & (NHEXT_IO_FMT_HEX | NHEXT_IO_FMT_ALT)) ==
      (NHEXT_IO_FMT_HEX | NHEXT_IO_FMT_ALT))
	padding -= 2;
    if (do_pad && !zero_pad) {
	for(i = 0; i < padding; i++)
	    if (nhext_io_fputc(' ', io) < 0)
		return -1;
	    else
		nb++;
    }
    if (pre) {
	if (flags & (NHEXT_IO_FMT_SIGN | NHEXT_IO_FMT_NEG)) {
	    if (nhext_io_fputc(flags & NHEXT_IO_FMT_NEG ? '-' : '+', io) < 0)
		return -1;
	    else
		nb++;
	}
	if ((flags & (NHEXT_IO_FMT_HEX | NHEXT_IO_FMT_ALT)) ==
	  (NHEXT_IO_FMT_HEX | NHEXT_IO_FMT_ALT)) {
	    if (nhext_io_fputc('0', io) < 0)
		return -1;
	    if (nhext_io_fputc(flags & NHEXT_IO_FMT_CAPS ? 'X' : 'x', io) < 0)
		return -1;
	    nb += 2;
	}
    }
    if (do_pad && zero_pad) {
	for(i = 0; i < padding; i++)
	    if (nhext_io_fputc('0', io) < 0)
		return -1;
	    else
		nb++;
    }
    return nb;
}

/* Warning: Always cast signed integers to long before passing to printi */

static int nhext_io_printi(NhExtIO *io, int flags, int width, unsigned long v)
{
    int i, retval = 0, digit, w, padding;
    int radix = flags & NHEXT_IO_FMT_HEX ? 16 : 10;
    static const char *lc_digits = "0123456789abcdef";
    static const char *uc_digits = "0123456789ABCDEF";
    const char *digits = flags & NHEXT_IO_FMT_CAPS ? uc_digits : lc_digits;
    char buffer[8 * sizeof(v) / 3 + 1];		/* Recalculate if radix < 10 */
    char *p;
    if (!(flags & NHEXT_IO_FMT_UNSIGNED) && (long)v < 0) {
	flags |= NHEXT_IO_FMT_NEG;
	v = -(long)v;
    }
    p = buffer + sizeof(buffer);
    w = 0;
    do {
	digit = v % radix;
	v /= radix;
	*--p = digits[digit];
	w++;
    } while (v || flags & NHEXT_IO_FMT_PTR && w < sizeof(void *) * 2);
    padding = width - w;
    i = nhext_io_pad(io, flags, padding, TRUE);
    if (i < 0)
	return -1;
    else
	retval += i;
    if (nhext_io_write(io, p, w) != w)
	return -1;
    else
	retval += w;
    i = nhext_io_pad(io, flags, padding, FALSE);
    if (i < 0)
	return -1;
    else
	retval += i;
    return retval;
}

/*
 * A cut down version of printf. Not all ANSI C features are implemented.
 * Use with caution.
 */

int nhext_io_vprintf(NhExtIO *io, char *fmt, va_list ap)
{
    int i, j, nb = 0;
    unsigned long ul;
    char *s;
    int flags, width, precision;
#ifdef PRINTF_FP_SUPPORT
    double d;
    int padding, expon, dp, sgn;
    static int exp_width = 0;
    if (!exp_width) {
	i = DBL_MAX_10_EXP;
	while (i) {
	    i /= 10;
	    exp_width++;
	}
	exp_width++;
    }
#endif
    while(*fmt) {
	for(i = 0; fmt[i] && fmt[i] != '%'; i++)
	    ;
	if (i) {
	    if (nhext_io_writet(io, fmt, i) != i)
		return -1;
	    else
		nb += i;
	    fmt += i;
	}
	if (!*fmt)
	    break;
	flags = 0;
	while (!(flags & NHEXT_IO_FMT_DONE) && *++fmt) {
	    switch(*fmt) {
		case '-':
		    flags |= NHEXT_IO_FMT_LJUST;
		    break;
		case '+':
		    flags |= NHEXT_IO_FMT_SIGN;
		    break;
		case '#':
		    flags |= NHEXT_IO_FMT_ALT;
		    break;
		case '.':
		    flags |= NHEXT_IO_FMT_PRECISION;
		    precision = 0;
		    break;
		case '*':
		    if (flags & NHEXT_IO_FMT_PRECISION)
			precision = va_arg(ap, int);
		    else {
			flags |= NHEXT_IO_FMT_WIDTH;
			width = va_arg(ap, int);
			if (width < 0) {
			    flags |= NHEXT_IO_FMT_LJUST;
			    width = -width;
			}
		    }
		    break;
		case '0':
		    if (flags & NHEXT_IO_FMT_PRECISION)
			precision *= 10;
		    else if (flags & NHEXT_IO_FMT_WIDTH)
			width *= 10;
		    else
			flags |= NHEXT_IO_FMT_ZEROPAD;
		    break;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		    if (flags & NHEXT_IO_FMT_PRECISION) {
			precision *= 10;
			precision += *fmt - '0';
		    } else if (flags & NHEXT_IO_FMT_WIDTH) {
			width *= 10;
			width += *fmt - '0';
		    } else {
			flags |= NHEXT_IO_FMT_WIDTH;
			width = *fmt - '0';
		    }
		    break;
		case 'h':
		    flags |= NHEXT_IO_FMT_SHORT;
		    flags &= ~NHEXT_IO_FMT_LONG;
		    break;
		case 'l':
		    flags |= NHEXT_IO_FMT_LONG;
		    flags &= ~NHEXT_IO_FMT_SHORT;
		    break;
		case 'c':
		    i = nhext_io_pad(io, flags, width - 1, TRUE);
		    if (i < 0)
			return -1;
		    else
			nb += i;
		    if (nhext_io_fputc((char)va_arg(ap, int), io) < 0)
			return -1;
		    else
			nb++;
		    i = nhext_io_pad(io, flags, width - 1, FALSE);
		    if (i < 0)
			return -1;
		    else
			nb += i;
		    flags |= NHEXT_IO_FMT_DONE;
		    break;
		case 'P':
		case 'p':
		case 'X':
		case 'x':
		    if (*fmt == 'p' || *fmt == 'P')
			flags |= NHEXT_IO_FMT_PTR;
		    if (*fmt == 'P' || *fmt == 'X')
			flags |= NHEXT_IO_FMT_CAPS;
		    flags |= NHEXT_IO_FMT_HEX;
		    /* Fall through */
		case 'u':
		    flags |= NHEXT_IO_FMT_UNSIGNED;
		    flags &= ~NHEXT_IO_FMT_SIGN;
		    /* Fall through */
		case 'd':
		case 'i':
		    if (flags & NHEXT_IO_FMT_LONG)
			ul = va_arg(ap, long);
		    else if (flags & NHEXT_IO_FMT_UNSIGNED)
			ul = va_arg(ap, unsigned int);
		    else
			ul = (long)va_arg(ap, int);
		    i = nhext_io_printi(io, flags, width, ul);
		    if (i < 0)
			return -1;
		    else
			nb += i;
		    flags |= NHEXT_IO_FMT_DONE;
		    break;
#ifdef PRINTF_FP_SUPPORT
		case 'E':
		case 'F':
		case 'G':
		    flags |= NHEXT_IO_FMT_CAPS;
		    /* Fall through */
		case 'e':
		case 'f':
		case 'g':
		    if (!(flags & NHEXT_IO_FMT_PRECISION))
			precision = 6;
		    d = va_arg(ap, double);
		    s = fcvt(d, precision, &dp, &sgn);
		    if (*fmt == 'f')
			j = strlen(s);
		    else {
			j = precision;
			if (*fmt == 'e' || *fmt == 'E')
			    j++;
			s = fcvt(d, j - dp, &dp, &sgn);
			if (j > precision || dp > precision || dp < -3) {
			    flags |= NHEXT_IO_FMT_EXP;
			    expon = dp - 1;
			    dp = 1;
			}
		    }
		    if (sgn)
			flags |= NHEXT_IO_FMT_NEG;
		    padding = width - j;
		    if (dp <= 0)
			padding += dp - 2;
		    else if (dp < j)
			padding--;
		    if (flags & NHEXT_IO_FMT_EXP)
			padding -= 5;
		    i = nhext_io_pad(io, flags, padding, TRUE);
		    if (i < 0)
			return -1;
		    else
			nb += i;
		    if (dp <= 0) {
			if (nhext_io_write(io, "0.", 2) != 2)
			    return -1;
			for(i = dp; i < 0; i++)
			    if (nhext_io_fputc('0', io) < 0)
				return -1;
			if (nhext_io_write(io, s, j) != j)
			    return -1;
			nb += 2 - dp;
		    } else if (dp < j) {
			if (nhext_io_write(io, s, dp) != dp)
			    return -1;
			if (nhext_io_fputc('.', io) < 0)
			    return -1;
			if (nhext_io_write(io, s + dp, j - dp) != j - dp)
			    return -1;
			nb++;
		    } else if (nhext_io_write(io, s, j) != j)
			return -1;
		    nb += j;
		    if (flags & NHEXT_IO_FMT_EXP) {
			if (nhext_io_fputc(
			  flags & NHEXT_IO_FMT_CAPS ? 'E' : 'e', io) < 0)
			    return -1;
			if (nhext_io_printi(io, NHEXT_IO_FMT_ZEROPAD |
			  NHEXT_IO_FMT_WIDTH | NHEXT_IO_FMT_SIGN, exp_width,
			  (long)expon) < 0)
			    return -1;
		    }
		    i = nhext_io_pad(io, flags, padding, FALSE);
		    if (i < 0)
			return -1;
		    else
			nb += i;
		    flags |= NHEXT_IO_FMT_DONE;
		    break;
#endif	/* PRINTF_FP_SUPPORT */
		case 's':
		    s = va_arg(ap, char *);
		    if (flags & NHEXT_IO_FMT_PRECISION)
			j = precision;
		    else
			j = s ? strlen(s) : 6;
		    i = nhext_io_pad(io, flags, width - j, TRUE);
		    if (i < 0)
			return -1;
		    else
			nb += i;
		    if (nhext_io_writet(io, s ? s : "(null)", j) != j)
			return -1;
		    else
			nb += j;
		    i = nhext_io_pad(io, flags, width - j, FALSE);
		    if (i < 0)
			return -1;
		    else
			nb += i;
		    flags |= NHEXT_IO_FMT_DONE;
		    break;
		case '%':
		default:
		    if (nhext_io_fputc(*fmt, io) < 0)
			return -1;
		    else
			nb++;
		    flags |= NHEXT_IO_FMT_DONE;
		    break;
	    }
	}
	if (flags & NHEXT_IO_FMT_DONE)
	    fmt++;
    }
    return nb;
}

int nhext_io_printf(NhExtIO *io, char *fmt, ...)
{
    int retval;
    va_list ap;
    va_start(ap, fmt);
    retval = nhext_io_vprintf(io, fmt, ap);
    va_end(ap);
    return retval;
}
