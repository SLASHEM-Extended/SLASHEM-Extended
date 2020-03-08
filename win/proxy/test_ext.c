/* $Id: test_ext.c,v 1.8 2003-10-25 18:06:01 j_ali Exp $ */
/* Copyright (c) Slash'EM Development Team 2001-2003 */
/* NetHack may be freely redistributed.  See license for details. */

/*
 * This module tests the NhExt support routines for sub-protocol 1.
 * These include nhext_init(), nhext_end() nhext_rpc() and nhext_svc(),
 * all of which can be found in nhext.c.
 *
 * Note: This module does not test the implementation of sub-protocol 1
 * itself (which is found in winproxy.c) or the support for sub-protocol 2.
 */

#include <stdio.h>
#include <stdarg.h>

#include "hack.h"
#include "nhxdr.h"
#include "proxycom.h"
#include "test_com.h"

volatile server_exit = 0;

#define EXT_FID_EXIT	0x7FFF
#define EXT_FID_TEST1	1
#define EXT_FID_TEST2	2
#define EXT_FID_TEST3	3
#define EXT_FID_TEST4	4
#define EXT_FID_TEST5	5
#define EXT_FID_TEST6	6
#define EXT_FID_TEST7	7

#define EXT_CID_TEST7	0x1007

void test7_callback(unsigned short id, NhExtXdr *request, NhExtXdr *reply)
{
    int i;
    nhext_rpc_params(request, 1, EXT_INT_P(i));
    nhext_rpc_params(reply, 1, EXT_INT(i * 5 + 3));
}

struct nhext_svc callbacks[] = {
    EXT_CID_TEST7,	test7_callback,
    0, NULL,
};

void svc_exit(unsigned short id, NhExtXdr *request, NhExtXdr *reply)
{
    server_exit = 1;
    nhext_rpc_params(reply, 0);
}

void svc_test1(unsigned short id, NhExtXdr *request, NhExtXdr *reply)
{
    nhext_rpc_params(reply, 0);
}

void svc_test2(unsigned short id, NhExtXdr *request, NhExtXdr *reply)
{
    int i;
    nhext_rpc_params(request, 1, EXT_INT_P(i));
    nhext_rpc_params(reply, 0);
}

void svc_test3(unsigned short id, NhExtXdr *request, NhExtXdr *reply)
{
    int i;
    nhext_rpc_params(request, 1, EXT_INT_P(i));
    nhext_rpc_params(reply, 1, EXT_INT(i + 1));
}

void svc_test4(unsigned short id, NhExtXdr *request, NhExtXdr *reply)
{
    char *s;
    char *buf;
    nhext_rpc_params(request, 1, EXT_STRING_P(s));
    buf = (char *)alloc(strlen(s) + 3);
    sprintf(buf, "<%s>", s);
    free(s);
    nhext_rpc_params(reply, 1, EXT_STRING(buf));
    free(buf);
}

struct test5_request {
    unsigned int n;
    long *array;
};

int svc_xdr_test5_request(xdrs, datum)
NhExtXdr *xdrs;
struct test5_request *datum;
{
    return nhext_xdr_array(xdrs, (char **)&datum->array, &datum->n,
      (unsigned int)-1, sizeof(long), nhext_xdr_long);
}

void svc_test5(unsigned short id, NhExtXdr *request, NhExtXdr *reply)
{
    int i;
    int total = 0;
    struct test5_request req = { 0, (long *)0 };
    nhext_rpc_params(request, 1, EXT_XDRF(svc_xdr_test5_request, &req));
    for(i = 0; i < req.n; i++)
	total += req.array[i];
    nhext_rpc_params(reply, 1, EXT_INT(total));
}

void svc_test6(unsigned short id, NhExtXdr *request, NhExtXdr *reply)
{
    int i;
    char *s;
    winid w;
    nhext_xdr_bool_t b;
    char c;
    nhext_rpc_params(request, 5, EXT_INT_P(i), EXT_INT_P(w), EXT_CHAR_P(c),
      EXT_STRING_P(s), EXT_BOOLEAN_P(b));
    nhext_rpc_params(reply, 5, EXT_INT(w), EXT_STRING(s), EXT_CHAR(c),
      EXT_INT(i), EXT_BOOLEAN(b));
    free(s);
}

void svc_test7(unsigned short id, NhExtXdr *request, NhExtXdr *reply)
{
    int i;
    nhext_rpc_params(request, 1, EXT_INT_P(i));
    nhext_rpc(EXT_CID_TEST7, 1, EXT_INT(i), 1, EXT_INT_P(i));
    nhext_rpc_params(reply, 1, EXT_INT(i));
}

struct nhext_svc services[] = {
    EXT_FID_EXIT,	svc_exit,
    EXT_FID_TEST1,	svc_test1,
    EXT_FID_TEST2,	svc_test2,
    EXT_FID_TEST3,	svc_test3,
    EXT_FID_TEST4,	svc_test4,
    EXT_FID_TEST5,	svc_test5,
    EXT_FID_TEST6,	svc_test6,
    EXT_FID_TEST7,	svc_test7,
    0,			NULL,
};

