/* $Id: test_io.c,v 1.3 2003-10-25 18:06:02 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2002-2003 */
/* NetHack may be freely redistributed.  See license for details. */

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include "nhxdr.h"
#include "test_com.h"

/* #define PRINTF_FP_SUPPORT */		/* Support for %e, %f & %g */

/*
 * This module tests the NhExtIO implementation by using it to write
 * and read data via pipes to and from a child process. This setup
 * allows us to write large packets without causing an infinite block
 * while still failing if NhExtIO attempts to read data ahead of time.
 */

#ifdef SIGPIPE
static void
sigpipe_handler(int signum)
{
    fprintf(stderr, "%s: SIGPIPE received\n", is_child ? "child" : "parent");
}
#endif

static int
child(NhExtIO *rio, NhExtIO *wio)
{
    int i, j, k, r, nb, exitcode = 1;
    unsigned char buffer[8192];
    unsigned long crc, crc1;		/* AUTODIN-II 32-bit CRC */
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
    if (nhext_io_filbuf(rio, TRUE) < 0) {
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
	    fprintf(stderr,"%s", "child: nhext_io_read(random): Bad CRC\n");
	    goto done;
	}
    }
    exitcode = 0;
    fprintf(stderr,"%s", "No errors found on child side\n");
done:
    return exitcode;
}

static int
parent(NhExtIO *rio, NhExtIO *wio)
{
    int c, nb, exitcode = 1;
    unsigned char buffer[32];
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
	  "parent: nhext_io_read(start): "
	  "Expecting Start\\n, got \"%*.*s\" (%d bytes)\n",
	  nb,nb,buffer,nb);
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
	    nb = nhext_io_filbuf(rio, TRUE);
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
    fprintf(stderr,"%s", "No errors found on parent side\n");
done:
    return exitcode;
}

int fd_write(void *handle, void *buf, unsigned int len)
{
    int retval;
    retval = write((int)handle, buf, len);
    return retval >= 0 ? retval : -1;
}

