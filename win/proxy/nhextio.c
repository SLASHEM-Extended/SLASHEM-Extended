/* $Id: nhextio.c,v 1.2 2002-11-02 15:47:03 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2002 */
/* NetHack may be freely redistributed.  See license for details. */

/* NhExt: buffering support */

#include <stdlib.h>
#include <limits.h>
#include "nhxdr.h"
#include "proxycom.h"

struct NhExtIO_ {
    unsigned int flags;
    nhext_io_func func;
    void *handle;
    unsigned char buffer[1025];		/* Capable of storing n-1 chars */
    unsigned char *rp, *wp;
};

#define ADVANCE_PTR(io, ptr, inc)				\
	do {							\
	    (ptr) += (inc);					\
	    if ((ptr) >= (io)->buffer + sizeof((io)->buffer))	\
		(ptr) -= sizeof((io)->buffer);			\
	} while(0)

/* Flags changeable by nhext_io_setmode */

#define NHEXT_IO__USERFLAGS	NHEXT_IO_NOAUTOFILL

NhExtIO *nhext_io_open(nhext_io_func func, void *handle, unsigned int flags)
{
    NhExtIO *io;
    io = malloc(sizeof(*io));
    if (!io)
	return NULL;
    io->flags = flags;
    io->func = func;
    io->handle = handle;
    io->rp = io->wp = io->buffer;
    return io;
}

int nhext_io_close(NhExtIO *io)
{
    int retval;
    retval = nhext_io_flush(io);
    free(io);
    return retval;
}

unsigned int nhext_io_getmode(NhExtIO *io)
{
    return io->flags;
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

/*
 * Returns: >0 if buffer now non-empty, else 0 on EOF, <0 on error
 */

int nhext_io_filbuf(NhExtIO *io)
{
    int retval, nf1, nf2;
    if (io->rp == io->wp)
	io->rp = io->wp = io->buffer;
    nhext_io__getfree(io, &nf1, &nf2);
    if (nf2) {
	if (nf1) {
	    /* Combine free blocks so that we don't make unneccesary
	     * calls to I/O function (which may be expensive).
	     */
	    memmove(io->buffer, io->rp, io->wp - io->rp);
	    io->wp += io->buffer - io->rp;
	    io->rp = io->buffer;
	}
	nf1 += nf2;
	/* nf2 = 0 */
    }
    if (!nf1)
	return 1;	/* Buffer is full */
    retval = (*io->func)(io->handle, io->wp, nf1);
    if (retval > 0)
	ADVANCE_PTR(io, io->wp, retval);
    return retval;
}

int nhext_io_getc(NhExtIO *io)
{
    unsigned char c;
    if (io->rp == io->wp &&
      (io->flags & NHEXT_IO_NOAUTOFILL || nhext_io_filbuf(io) <= 0))
	return -1;		/* getc doesn't distinguish EOF and ERROR */
    c = *io->rp;
    ADVANCE_PTR(io, io->rp, 1);
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
    /* The buffer is empty.
     * If we've read some data or we shouldn't auto-fill then we're done
     */
    if (retval || io->flags & NHEXT_IO_NOAUTOFILL)
	return retval;
    if (nb >= sizeof(io->buffer)) {
	/* If caller still wants more than we can buffer, read direct */
	i = (*io->func)(io->handle, buf, nb);
	if (i <= 0)
	    /* If we have previously read some data correctly, then return
	     * this number and leave EOF/ERROR reporting until later.
	     * Otherwise report EOF as 0 and ERROR as -1
	     */
	    return retval ? retval : i < 0 ? -1 : 0;
	buf += i;
	nb -= i;
	retval += i;
    } else if (nb) {
	/* Caller wants a fragment; fill our buffer and then satisfy from there.
	 */
	i = nhext_io_filbuf(io);
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
		memcpy(io->buffer, buffer - nbp, nbp);
		io->rp = io->buffer;
		io->wp = io->buffer + nbp;
		return nmemb - nm;
	    }
	    nbp += nb;
	    buffer += nb;
	    if (nbp == size) {
		nm++;
		nbp = 0;
	    }
	}
    }
    return nmemb;
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
