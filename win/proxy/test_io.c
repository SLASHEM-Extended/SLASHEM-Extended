/* $Id: test_io.c,v 1.2 2002-11-23 22:41:59 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2002 */
/* NetHack may be freely redistributed.  See license for details. */

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include "nhxdr.h"

/*
 * This module tests the NhExtIO implementation by using it to write
 * and read data via pipes to and from a child process. This setup
 * allows us to write large packets without causing an infinite block
 * while still failing if NhExtIO attempts to read data ahead of time.
 */

static int is_child;

static void
sigpipe_handler(int signum)
{
    fprintf(stderr, "%s: SIGPIPE received\n", is_child ? "child" : "parent");
}

static int
read_f(void *handle, void *buf, unsigned int len)
{
    return read((int)handle, buf, len);
}

static int
write_f(void *handle, void *buf, unsigned int len)
{
    return write((int)handle, buf, len);
}

static int
child(int rfd, int wfd)
{
    NhExtIO *rio, *wio;
    int i, j, k, r, nb, exitcode = 1;
    unsigned char buffer[8192];
    unsigned long crc, crc1;		/* AUTODIN-II 32-bit CRC */
    rio = nhext_io_open(read_f, (void *)rfd,
      NHEXT_IO_RDONLY | NHEXT_IO_NOAUTOFILL);
    if (!rio) {
	perror("child: nhext_io_open(read)");
	goto done;
    }
    wio = nhext_io_open(write_f, (void *)wfd, NHEXT_IO_WRONLY);
    if (!wio) {
	perror("child: nhext_io_open(write)");
	goto done;
    }
    nb = nhext_io_read(rio, buffer, sizeof(buffer));
    if (nb) {
	/*
	 * auto-fill is off. nb should be zero to indicate EOF.
	 * If NhExtIO returns anything else, there's something wrong.
	 * Should NhExtIO actually attempt a read it will cause an
	 * infinite block since the parent process won't write
	 * anything until it gets the start command.
	 */
	if (nb < 0)
	    perror("child: nhext_io_read(EOF)");
	else
	    fprintf(stderr,
	      "child: nhext_io_read(EOF): Expecting EOF, got %d bytes\n", nb);
	goto done;
    }
    if (nhext_io_write(wio, "Start\n", 6) != 6) {
	perror("child: write(start)");
	goto done;
    }
    if (nhext_io_flush(wio)) {
	perror("child: flush(start)");
	goto done;
    }
    if (nhext_io_filbuf(rio) < 0) {
	perror("child: filbuf");
	goto done;
    }
    r = nhext_io_read(rio, buffer, sizeof(buffer));
    if (r <= 0) {
	if (r < 0)
	    perror("child: nhext_io_read(ACK)");
	else
	    fprintf(stderr,
	      "child: nhext_io_read(ACK): Expecting ACK, got EOF\n");
	goto done;
    }
    if (r != 4 || memcmp(buffer, "ACK\n", 4)) {
	fprintf(stderr,
	  "child: nhext_io_read(ACK): Expecting ACK, got %*.*s\n",r,r,buffer);
	goto done;
    }
    if (nhext_io_getmode(rio) != (NHEXT_IO_RDONLY | NHEXT_IO_NOAUTOFILL)) {
	fprintf(stderr,
	  "child: nhext_io_getmode: Bad mode (%d)\n", nhext_io_getmode(rio));
	goto done;
    }
    nhext_io_setmode(rio, 0);
    if (nhext_io_getmode(rio) != NHEXT_IO_RDONLY) {
	fprintf(stderr,
	  "child: nhext_io_setmode: Failed (now %d)\n", nhext_io_getmode(rio));
	goto done;
    }
    for(i = 0; i < 10000; i++) {
	if (i%100 == 0)
	    fprintf(stderr, "Packet %d\n", i);
	/* A random sized packet */
	nb = 1 + (int)(rand() * 8192.0 / (RAND_MAX + 1.0));
	/* Containing random data */
	crc = 0;
	for(j = 0; j < nb; j++) {
	    buffer[j] = (int)(rand() * 256.0 / (RAND_MAX + 1.0));
	    crc ^= buffer[j] << 24;
	    for(k = 0; k < 8; k++) {
		if (crc & 1UL << 31) {
		    crc <<= 1;
		    crc ^= 0x4C11DB7;
		} else
		    crc <<= 1;
	    }
	}
	/* Write packet to parent */
	j = 1 + (int)(rand() * (double)nb / (RAND_MAX + 1.0));
	if (nhext_io_write(wio, buffer, j) != j) {
	    perror("child: write(random)");
	    goto done;
	}
	if (j != nb) {
	    if (nhext_io_write(wio, buffer + j, nb - j) != nb - j) {
		perror("child: write(random)");
		goto done;
	    }
	}
	if (nhext_io_flush(wio)) {
	    perror("child: flush(random)");
	    goto done;
	}
	/* Read back the XORed data */
	k = 0;					/* Total read so far */
	do {
	    j = 1 + (int)(rand() * (double)(sizeof(buffer) - k) /
	      (RAND_MAX + 1.0));		/* Bytes this fragment */
	    r = nhext_io_read(rio, buffer + k, j);
	    if (r <= 0) {
		if (r < 0)
		    perror("child: nhext_io_read(random)");
		else
		    fprintf(stderr,
		      "child: nhext_io_read(random): Expecting data, got EOF\n");
		goto done;
	    }
	    k += r;
	} while (k < nb);
	if (k != nb) {
	    fprintf(stderr,
	      "child: nhext_io_read(random): Expecting %d bytes, got %d\n",
	      nb, k);
	    goto done;
	}
	/* Reverse the XOR and compute CRC */
	crc1 = 0;
	for(j = 0; j < nb; j++) {
	    buffer[j] ^= 0x8D;
	    crc1 ^= buffer[j] << 24;
	    for(k = 0; k < 8; k++) {
		if (crc1 & 1UL << 31) {
		    crc1 <<= 1;
		    crc1 ^= 0x4C11DB7;
		} else
		    crc1 <<= 1;
	    }
	}
	if (crc1 != crc) {
	    fprintf(stderr, "child: nhext_io_read(random): Bad CRC\n");
	    goto done;
	}
    }
    exitcode = 0;
    fprintf(stderr, "No errors found on child side\n");
done:
    if (rio)
	nhext_io_close(rio);
    if (wio)
	nhext_io_close(wio);
    exit(exitcode);
}