void server(void)
{
    int i;
    NhExtIO *rd, *wr;
    rd = nhext_io_open(parent_read, get_parent_readh(), NHEXT_IO_RDONLY);
    wr = nhext_io_open(parent_write, get_parent_writeh(), NHEXT_IO_WRONLY);
    if (!rd || !wr) {
	fprintf(stderr,"%s", "C Failed to open I/O streams.\n");
	exit(1);
    }
    (void)nhext_set_errhandler(nhext_error_handler);
    if (nhext_init(rd, wr, callbacks) < 0) {
	fprintf(stderr,"%s", "C Failed to initialize NhExt.\n");
	exit(1);
    }
    if (nhext_set_protocol(1)) {
	fprintf(stderr,"%s", "C Failed to select protocol 1.\n");
	exit(1);
    }
    do {
	i = nhext_svc(services);
	if (!i)
	    impossible("Ignoring packet with zero ID");
    } while (!server_exit);
    nhext_end();
    nhext_io_close(rd);
    nhext_io_close(wr);
}

void run_tests(void)
{
    int i, retval;
    char c, *s;
    nhext_xdr_bool_t b;
    winid w;
    int total;
    struct test5_request req;
    fprintf(stderr,"%s", "Test 1...\n");
    retval = nhext_rpc(EXT_FID_TEST1, 0, 0);
    fprintf(stderr, "Test 1 %s.\n", retval ? "passed" : "failed");
    fprintf(stderr,"%s", "Test 2...\n");
    retval = nhext_rpc(EXT_FID_TEST2, 1, EXT_INT(0), 0);
    fprintf(stderr, "Test 2 %s.\n", retval ? "passed" : "failed");
    fprintf(stderr,"%s", "Test 3...\n");
    retval = nhext_rpc(EXT_FID_TEST3, 1, EXT_INT(67), 1, EXT_INT_P(i));
    if (i != 68)
	retval = FALSE;
    fprintf(stderr, "Test 3 %s.\n", retval ? "passed" : "failed");
    fprintf(stderr,"%s", "Test 4...\n");
    retval = nhext_rpc(EXT_FID_TEST4, 1, EXT_STRING("Hello"),
      1, EXT_STRING_P(s));
    if (strcmp(s, "<Hello>"))
	retval = FALSE;
    fprintf(stderr, "Test 4 %s.\n", retval ? "passed" : "failed");
    fprintf(stderr,"%s", "Test 5...\n");
    req.n = 5;
    req.array = (long *)alloc(req.n * sizeof(long));
    for(i = 0; i < 5; i++)
	req.array[i] = 7 * i + 3;
    retval = nhext_rpc(EXT_FID_TEST5,
      1, EXT_XDRF(svc_xdr_test5_request, &req), 1, EXT_INT_P(total));
    for(i = 0; i < 5; i++)
	total -= req.array[i];
    if (total)
	retval = FALSE;
    free(req.array);
    fprintf(stderr, "Test 5 %s.\n", retval ? "passed" : "failed");
    fprintf(stderr,"%s", "Test 6...\n");
    retval = nhext_rpc(EXT_FID_TEST6,
      5, EXT_INT(37), EXT_INT(2), EXT_CHAR('l'), EXT_STRING("Shalom"),
         EXT_BOOLEAN(TRUE),
      5, EXT_INT_P(w), EXT_STRING_P(s), EXT_CHAR_P(c), EXT_INT_P(i),
         EXT_BOOLEAN_P(b));
    if (w != 2 || strcmp(s, "Shalom") || c != 'l' || i != 37 || !b)
	retval = FALSE;
    free(s);
    fprintf(stderr, "Test 6 %s.\n", retval ? "passed" : "failed");
    fprintf(stderr,"%s", "Test 7...\n");
    retval = nhext_rpc(EXT_FID_TEST7, 1, EXT_INT(11), 1, EXT_INT_P(i));
    if (i != 58)
	retval = FALSE;
    fprintf(stderr, "Test 7 %s.\n", retval ? "passed" : "failed");
}

main(argc, argv)
int argc;
char **argv;
{
    NhExtIO *rd, *wr;
    if (argc > 1 && !strcmp(argv[1], "-c")) {
	is_child++;
	server();
	exit(0);
    }
    if (!child_start(argv[0])) {
	fprintf(stderr,"%s", "Failed to start child.\n");
	exit(1);
    }
    rd = nhext_io_open(debug_read, get_child_readh(), NHEXT_IO_RDONLY);
    wr = nhext_io_open(debug_write, get_child_writeh(), NHEXT_IO_WRONLY);
    if (!rd || !wr) {
	fprintf(stderr,"%s", "Failed to open I/O streams.\n");
	exit(1);
    }
    (void)nhext_set_errhandler(nhext_error_handler);
    if (nhext_init(rd, wr, callbacks) < 0) {
	fprintf(stderr,"%s", "Failed to initialize NhExt.\n");
	exit(1);
    }
    if (nhext_set_protocol(1)) {
	fprintf(stderr,"%s", "Failed to select protocol 1.\n");
	exit(1);
    }
    run_tests();
    nhext_rpc(EXT_FID_EXIT, 0, 0);
    nhext_end();
    nhext_io_close(rd);
    nhext_io_close(wr);
    if (!child_wait()) {
	fprintf(stderr,"%s", "Error while waiting for child.\n");
	exit(1);
    }
    exit(0);
}