test_printf()
{
    NhExtIO *wr;
    wr = nhext_io_open(fd_write, (void *)1, NHEXT_IO_WRONLY);
#ifdef PRINTF_FP_SUPPORT
    printf("Test of %%f output\n"
           "-----------------\n");
    printf("printf:\n");
    printf(">%-8.4lf<\n", 3.1415926);
    printf(">%8.4lf<\n", -3.1415926);
    printf(">%8.4lf<\n", 31415926.0);
    printf(">%8.4lf<\n", 0.00031415926);
    printf(">%8.4lf<\n", 0.000031415926);
    nhext_io_printf(wr, "nhext_io_printf:\n");
    nhext_io_printf(wr, ">%-8.4lf<\n", 3.1415926);
    nhext_io_printf(wr, ">%8.4lf<\n", -3.1415926);
    nhext_io_printf(wr, ">%8.4lf<\n", 31415926.0);
    nhext_io_printf(wr, ">%8.4lf<\n", 0.00031415926);
    nhext_io_printf(wr, ">%8.4lf<\n", 0.000031415926);
    nhext_io_flush(wr);
    printf("\nTest of %%e output\n"
           "-----------------\n");
    printf("printf:\n");
    printf(">%-8.4le<\n", 3.1415926);
    printf(">%8.4le<\n", -3.1415926);
    printf(">%8.4le<\n", 31415926.0);
    printf(">%8.4le<\n", 0.00031415926);
    printf(">%8.4le<\n", 0.000031415926);
    nhext_io_printf(wr, "nhext_io_printf:\n");
    nhext_io_printf(wr, ">%-8.4le<\n", 3.1415926);
    nhext_io_printf(wr, ">%8.4le<\n", -3.1415926);
    nhext_io_printf(wr, ">%8.4le<\n", 31415926.0);
    nhext_io_printf(wr, ">%8.4le<\n", 0.00031415926);
    nhext_io_printf(wr, ">%8.4le<\n", 0.000031415926);
    nhext_io_flush(wr);
    printf("\nTest of %%g output\n"
           "-----------------\n");
    printf("printf:\n");
    printf(">%-8.4lg<\n", 3.1415926);
    printf(">%8.4lg<\n", -3.1415926);
    printf(">%8.4lg<\n", 31415926.0);
    printf(">%8.4lg<\n", 0.00031415926);
    printf(">%8.4lg<\n", 0.000031415926);
    nhext_io_printf(wr, "nhext_io_printf:\n");
    nhext_io_printf(wr, ">%-8.4lg<\n", 3.1415926);
    nhext_io_printf(wr, ">%8.4lg<\n", -3.1415926);
    nhext_io_printf(wr, ">%8.4lg<\n", 31415926.0);
    nhext_io_printf(wr, ">%8.4lg<\n", 0.00031415926);
    nhext_io_printf(wr, ">%8.4lg<\n", 0.000031415926);
    nhext_io_flush(wr);
#endif
    printf("\nTest of %%d output\n"
           "-----------------\n");
    printf("printf:\n");
    printf(">%-8d<\n", 314159);
    printf(">%8d<\n", -314159);
    printf(">%08d<\n", 314159);
    printf(">%-08d<\n", 314159);
    printf(">%+08d<\n", 314159);
    nhext_io_printf(wr, "nhext_io_printf:\n");
    nhext_io_printf(wr, ">%-8d<\n", 314159);
    nhext_io_printf(wr, ">%8d<\n", -314159);
    nhext_io_printf(wr, ">%08d<\n", 314159);
    nhext_io_printf(wr, ">%-08d<\n", 314159);
    nhext_io_printf(wr, ">%+08d<\n", 314159);
    nhext_io_flush(wr);
    printf("\nTest of %%lu output\n"
           "-----------------\n");
    printf("printf:\n");
    printf(">%-8lu<\n", 314159UL);
    printf(">%8lu<\n", 314159UL);
    printf(">%08lu<\n", 314159UL);
    printf(">%-08lu<\n", 314159UL);
    printf(">%+08lu<\n", 314159UL);
    nhext_io_printf(wr, "nhext_io_printf:\n");
    nhext_io_printf(wr, ">%-8lu<\n", 314159UL);
    nhext_io_printf(wr, ">%8lu<\n", 314159UL);
    nhext_io_printf(wr, ">%08lu<\n", 314159UL);
    nhext_io_printf(wr, ">%-08lu<\n", 314159UL);
    nhext_io_printf(wr, ">%+08lu<\n", 314159UL);
    nhext_io_flush(wr);
    printf("\nTest of %%x output\n"
           "-----------------\n");
    printf("printf:\n");
    printf(">%-8x<\n", 314159U);
    printf(">%8X<\n", 314159U);
    printf(">%08x<\n", 314159U);
    printf(">%#-08X<\n", 314159U);
    printf(">%#08x<\n", 314159U);
    nhext_io_printf(wr, "nhext_io_printf:\n");
    nhext_io_printf(wr, ">%-8x<\n", 314159U);
    nhext_io_printf(wr, ">%8X<\n", 314159U);
    nhext_io_printf(wr, ">%08x<\n", 314159U);
    nhext_io_printf(wr, ">%#-08X<\n", 314159U);
    nhext_io_printf(wr, ">%#08x<\n", 314159U);
    nhext_io_flush(wr);
    printf("\nTest of %%p output\n"
           "-----------------\n");
    printf("printf:\n");
    printf(">%-9p<\n", wr);
    printf(">%9P<\n", wr);
    printf(">%09p<\n", wr);
    printf(">%#-09P<\n", wr);
    printf(">%#09p<\n", wr);
    nhext_io_printf(wr, "nhext_io_printf:\n");
    nhext_io_printf(wr, ">%-9p<\n", wr);
    nhext_io_printf(wr, ">%9P<\n", wr);
    nhext_io_printf(wr, ">%09p<\n", wr);
    nhext_io_printf(wr, ">%#-09P<\n", wr);
    nhext_io_printf(wr, ">%#09p<\n", wr);
    nhext_io_flush(wr);
    printf("\nTest of %%s output\n"
           "-----------------\n");
    printf("printf:\n");
    printf(">%*s<\n", -9, "Hello");
    printf(">%9s<\n", "Hello");
    printf(">%09s<\n", "Hello");
    printf(">%-*s<\n", 9, "Hello");
    printf(">%9.*s<\n", 4, "Hello");
    printf(">%s<\n", (char *)0);
    nhext_io_printf(wr, "nhext_io_printf:\n");
    nhext_io_printf(wr, ">%*s<\n", -9, "Hello");
    nhext_io_printf(wr, ">%9s<\n", "Hello");
    nhext_io_printf(wr, ">%09s<\n", "Hello");
    nhext_io_printf(wr, ">%-*s<\n", 9, "Hello");
    nhext_io_printf(wr, ">%9.*s<\n", 4, "Hello");
    nhext_io_printf(wr, ">%s<\n", (char *)0);
    nhext_io_flush(wr);
    nhext_io_close(wr);
}

int
main(argc, argv)
int argc;
char **argv;
{
    int retval = 0;
    NhExtIO *rd, *wr;
#ifdef SIGPIPE
    signal(SIGPIPE, sigpipe_handler);
#endif
    if (argc > 1 && !strcmp(argv[1], "-c")) {
	is_child++;
	rd = nhext_io_open(parent_read, get_parent_readh(),
	  NHEXT_IO_RDONLY | NHEXT_IO_NOAUTOFILL);
	wr = nhext_io_open(parent_write, get_parent_writeh(), NHEXT_IO_WRONLY);
	if (!rd || !wr) {
	    fprintf(stderr,"%s", "C Failed to open I/O streams.\n");
	    exit(1);
	}
	retval = child(rd, wr);
	nhext_io_close(rd);
	nhext_io_close(wr);
	exit(retval);
    }
    if (!child_start(argv[0])) {
	fprintf(stderr,"%s", "Failed to start child.\n");
	exit(1);
    }
    rd = nhext_io_open(child_read, get_child_readh(), NHEXT_IO_RDONLY);
    wr = nhext_io_open(child_write, get_child_writeh(), NHEXT_IO_WRONLY);
    if (!rd || !wr) {
	fprintf(stderr,"%s", "Failed to open I/O streams.\n");
	exit(1);
    }
    retval = parent(rd, wr);
    nhext_io_close(rd);
    nhext_io_close(wr);
    test_printf();
    exit(retval);
}