static int
parent(int rfd, int wfd)
{
    NhExtIO *rio, *wio;
    int c, nb, exitcode = 1;
    unsigned char buffer[32];
    rio = nhext_io_open(read_f, (void *)rfd, NHEXT_IO_RDONLY);
    if (!rio) {
	perror("parent: nhext_io_open(read)");
	goto done;
    }
    wio = nhext_io_open(write_f, (void *)wfd, NHEXT_IO_WRONLY);
    if (!wio) {
	perror("parent: nhext_io_open(write)");
	goto done;
    }
    nb = nhext_io_read(rio, buffer, sizeof(buffer));
    if (nb <= 0) {
	if (nb < 0)
	    perror("parent: nhext_io_read(start)");
	else
	    fprintf(stderr,
	      "parent: nhext_io_read(start): Expecting Start, got EOF\n");
	goto done;
    }
    if (nb != 6 || memcmp(buffer, "Start\n", 6)) {
	fprintf(stderr,
	  "child: nhext_io_read(start): Expecting Start, got %*.*s\n",
	  nb,nb,buffer);
	goto done;
    }
    if (nhext_io_write(wio, "ACK\n", 4) != 4) {
	perror("parent: write(ACK)");
	goto done;
    }
    if (nhext_io_flush(wio)) {
	perror("parent: flush(ACK)");
	goto done;
    }
    if (nhext_io_getmode(rio) != NHEXT_IO_RDONLY) {
	fprintf(stderr,
	  "parent: nhext_io_getmode: Bad mode (%d)\n", nhext_io_getmode(rio));
	goto done;
    }
    nhext_io_setmode(rio, NHEXT_IO_NOAUTOFILL);
    if (nhext_io_getmode(rio) != (NHEXT_IO_RDONLY | NHEXT_IO_NOAUTOFILL)) {
	fprintf(stderr,
	  "parent: nhext_io_setmode: Failed (now %d)\n", nhext_io_getmode(rio));
	goto done;
    }
    for(;;) {
	/* Read byte from child */
	c = nhext_io_getc(rio);
	if (c < 0) {
	    if (nhext_io_flush(wio)) {
		perror("parent: flush(random)");
		goto done;
	    }
	    nb = nhext_io_filbuf(rio);
	    if (nb < 0) {
		perror("parent: filbuf");
		goto done;
	    } else if (!nb)
		break;		/* EOF */
	    c = nhext_io_getc(rio);
	    if (c < 0) {
		perror("parent: getc(random)");
		goto done;
	    }
	}
	/* XOR it */
	c ^= 0x8D;
	/* Write it back to child */
	if (nhext_io_fputc(c, wio) < 0) {
	    perror("parent: fputc(random)");
	    goto done;
	}
    }
    exitcode = 0;
    fprintf(stderr, "No errors found on parent side\n");
done:
    if (rio)
	nhext_io_close(rio);
    if (wio)
	nhext_io_close(wio);
    exit(exitcode);
}

int
main(argc, argv)
int argc;
char **argv;
{
    int to_child[2], from_child[2];
    if (pipe(to_child) || pipe(from_child)) {
	perror("pipe");
	exit(1);
    }
    if (!fork()) {
	is_child = 1;
	signal(SIGPIPE, sigpipe_handler);
	close(to_child[1]);
	close(from_child[0]);
	child(to_child[0], from_child[1]);
    } else {
	is_child = 0;
	signal(SIGPIPE, sigpipe_handler);
	close(to_child[0]);
	close(from_child[1]);
	parent(from_child[0], to_child[1]);
    }
}
